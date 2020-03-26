#include <kernel.h>


#define EXT2_BGD_BLOCK 2

#define E_SUCCESS   0
#define E_BADBLOCK  1
#define E_NOSPACE   2
#define E_BADPARENT 3

#undef _symlink
#define _symlink(inode) ((char *)(inode)->block)

/*
 * EXT2 filesystem object
 */
typedef struct {
    ext2_superblock_t       * superblock;          /* Device superblock, contains important information */
    ext2_bgdescriptor_t     * block_groups;        /* Block Group Descriptor / Block groups */
    fs_node_t               * root_node;           /* Root FS node (attached to mountpoint) */

    fs_node_t               * block_device;        /* Block device node XXX unused */

    unsigned int              block_size;          /* Size of one block */
    unsigned int              pointers_per_block;  /* Number of pointers that fit in a block */
    unsigned int              inodes_per_group;    /* Number of inodes in a "group" */
    unsigned int              block_group_count;   /* Number of blocks groups */

    ext2_disk_cache_entry_t * disk_cache;          /* Dynamically allocated array of cache entries */
    unsigned int              cache_entries;       /* Size of ->disk_cache */
    unsigned int              cache_time;          /* "timer" that increments with each cache read/write */

    uint8_t                   bgd_block_span;
    uint8_t                   bgd_offset;
    unsigned int              inode_size;

    uint8_t *                 cache_data;

    int flags;
} ext2_fs_t;

#define EXT2_FLAG_NOCACHE 0x0001

/*
 * These macros were used in the original toaru ext2 driver.
 * They make referring to some of the core parts of the drive a bit easier.
 */
#define BGDS (this->block_group_count)
#define SB   (this->superblock)
#define BGD  (this->block_groups)
#define RN   (this->root_node)
#define DC   (this->disk_cache)

/*
 * These macros deal with the block group descriptor bitmap
 */
#define BLOCKBIT(n)  (bg_buffer[((n) >> 3)] & (1 << (((n) % 8))))
#define BLOCKBYTE(n) (bg_buffer[((n) >> 3)])
#define SETBIT(n)    (1 << (((n) % 8)))

uint32_t node_from_file(ext2_fs_t * this, ext2_inodetable_t *inode, ext2_dir_t *direntry,  fs_node_t *fnode);
uint32_t ext2_root(ext2_fs_t * this, ext2_inodetable_t *inode, fs_node_t *fnode);
ext2_inodetable_t * read_inode(ext2_fs_t * this, uint32_t inode);
void refresh_inode(ext2_fs_t * this, ext2_inodetable_t * inodet,  uint32_t inode);
int write_inode(ext2_fs_t * this, ext2_inodetable_t *inode, uint32_t index);
fs_node_t * finddir_ext2(fs_node_t *node, char *name);
unsigned int allocate_block(ext2_fs_t * this);

unsigned int get_cache_time(ext2_fs_t * this) {
    return this->cache_time++;
}

int cache_flush_dirty(ext2_fs_t * this, unsigned int ent_no) {
    write_fs(this->block_device, (DC[ent_no].block_no) * this->block_size, this->block_size, (uint8_t *)(DC[ent_no].block));
    DC[ent_no].dirty = 0;

    return E_SUCCESS;
}

int rewrite_superblock(ext2_fs_t * this) {
    write_fs(this->block_device, 1024, sizeof(ext2_superblock_t), (uint8_t *)SB);
    return E_SUCCESS;
}

int read_block(ext2_fs_t * this, unsigned int block_no, uint8_t * buf) {
    /* 0 is an invalid block number. So is anything beyond the total block count, but we can't check that. */
    if (!block_no) {
        return E_BADBLOCK;
    }

    /* We can make reads without a cache in place. */
    if (!DC) {
        /* In such cases, we read directly from the block device */
        read_fs(this->block_device, block_no * this->block_size, this->block_size, (uint8_t *)buf);
        /* And return SUCCESS */
        return E_SUCCESS;
    }

    /*
     * Search the cache for this entry
     * We'll look for the oldest entry, too.
     */
    int oldest = -1;
    unsigned int oldest_age = UINT32_MAX;
    for (unsigned int i = 0; i < this->cache_entries; ++i) {
        if (DC[i].block_no == block_no) {
            /* We found it! Update usage times */
            DC[i].last_use = get_cache_time(this);
            /* Read the block */
            memcpy(buf, DC[i].block, this->block_size);
            /* Success! */
            return E_SUCCESS;
        }
        if (DC[i].last_use < oldest_age) {
            /* We found an older block, remember this. */
            oldest = i;
            oldest_age = DC[i].last_use;
        }
    }

    /*
     * At this point, we did not find this block in the cache.
     * We are going to replace the oldest entry with this new one.
     */

    /* We'll start by flushing the block if it was dirty. */
    if (DC[oldest].dirty) {
        cache_flush_dirty(this, oldest);
    }

    /* Then we'll read the new one */
    read_fs(this->block_device, block_no * this->block_size, this->block_size, (uint8_t *)DC[oldest].block);

    /* And copy the results to the output buffer */
    memcpy(buf, DC[oldest].block, this->block_size);

    /* And update the cache entry to point to the new block */
    DC[oldest].block_no = block_no;
    DC[oldest].last_use = get_cache_time(this);
    DC[oldest].dirty = 0;

    /* And return success */
    return E_SUCCESS;
}

int write_block(ext2_fs_t * this, unsigned int block_no, uint8_t *buf) {
    if (!block_no) {
        serialLog("[x] Attempted to write to block #0. Enable tracing and retry this operation.\n");
        serialLog("[x] Your file system is most likely corrupted now.\n");
        return E_BADBLOCK;
    }


    if (!DC) {
        write_fs(this->block_device, block_no * this->block_size, this->block_size, buf);
        return E_SUCCESS;
    }

    /* Find the entry in the cache */
    int oldest = -1;
    unsigned int oldest_age = UINT32_MAX;
    for (unsigned int i = 0; i < this->cache_entries; ++i) {
        if (DC[i].block_no == block_no) {
            /* We found it. Update the cache entry */
            DC[i].last_use = get_cache_time(this);
            DC[i].dirty = 1;
            memcpy(DC[i].block, buf, this->block_size);
            return E_SUCCESS;
        }
        if (DC[i].last_use < oldest_age) {
            /* Keep track of the oldest entry */
            oldest = i;
            oldest_age = DC[i].last_use;
        }
    }

    /* We did not find this element in the cache, so make room. */
    if (DC[oldest].dirty) {
        /* Flush the oldest entry */
        cache_flush_dirty(this, oldest);
    }

    /* Update the entry */
    memcpy(DC[oldest].block, buf, this->block_size);
    DC[oldest].block_no = block_no;
    DC[oldest].last_use = get_cache_time(this);
    DC[oldest].dirty = 1;

    /* We're done. */
    return E_SUCCESS;
}

unsigned int ext2_sync(ext2_fs_t * this) {
    if (!this->disk_cache) return 0;

    /* Flush each cache entry. */
    for (unsigned int i = 0; i < this->cache_entries; ++i) {
        if (DC[i].dirty) {
            cache_flush_dirty(this, i);
        }
    }

    return 0;
}

unsigned int set_block_number(ext2_fs_t * this, ext2_inodetable_t * inode, unsigned int inode_no, unsigned int iblock, unsigned int rblock) {

    unsigned int p = this->pointers_per_block;

    /* We're going to do some crazy math in a bit... */
    unsigned int a, b, c, d, e, f, g;

    uint8_t * tmp;

    if (iblock < EXT2_DIRECT_BLOCKS) {
        inode->block[iblock] = rblock;
        return E_SUCCESS;
    } else if (iblock < EXT2_DIRECT_BLOCKS + p) {
        /* XXX what if inode->block[EXT2_DIRECT_BLOCKS] isn't set? */
        if (!inode->block[EXT2_DIRECT_BLOCKS]) {
            unsigned int block_no = allocate_block(this);
            if (!block_no) return E_NOSPACE;
            inode->block[EXT2_DIRECT_BLOCKS] = block_no;
            write_inode(this, inode, inode_no);
        }
        tmp = kmalloc(this->block_size);
        read_block(this, inode->block[EXT2_DIRECT_BLOCKS], (uint8_t *)tmp);

        ((uint32_t *)tmp)[iblock - EXT2_DIRECT_BLOCKS] = rblock;
        write_block(this, inode->block[EXT2_DIRECT_BLOCKS], (uint8_t *)tmp);

        free(tmp);
        return E_SUCCESS;
    } else if (iblock < EXT2_DIRECT_BLOCKS + p + p * p) {
        a = iblock - EXT2_DIRECT_BLOCKS;
        b = a - p;
        c = b / p;
        d = b - c * p;

        if (!inode->block[EXT2_DIRECT_BLOCKS+1]) {
            unsigned int block_no = allocate_block(this);
            if (!block_no) return E_NOSPACE;
            inode->block[EXT2_DIRECT_BLOCKS+1] = block_no;
            write_inode(this, inode, inode_no);
        }

        tmp = kmalloc(this->block_size);
        read_block(this, inode->block[EXT2_DIRECT_BLOCKS + 1], (uint8_t *)tmp);

        if (!((uint32_t *)tmp)[c]) {
            unsigned int block_no = allocate_block(this);
            if (!block_no) goto no_space_free;
            ((uint32_t *)tmp)[c] = block_no;
            write_block(this, inode->block[EXT2_DIRECT_BLOCKS + 1], (uint8_t *)tmp);
        }

        uint32_t nblock = ((uint32_t *)tmp)[c];
        read_block(this, nblock, (uint8_t *)tmp);

        ((uint32_t  *)tmp)[d] = rblock;
        write_block(this, nblock, (uint8_t *)tmp);

        free(tmp);
        return E_SUCCESS;
    } else if (iblock < EXT2_DIRECT_BLOCKS + p + p * p + p) {
        a = iblock - EXT2_DIRECT_BLOCKS;
        b = a - p;
        c = b - p * p;
        d = c / (p * p);
        e = c - d * p * p;
        f = e / p;
        g = e - f * p;

        if (!inode->block[EXT2_DIRECT_BLOCKS+2]) {
            unsigned int block_no = allocate_block(this);
            if (!block_no) return E_NOSPACE;
            inode->block[EXT2_DIRECT_BLOCKS+2] = block_no;
            write_inode(this, inode, inode_no);
        }

        tmp = kmalloc(this->block_size);
        read_block(this, inode->block[EXT2_DIRECT_BLOCKS + 2], (uint8_t *)tmp);

        if (!((uint32_t *)tmp)[d]) {
            unsigned int block_no = allocate_block(this);
            if (!block_no) goto no_space_free;
            ((uint32_t *)tmp)[d] = block_no;
            write_block(this, inode->block[EXT2_DIRECT_BLOCKS + 2], (uint8_t *)tmp);
        }

        uint32_t nblock = ((uint32_t *)tmp)[d];
        read_block(this, nblock, (uint8_t *)tmp);

        if (!((uint32_t *)tmp)[f]) {
            unsigned int block_no = allocate_block(this);
            if (!block_no) goto no_space_free;
            ((uint32_t *)tmp)[f] = block_no;
            write_block(this, nblock, (uint8_t *)tmp);
        }

        nblock = ((uint32_t *)tmp)[f];
        read_block(this, nblock, (uint8_t *)tmp);

        ((uint32_t *)tmp)[g] = nblock;
        write_block(this, nblock, (uint8_t *)tmp);

        free(tmp);
        return E_SUCCESS;
    }

    serialLog("[!] EXT2 driver tried to write to a block number that was too high (%d)", rblock);
    return E_BADBLOCK;
    no_space_free:
    free(tmp);
    return E_NOSPACE;
}

unsigned int get_block_number(ext2_fs_t * this, ext2_inodetable_t * inode, unsigned int iblock) {

    unsigned int p = this->pointers_per_block;

    /* We're going to do some crazy math in a bit... */
    unsigned int a, b, c, d, e, f, g;

    uint8_t * tmp;

    if (iblock < EXT2_DIRECT_BLOCKS) {
        return inode->block[iblock];
    } else if (iblock < EXT2_DIRECT_BLOCKS + p) {
        /* XXX what if inode->block[EXT2_DIRECT_BLOCKS] isn't set? */
        tmp = kmalloc(this->block_size);
        read_block(this, inode->block[EXT2_DIRECT_BLOCKS], (uint8_t *)tmp);

        unsigned int out = ((uint32_t *)tmp)[iblock - EXT2_DIRECT_BLOCKS];
        free(tmp);
        return out;
    } else if (iblock < EXT2_DIRECT_BLOCKS + p + p * p) {
        a = iblock - EXT2_DIRECT_BLOCKS;
        b = a - p;
        c = b / p;
        d = b - c * p;

        tmp = kmalloc(this->block_size);
        read_block(this, inode->block[EXT2_DIRECT_BLOCKS + 1], (uint8_t *)tmp);

        uint32_t nblock = ((uint32_t *)tmp)[c];
        read_block(this, nblock, (uint8_t *)tmp);

        unsigned int out = ((uint32_t  *)tmp)[d];
        free(tmp);
        return out;
    } else if (iblock < EXT2_DIRECT_BLOCKS + p + p * p + p) {
        a = iblock - EXT2_DIRECT_BLOCKS;
        b = a - p;
        c = b - p * p;
        d = c / (p * p);
        e = c - d * p * p;
        f = e / p;
        g = e - f * p;

        tmp = kmalloc(this->block_size);
        read_block(this, inode->block[EXT2_DIRECT_BLOCKS + 2], (uint8_t *)tmp);

        uint32_t nblock = ((uint32_t *)tmp)[d];
        read_block(this, nblock, (uint8_t *)tmp);

        nblock = ((uint32_t *)tmp)[f];
        read_block(this, nblock, (uint8_t *)tmp);

        unsigned int out = ((uint32_t  *)tmp)[g];
        free(tmp);
        return out;
    }

    serialLog("[!]EXT2 driver tried to read to a block number that was too high (%d)", iblock);

    return 0;
}

int write_inode(ext2_fs_t * this, ext2_inodetable_t *inode, uint32_t index) {
    uint32_t group = index / this->inodes_per_group;
    if (group > BGDS) {
        return E_BADBLOCK;
    }

    uint32_t inode_table_block = BGD[group].inode_table;
    index -= group * this->inodes_per_group;
    uint32_t block_offset = ((index - 1) * this->inode_size) / this->block_size;
    uint32_t offset_in_block = (index - 1) - block_offset * (this->block_size / this->inode_size);

    ext2_inodetable_t *inodet = kmalloc(this->block_size);
    /* Read the current table block */
    read_block(this, inode_table_block + block_offset, (uint8_t *)inodet);
    memcpy((uint8_t *)((uint32_t)inodet + offset_in_block * this->inode_size), inode, this->inode_size);
    write_block(this, inode_table_block + block_offset, (uint8_t *)inodet);
    free(inodet);

    return E_SUCCESS;
}

unsigned int allocate_block(ext2_fs_t * this) {
    unsigned int block_no     = 0;
    unsigned int block_offset = 0;
    unsigned int group        = 0;
    uint8_t * bg_buffer = kmalloc(this->block_size);

    for (unsigned int i = 0; i < BGDS; ++i) {
        if (BGD[i].free_blocks_count > 0) {
            read_block(this, BGD[i].block_bitmap, (uint8_t *)bg_buffer);
            while (BLOCKBIT(block_offset)) {
                ++block_offset;
            }
            block_no = block_offset + SB->blocks_per_group * i;
            group = i;
            break;
        }
    }

    if (!block_no) {
        serialLog("[!] No available blocks, disk is out of space!");
        free(bg_buffer);
        return 0;
    }

    serialLog("[x] allocating block #%d (group %d)", block_no, group);

    BLOCKBYTE(block_offset) |= SETBIT(block_offset);
    write_block(this, BGD[group].block_bitmap, (uint8_t *)bg_buffer);

    BGD[group].free_blocks_count--;
    for (int i = 0; i < this->bgd_block_span; ++i) {
        write_block(this, this->bgd_offset + i, (uint8_t *)((uint32_t)BGD + this->block_size * i));
    }

    SB->free_blocks_count--;
    rewrite_superblock(this);

    memset(bg_buffer, 0x00, this->block_size);
    write_block(this, block_no, bg_buffer);

    free(bg_buffer);

    return block_no;

}

int allocate_inode_block(ext2_fs_t * this, ext2_inodetable_t * inode, unsigned int inode_no, unsigned int block) {
    serialLog("[*] Allocating block #%d for inode #%d", block, inode_no);
    unsigned int block_no = allocate_block(this);

    if (!block_no) return E_NOSPACE;

    set_block_number(this, inode, inode_no, block, block_no);

    unsigned int t = (block + 1) * (this->block_size / 512);
    if (inode->blocks < t) {
        serialLog("[*] Setting inode->blocks to %d = (%d fs blocks)", t, t / (this->block_size / 512));
        inode->blocks = t;
    }
    write_inode(this, inode, inode_no);

    return E_SUCCESS;
}

unsigned int inode_read_block(ext2_fs_t * this, ext2_inodetable_t * inode, unsigned int block, uint8_t * buf) {

    if (block >= inode->blocks / (this->block_size / 512)) {
        memset(buf, 0x00, this->block_size);
        serialLog("[x] Tried to read an invalid block. Asked for %d (0-indexed), but inode only has %d!", block, inode->blocks / (this->block_size / 512));
        return 0;
    }

    unsigned int real_block = get_block_number(this, inode, block);
    read_block(this, real_block, buf);

    return real_block;
}

unsigned int inode_write_block(ext2_fs_t * this, ext2_inodetable_t * inode, unsigned int inode_no, unsigned int block, uint8_t * buf) {
    if (block >= inode->blocks / (this->block_size / 512)) {
        serialLog("[x] Attempting to write beyond the existing allocated blocks for this inode.");
        serialLog("[x] Inode %d, Block %d", inode_no, block);
    }

    serialLog("[x] clearing and allocating up to required blocks (block=%d, %d)", block, inode->blocks);
    char * empty = NULL;
    while (block >= inode->blocks / (this->block_size / 512)) {
        allocate_inode_block(this, inode, inode_no, inode->blocks / (this->block_size / 512));
        refresh_inode(this, inode, inode_no);
    }
    if (empty) free(empty);
    serialLog("[x] ... done");

    unsigned int real_block = get_block_number(this, inode, block);
    serialLog("[x] Writing virtual block %d for inode %d maps to real block %d", block, inode_no, real_block);

    write_block(this, real_block, buf);
    return real_block;
}

int create_entry(fs_node_t * parent, char * name, uint32_t inode) {
    ext2_fs_t * this = (ext2_fs_t *)parent->device;

    ext2_inodetable_t * pinode = read_inode(this,parent->inode);
    if (((pinode->mode & EXT2_S_IFDIR) == 0) || (name == NULL)) {
        serialLog("[x] Attempted to allocate an inode in a parent that was not a directory.");
        return E_BADPARENT;
    }

    serialLog("[x] Creating a directory entry for %s pointing to inode %d.", name, inode);

    /* okay, how big is it... */

    serialLog("[x] We need to append %d bytes to the direcotry.", sizeof(ext2_dir_t) + strlen(name));

    unsigned int rec_len = sizeof(ext2_dir_t) + strlen(name);
    rec_len += (rec_len % 4) ? (4 - (rec_len % 4)) : 0;

    serialLog("[x] Our directory entry looks like this:");
    serialLog("[x]   inode     = %d", inode);
    serialLog("[x]   rec_len   = %d", rec_len);
    serialLog("[x]   name_len  = %d", strlen(name));
    serialLog("[x]   file_type = %d", 0);
    serialLog("[x]   name      = %s", name);

    serialLog("[x] The inode size is marked as: %d", pinode->size);
    serialLog("[x] Block size is %d", this->block_size);

    uint8_t * block = kmalloc(this->block_size);
    uint8_t block_nr = 0;
    uint32_t dir_offset = 0;
    uint32_t total_offset = 0;
    int modify_or_replace = 0;
    ext2_dir_t *previous;

    inode_read_block(this, pinode, block_nr, block);
    while (total_offset < pinode->size) {
        if (dir_offset >= this->block_size) {
            block_nr++;
            dir_offset -= this->block_size;
            inode_read_block(this, pinode, block_nr, block);
        }
        ext2_dir_t *d_ent = (ext2_dir_t *)((uintptr_t)block + dir_offset);

        unsigned int sreclen = d_ent->name_len + sizeof(ext2_dir_t);
        sreclen += (sreclen % 4) ? (4 - (sreclen % 4)) : 0;

        {
            char f[d_ent->name_len+1];
            memcpy(f, d_ent->name, d_ent->name_len);
            f[d_ent->name_len] = 0;
            serialLog("[x]  * file: %s", f);
        }
        serialLog("[x]    rec_len: %d", d_ent->rec_len);
        serialLog("[x]    type: %d", d_ent->file_type);
        serialLog("[x]    namel: %d", d_ent->name_len);
        serialLog("[x]    inode: %d", d_ent->inode);

        if (d_ent->rec_len != sreclen && total_offset + d_ent->rec_len == pinode->size) {
            serialLog("[x]   - should be %d, but instead points to end of block", sreclen);
            serialLog("[x]   - we've hit the end, should change this pointer");

            dir_offset += sreclen;
            total_offset += sreclen;

            modify_or_replace = 1; /* Modify */
            previous = d_ent;

            break;
        }

        if (d_ent->inode == 0) {
            modify_or_replace = 2; /* Replace */
        }

        dir_offset += d_ent->rec_len;
        total_offset += d_ent->rec_len;
    }

    if (!modify_or_replace) {
        serialLog("[x] That's odd, this shouldn't have happened, we made it all the way here without hitting our two end conditions?");
    }

    if (modify_or_replace == 1) {
        serialLog("[x] The last node in the list is a real node, we need to modify it.");

        if (dir_offset + rec_len >= this->block_size) {
            serialLog("[x] Need to allocate more space, bail!");
            free(block);
            return E_NOSPACE;
        } else {
            unsigned int sreclen = previous->name_len + sizeof(ext2_dir_t);
            sreclen += (sreclen % 4) ? (4 - (sreclen % 4)) : 0;
            previous->rec_len = sreclen;
            serialLog("[x] Set previous node rec_len to %d", sreclen);
        }

    } else if (modify_or_replace == 2) {
        serialLog("[x] The last node in the list is a fake node, we'll replace it.");
    }

    serialLog("[x]  total_offset = 0x%x", total_offset);
    serialLog("[x]    dir_offset = 0x%x", dir_offset);
    ext2_dir_t *d_ent = (ext2_dir_t *)((uintptr_t)block + dir_offset);

    d_ent->inode     = inode;
    d_ent->rec_len   = this->block_size - dir_offset;
    d_ent->name_len  = strlen(name);
    d_ent->file_type = 0; /* This is unused */
    memcpy(d_ent->name, name, strlen(name));

    inode_write_block(this, pinode, parent->inode, block_nr, block);

    free(block);
    free(pinode);


    return E_NOSPACE;
}

unsigned int allocate_inode(ext2_fs_t * this) {
    uint32_t node_no     = 0;
    uint32_t node_offset = 0;
    uint32_t group       = 0;
    uint8_t * bg_buffer  = kmalloc(this->block_size);

    for (unsigned int i = 0; i < BGDS; ++i) {
        if (BGD[i].free_inodes_count > 0) {
            serialLog("[*] Group %d has %d free inodes.", i, BGD[i].free_inodes_count);
            read_block(this, BGD[i].inode_bitmap, (uint8_t *)bg_buffer);
            while (BLOCKBIT(node_offset)) {
                node_offset++;
            }
            node_no = node_offset + i * this->inodes_per_group + 1;
            group = i;
            break;
        }
    }
    if (!node_no) {
        serialLog("[!] Ran out of inodes!");
        return 0;
    }

    BLOCKBYTE(node_offset) |= SETBIT(node_offset);

    write_block(this, BGD[group].inode_bitmap, (uint8_t *)bg_buffer);
    free(bg_buffer);

    BGD[group].free_inodes_count--;
    for (int i = 0; i < this->bgd_block_span; ++i) {
        write_block(this, this->bgd_offset + i, (uint8_t *)((uint32_t)BGD + this->block_size * i));
    }

    SB->free_inodes_count--;
    rewrite_superblock(this);

    return node_no;
}


int mkdir_ext2(fs_node_t * parent, char * name) {
    if (!name) return -EINVAL;

    ext2_fs_t * this = parent->device;

    /* first off, check if it exists */
    fs_node_t * check = finddir_ext2(parent, name);
    if (check) {
        serialLog("[x] A file by this name already exists: %s", name);
        free(check);
        return -EEXIST;
    }

    /* Allocate an inode for it */
    unsigned int inode_no = allocate_inode(this);
    ext2_inodetable_t * inode = read_inode(this,inode_no);

    /* Set the access and creation times to now */
    inode->atime = now();
    inode->ctime = inode->atime;
    inode->mtime = inode->atime;
    inode->dtime = 0; /* This inode was never deleted */

    /* Empty the file */
    memset(inode->block, 0x00, sizeof(inode->block));
    inode->blocks = 0;
    inode->size = 0; /* empty */

    /* misc */
    inode->faddr = 0;
    inode->links_count = 2; /* There's the parent's pointer to us, and our pointer to us. */
    inode->flags = 0;
    inode->osd1 = 0;
    inode->generation = 0;
    inode->file_acl = 0;
    inode->dir_acl = 0;

    /* File mode */
    inode->mode = EXT2_S_IFDIR;
    inode->mode |= 0xFFF;

    /* Write the osd blocks to 0 */
    memset(inode->osd2, 0x00, sizeof(inode->osd2));

    /* Write out inode changes */
    write_inode(this, inode, inode_no);

    /* Now append the entry to the parent */
    create_entry(parent, name, inode_no);

    inode->size = this->block_size;
    write_inode(this, inode, inode_no);

    uint8_t * tmp = kmalloc(this->block_size);

    ext2_dir_t * t = kmalloc(12);
    memset(t, 0, 12);
    t->inode = inode_no;
    t->rec_len = 12;
    t->name_len = 1;
    t->name[0] = '.';
    memcpy(&tmp[0], t, 12);
    t->inode = parent->inode;
    t->name_len = 2;
    t->name[1] = '.';
    t->rec_len = this->block_size - 12;
    memcpy(&tmp[12], t, 12);
    free(t);

    inode_write_block(this, inode, inode_no, 0, tmp);

    free(inode);
    free(tmp);

    /* Update parent link count */
    ext2_inodetable_t * pinode = read_inode(this, parent->inode);
    pinode->links_count++;
    write_inode(this, pinode, parent->inode);
    free(pinode);

    /* Update directory count in block group descriptor */
    uint32_t group = inode_no / this->inodes_per_group;
    BGD[group].used_dirs_count++;
    for (int i = 0; i < this->bgd_block_span; ++i) {
        write_block(this, this->bgd_offset + i, (uint8_t *)((uint32_t)BGD + this->block_size * i));
    }

    ext2_sync(this);

    return 0;
}

int create_ext2(fs_node_t * parent, char * name) {
    if (!name) return -EINVAL;

    ext2_fs_t * this = parent->device;

    /* first off, check if it exists */
    fs_node_t * check = finddir_ext2(parent, name);
    if (check) {
        serialLog("[x] A file by this name already exists: %s", name);
        free(check);
        return -EEXIST;
    }

    /* Allocate an inode for it */
    unsigned int inode_no = allocate_inode(this);
    ext2_inodetable_t * inode = read_inode(this,inode_no);

    /* Set the access and creation times to now */
    inode->atime = now();
    inode->ctime = inode->atime;
    inode->mtime = inode->atime;
    inode->dtime = 0; /* This inode was never deleted */

    /* Empty the file */
    memset(inode->block, 0x00, sizeof(inode->block));
    inode->blocks = 0;
    inode->size = 0; /* empty */

    /* misc */
    inode->faddr = 0;
    inode->links_count = 1; /* The one we're about to create. */
    inode->flags = 0;
    inode->osd1 = 0;
    inode->generation = 0;
    inode->file_acl = 0;
    inode->dir_acl = 0;

    /* File mode */
    /* TODO: Use the mask from `permission` */
    inode->mode = EXT2_S_IFREG;
    inode->mode |= 0xFFF;

    /* Write the osd blocks to 0 */
    memset(inode->osd2, 0x00, sizeof(inode->osd2));

    /* Write out inode changes */
    write_inode(this, inode, inode_no);

    /* Now append the entry to the parent */
    create_entry(parent, name, inode_no);

    free(inode);

    ext2_sync(this);

    return 0;
}

ext2_dir_t * direntry_ext2(ext2_fs_t * this, ext2_inodetable_t * inode, uint32_t no, uint32_t index) {
    uint8_t *block = kmalloc(this->block_size);
    uint8_t block_nr = 0;
    inode_read_block(this, inode, block_nr, block);
    uint32_t dir_offset = 0;
    uint32_t total_offset = 0;
    uint32_t dir_index = 0;

    while (total_offset < inode->size && dir_index <= index) {
        ext2_dir_t *d_ent = (ext2_dir_t *)((uintptr_t)block + dir_offset);

        if (d_ent->inode != 0 && dir_index == index) {
            ext2_dir_t *out = kmalloc(d_ent->rec_len);
            memcpy(out, d_ent, d_ent->rec_len);
            free(block);
            return out;
        }

        dir_offset += d_ent->rec_len;
        total_offset += d_ent->rec_len;

        if (d_ent->inode) {
            dir_index++;
        }

        if (dir_offset >= this->block_size) {
            block_nr++;
            dir_offset -= this->block_size;
            inode_read_block(this, inode, block_nr, block);
        }
    }

    free(block);
    return NULL;
}

fs_node_t * finddir_ext2(fs_node_t *node, char *name) {

    ext2_fs_t * this = (ext2_fs_t *)node->device;

    ext2_inodetable_t *inode = read_inode(this,node->inode);
    assert(inode->mode & EXT2_S_IFDIR);
    uint8_t * block = kmalloc(this->block_size);
    ext2_dir_t *direntry = NULL;
    uint8_t block_nr = 0;
    inode_read_block(this, inode, block_nr, block);
    uint32_t dir_offset = 0;
    uint32_t total_offset = 0;

    while (total_offset < inode->size) {
        if (dir_offset >= this->block_size) {
            block_nr++;
            dir_offset -= this->block_size;
            inode_read_block(this, inode, block_nr, block);
        }
        ext2_dir_t *d_ent = (ext2_dir_t *)((uintptr_t)block + dir_offset);

        if (d_ent->inode == 0 || strlen(name) != d_ent->name_len) {
            dir_offset += d_ent->rec_len;
            total_offset += d_ent->rec_len;

            continue;
        }

        char *dname = kmalloc(sizeof(char) * (d_ent->name_len + 1));
        memcpy(dname, &(d_ent->name), d_ent->name_len);
        dname[d_ent->name_len] = '\0';
        if (!strcmp(dname, name)) {
            free(dname);
            direntry = kmalloc(d_ent->rec_len);
            memcpy(direntry, d_ent, d_ent->rec_len);
            break;
        }
        free(dname);

        dir_offset += d_ent->rec_len;
        total_offset += d_ent->rec_len;
    }
    free(inode);
    if (!direntry) {
        free(block);
        return NULL;
    }
    fs_node_t *outnode = kmalloc(sizeof(fs_node_t));
    memset(outnode, 0, sizeof(fs_node_t));

    inode = read_inode(this, direntry->inode);

    if (!node_from_file(this, inode, direntry, outnode)) {
        serialLog("[!] Oh dear. Couldn't allocate the outnode?");
    }

    free(direntry);
    free(inode);
    free(block);
    return outnode;
}

int unlink_ext2(fs_node_t * node, char * name) {
    /* XXX this is a very bad implementation */
    ext2_fs_t * this = (ext2_fs_t *)node->device;

    ext2_inodetable_t *inode = read_inode(this,node->inode);
    assert(inode->mode & EXT2_S_IFDIR);
    uint8_t * block = kmalloc(this->block_size);
    ext2_dir_t *direntry = NULL;
    uint8_t block_nr = 0;
    inode_read_block(this, inode, block_nr, block);
    uint32_t dir_offset = 0;
    uint32_t total_offset = 0;

    while (total_offset < inode->size) {
        if (dir_offset >= this->block_size) {
            block_nr++;
            dir_offset -= this->block_size;
            inode_read_block(this, inode, block_nr, block);
        }
        ext2_dir_t *d_ent = (ext2_dir_t *)((uintptr_t)block + dir_offset);

        if (d_ent->inode == 0 || strlen(name) != d_ent->name_len) {
            dir_offset += d_ent->rec_len;
            total_offset += d_ent->rec_len;

            continue;
        }

        char *dname = kmalloc(sizeof(char) * (d_ent->name_len + 1));
        memcpy(dname, &(d_ent->name), d_ent->name_len);
        dname[d_ent->name_len] = '\0';
        if (!strcmp(dname, name)) {
            free(dname);
            direntry = d_ent;
            break;
        }
        free(dname);

        dir_offset += d_ent->rec_len;
        total_offset += d_ent->rec_len;
    }
    free(inode);
    if (!direntry) {
        free(block);
        return -ENOENT;
    }

    direntry->inode = 0;

    inode_write_block(this, inode, node->inode, block_nr, block);
    free(block);

    ext2_sync(this);

    return 0;
}

void refresh_inode(ext2_fs_t * this, ext2_inodetable_t * inodet,  uint32_t inode) {
    uint32_t group = inode / this->inodes_per_group;
    if (group > BGDS) {
        return;
    }
    uint32_t inode_table_block = BGD[group].inode_table;
    inode -= group * this->inodes_per_group;	// adjust index within group
    uint32_t block_offset		= ((inode - 1) * this->inode_size) / this->block_size;
    uint32_t offset_in_block    = (inode - 1) - block_offset * (this->block_size / this->inode_size);

    uint8_t * buf = kmalloc(this->block_size);

    read_block(this, inode_table_block + block_offset, buf);

    ext2_inodetable_t *inodes = (ext2_inodetable_t *)buf;

    memcpy(inodet, (uint8_t *)((uint32_t)inodes + offset_in_block * this->inode_size), this->inode_size);

    free(buf);
}

ext2_inodetable_t * read_inode(ext2_fs_t * this, uint32_t inode) {
    ext2_inodetable_t *inodet   = kmalloc(this->inode_size);
    refresh_inode(this, inodet, inode);
    return inodet;
}

uint32_t read_ext2(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    ext2_fs_t * this = (ext2_fs_t *)node->device;
    ext2_inodetable_t * inode = read_inode(this, node->inode);
    uint32_t end;
    if (inode->size == 0) return 0;
    if (offset + size > inode->size) {
        end = inode->size;
    } else {
        end = offset + size;
    }
    uint32_t start_block  = offset / this->block_size;
    uint32_t end_block    = end / this->block_size;
    uint32_t end_size     = end - end_block * this->block_size;
    uint32_t size_to_read = end - offset;

    uint8_t * buf = kmalloc(this->block_size);
    if (start_block == end_block) {
        inode_read_block(this, inode, start_block, buf);
        memcpy(buffer, (uint8_t *)(((uint32_t)buf) + (offset % this->block_size)), size_to_read);
    } else {
        uint32_t block_offset;
        uint32_t blocks_read = 0;
        for (block_offset = start_block; block_offset < end_block; block_offset++, blocks_read++) {
            if (block_offset == start_block) {
                inode_read_block(this, inode, block_offset, buf);
                memcpy(buffer, (uint8_t *)(((uint32_t)buf) + (offset % this->block_size)), this->block_size - (offset % this->block_size));
            } else {
                inode_read_block(this, inode, block_offset, buf);
                memcpy(buffer + this->block_size * blocks_read - (offset % this->block_size), buf, this->block_size);
            }
        }
        if (end_size) {
            inode_read_block(this, inode, end_block, buf);
            memcpy(buffer + this->block_size * blocks_read - (offset % this->block_size), buf, end_size);
        }
    }
    free(inode);
    free(buf);
    return size_to_read;
}

uint32_t write_inode_buffer(ext2_fs_t * this, ext2_inodetable_t * inode, uint32_t inode_number, uint32_t offset, uint32_t size, uint8_t *buffer) {
    uint32_t end = offset + size;
    if (end > inode->size) {
        inode->size = end;
        write_inode(this, inode, inode_number);
    }

    uint32_t start_block  = offset / this->block_size;
    uint32_t end_block    = end / this->block_size;
    uint32_t end_size     = end - end_block * this->block_size;
    uint32_t size_to_read = end - offset;
    uint8_t * buf = kmalloc(this->block_size);
    if (start_block == end_block) {
        inode_read_block(this, inode, start_block, buf);
        memcpy((uint8_t *)(((uint32_t)buf) + (offset % this->block_size)), buffer, size_to_read);
        inode_write_block(this, inode, inode_number, start_block, buf);
    } else {
        uint32_t block_offset;
        uint32_t blocks_read = 0;
        for (block_offset = start_block; block_offset < end_block; block_offset++, blocks_read++) {
            if (block_offset == start_block) {
                int b = inode_read_block(this, inode, block_offset, buf);
                memcpy((uint8_t *)(((uint32_t)buf) + (offset % this->block_size)), buffer, this->block_size - (offset % this->block_size));
                inode_write_block(this, inode, inode_number, block_offset, buf);
                if (!b) {
                    refresh_inode(this, inode, inode_number);
                }
            } else {
                int b = inode_read_block(this, inode, block_offset, buf);
                memcpy(buf, buffer + this->block_size * blocks_read - (offset % this->block_size), this->block_size);
                inode_write_block(this, inode, inode_number, block_offset, buf);
                if (!b) {
                    refresh_inode(this, inode, inode_number);
                }
            }
        }
        if (end_size) {
            inode_read_block(this, inode, end_block, buf);
            memcpy(buf, buffer + this->block_size * blocks_read - (offset % this->block_size), end_size);
            inode_write_block(this, inode, inode_number, end_block, buf);
        }
    }
    free(buf);
    return size_to_read;
}

uint32_t write_ext2(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    ext2_fs_t * this = (ext2_fs_t *)node->device;
    ext2_inodetable_t * inode = read_inode(this, node->inode);

    uint32_t rv = write_inode_buffer(this, inode, node->inode, offset, size, buffer);
    free(inode);
    return rv;
}

void open_ext2(fs_node_t *node, unsigned int flags) {
    ext2_fs_t * this = node->device;

    if (flags & O_TRUNC) {
        /* Uh, herp */
        ext2_inodetable_t * inode = read_inode(this,node->inode);
        inode->size = 0;
        write_inode(this, inode, node->inode);
    }
}

void close_ext2(fs_node_t *node) {
    /* Nothing to do here */
}

struct dirent * readdir_ext2(fs_node_t *node, uint32_t index) {

    ext2_fs_t * this = (ext2_fs_t *)node->device;

    ext2_inodetable_t *inode = read_inode(this, node->inode);
    assert(inode->mode & EXT2_S_IFDIR);
    ext2_dir_t *direntry = direntry_ext2(this, inode, node->inode, index);
    if (!direntry) {
        free(inode);
        return NULL;
    }
    struct dirent *dirent = kmalloc(sizeof(struct dirent));
    memcpy(&dirent->name, &direntry->name, direntry->name_len);
    dirent->name[direntry->name_len] = '\0';
    dirent->ino = direntry->inode;
    free(direntry);
    free(inode);
    return dirent;
}

int symlink_ext2(fs_node_t * parent, char * target, char * name) {
    if (!name) return -EINVAL;

    ext2_fs_t * this = parent->device;

    /* first off, check if it exists */
    fs_node_t * check = finddir_ext2(parent, name);
    if (check) {
        serialLog("[x] A file by this name already exists: %s", name);
        free(check);
        return -EEXIST; /* this should probably have a return value... */
    }

    /* Allocate an inode for it */
    unsigned int inode_no = allocate_inode(this);
    ext2_inodetable_t * inode = read_inode(this,inode_no);

    /* Set the access and creation times to now */
    inode->atime = now();
    inode->ctime = inode->atime;
    inode->mtime = inode->atime;
    inode->dtime = 0; /* This inode was never deleted */

    /* Empty the file */
    memset(inode->block, 0x00, sizeof(inode->block));
    inode->blocks = 0;
    inode->size = 0; /* empty */

    /* misc */
    inode->faddr = 0;
    inode->links_count = 1; /* The one we're about to create. */
    inode->flags = 0;
    inode->osd1 = 0;
    inode->generation = 0;
    inode->file_acl = 0;
    inode->dir_acl = 0;

    inode->mode = EXT2_S_IFLNK;

    /* I *think* this is what you're supposed to do with symlinks */
    inode->mode |= 0777;

    /* Write the osd blocks to 0 */
    memset(inode->osd2, 0x00, sizeof(inode->osd2));

    size_t target_len = strlen(target);
    int embedded = target_len <= 60; // sizeof(_symlink(inode));
    if (embedded) {
        memcpy(_symlink(inode), target, target_len);
        inode->size = target_len;
    }

    /* Write out inode changes */
    write_inode(this, inode, inode_no);

    /* Now append the entry to the parent */
    create_entry(parent, name, inode_no);


    /* If we didn't embed it in the inode just use write_inode_buffer to finish the job */
    if (!embedded) {
        write_inode_buffer(parent->device, inode, inode_no, 0, target_len, (uint8_t *)target);
    }
    free(inode);

    ext2_sync(this);

    return 0;
}

int readlink_ext2(fs_node_t * node, char * buf, size_t size) {
    ext2_fs_t * this = (ext2_fs_t *)node->device;
    ext2_inodetable_t * inode = read_inode(this, node->inode);
    size_t read_size = inode->size < size ? inode->size : size;
    if (inode->size > 60) { //sizeof(_symlink(inode))) {
        read_ext2(node, 0, read_size, (uint8_t *)buf);
    } else {
        memcpy(buf, _symlink(inode), read_size);
    }

    /* Believe it or not, we actually aren't supposed to include the nul in the length. */
    if (read_size < size) {
        buf[read_size] = '\0';
    }

    free(inode);
    return read_size;
}



uint32_t node_from_file(ext2_fs_t * this, ext2_inodetable_t *inode, ext2_dir_t *direntry,  fs_node_t *fnode) {
    if (!fnode) {
        /* You didn't give me a node to write into, go **** yourself */
        return 0;
    }
    /* Information from the direntry */
    fnode->device = (void *)this;
    fnode->inode = direntry->inode;
    memcpy(&fnode->name, &direntry->name, direntry->name_len);
    fnode->name[direntry->name_len] = '\0';
    /* Information from the inode */
    fnode->uid = inode->uid;
    fnode->gid = inode->gid;
    fnode->length = inode->size;
    fnode->mask = inode->mode & 0xFFF;
    fnode->nlink = inode->links_count;
    /* File Flags */
    fnode->flags = 0;
    if ((inode->mode & EXT2_S_IFREG) == EXT2_S_IFREG) {
        fnode->flags   |= FS_FILE;
        fnode->read     = read_ext2;
        fnode->write    = write_ext2;
        fnode->create   = NULL;
        fnode->mkdir    = NULL;
        fnode->readdir  = NULL;
        fnode->finddir  = NULL;
        fnode->symlink  = NULL;
        fnode->readlink = NULL;
    }
    if ((inode->mode & EXT2_S_IFDIR) == EXT2_S_IFDIR) {
        fnode->flags   |= FS_DIRECTORY;
        fnode->create   = create_ext2;
        fnode->mkdir    = mkdir_ext2;
        fnode->readdir  = readdir_ext2;
        fnode->finddir  = finddir_ext2;
        fnode->unlink   = unlink_ext2;
        fnode->write    = NULL;
        fnode->symlink  = symlink_ext2;
        fnode->readlink = NULL;
    }
    if ((inode->mode & EXT2_S_IFBLK) == EXT2_S_IFBLK) {
        fnode->flags |= FS_BLOCKDEVICE;
    }
    if ((inode->mode & EXT2_S_IFCHR) == EXT2_S_IFCHR) {
        fnode->flags |= FS_CHARDEVICE;
    }
    if ((inode->mode & EXT2_S_IFIFO) == EXT2_S_IFIFO) {
        fnode->flags |= FS_PIPE;
    }
    if ((inode->mode & EXT2_S_IFLNK) == EXT2_S_IFLNK) {
        fnode->flags   |= FS_SYMLINK;
        fnode->read     = NULL;
        fnode->write    = NULL;
        fnode->create   = NULL;
        fnode->mkdir    = NULL;
        fnode->readdir  = NULL;
        fnode->finddir  = NULL;
        fnode->readlink = readlink_ext2;
    }

    fnode->atime   = inode->atime;
    fnode->mtime   = inode->mtime;
    fnode->ctime   = inode->ctime;
    serialLog("[*]file a/m/c times are %d/%d/%d", fnode->atime, fnode->mtime, fnode->ctime);

    fnode->open    = open_ext2;
    fnode->close   = close_ext2;
    fnode->ioctl   = NULL;
    return 1;
}

uint32_t ext2_root(ext2_fs_t * this, ext2_inodetable_t *inode, fs_node_t *fnode) {
    if (!fnode) {
        return 0;
    }
    /* Information for root dir */
    fnode->device = (void *)this;
    fnode->inode = 2;
    fnode->name[0] = '/';
    fnode->name[1] = '\0';
    /* Information from the inode */
    fnode->uid = inode->uid;
    fnode->gid = inode->gid;
    fnode->length = inode->size;
    fnode->mask = inode->mode & 0xFFF;
    fnode->nlink = inode->links_count;
    /* File Flags */
    fnode->flags = 0;
    if ((inode->mode & EXT2_S_IFREG) == EXT2_S_IFREG) {
        serialLog("[!!]Root appears to be a regular file.");
        serialLog("[!!]This is probably very, very wrong.");
        return 0;
    }
    if ((inode->mode & EXT2_S_IFDIR) == EXT2_S_IFDIR) {
    } else {
        serialLog("[!!]Root doesn't appear to be a directory.");
        serialLog("[!!]This is probably very, very wrong.");

        serialLog("[!] Other useful information:");
        serialLog("[!] %d", inode->uid);
        serialLog("[!] %d", inode->gid);
        serialLog("[!] %d", inode->size);
        serialLog("[!] %d", inode->mode);
        serialLog("[!] %d", inode->links_count);

        return 0;
    }
    if ((inode->mode & EXT2_S_IFBLK) == EXT2_S_IFBLK) {
        fnode->flags |= FS_BLOCKDEVICE;
    }
    if ((inode->mode & EXT2_S_IFCHR) == EXT2_S_IFCHR) {
        fnode->flags |= FS_CHARDEVICE;
    }
    if ((inode->mode & EXT2_S_IFIFO) == EXT2_S_IFIFO) {
        fnode->flags |= FS_PIPE;
    }
    if ((inode->mode & EXT2_S_IFLNK) == EXT2_S_IFLNK) {
        fnode->flags |= FS_SYMLINK;
    }

    fnode->atime   = inode->atime;
    fnode->mtime   = inode->mtime;
    fnode->ctime   = inode->ctime;

    fnode->flags |= FS_DIRECTORY;
    fnode->read    = NULL;
    fnode->write   = NULL;
    fnode->open    = open_ext2;
    fnode->close   = close_ext2;
    fnode->readdir = readdir_ext2;
    fnode->finddir = finddir_ext2;
    fnode->ioctl   = NULL;
    fnode->create  = create_ext2;
    fnode->mkdir   = mkdir_ext2;
    fnode->unlink  = unlink_ext2;
    return 1;
}

fs_node_t * mount_ext2(fs_node_t * block_device, int flags) {
    ext2_fs_t * this = kmalloc(sizeof(ext2_fs_t));

    memset(this, 0x00, sizeof(ext2_fs_t));

    this->flags = flags;

    this->block_device = block_device;
    this->block_size = 1024;

    SB = kmalloc(this->block_size);

    serialLog("[*]Reading superblock...");
    read_block(this, 1, (uint8_t *)SB);
    if (SB->magic != EXT2_SUPER_MAGIC) {
        serialLog("[!] ... not an EXT2 filesystem? (magic didn't match, got 0x%x)", SB->magic);
        return NULL;
    }
    this->inode_size = SB->inode_size;
    if (SB->inode_size == 0) {
        this->inode_size = 128;
    }
    this->block_size = 1024 << SB->log_block_size;
    this->cache_entries = 10240;
    if (this->block_size > 2048) {
        this->cache_entries /= 4;
    }
    serialLog("[*]bs=%d, cache entries=%d", this->block_size, this->cache_entries);
    this->pointers_per_block = this->block_size / 4;
    serialLog("[*]Log block size = %d -> %d", SB->log_block_size, this->block_size);
    BGDS = SB->blocks_count / SB->blocks_per_group;
    if (SB->blocks_per_group * BGDS < SB->blocks_count) {
        BGDS += 1;
    }
    this->inodes_per_group = SB->inodes_count / BGDS;

    if (!(this->flags & EXT2_FLAG_NOCACHE)) {
        DC = kmalloc(sizeof(ext2_disk_cache_entry_t) * this->cache_entries);
        this->cache_data = calloc(this->block_size, this->cache_entries);
        for (uint32_t i = 0; i < this->cache_entries; ++i) {
            DC[i].block_no = 0;
            DC[i].dirty = 0;
            DC[i].last_use = 0;
            DC[i].block = this->cache_data + i * this->block_size;
            if (i % 128 == 0) {
            }
        }
    } else {
        DC = NULL;
    }

    // load the block group descriptors
    this->bgd_block_span = sizeof(ext2_bgdescriptor_t) * BGDS / this->block_size + 1;
    BGD = kmalloc(this->block_size * this->bgd_block_span);

    serialLog("[*]bgd_block_span = %d", this->bgd_block_span);

    this->bgd_offset = 2;

    if (this->block_size > 1024) {
        this->bgd_offset = 1;
    }

    for (int i = 0; i < this->bgd_block_span; ++i) {
        read_block(this, this->bgd_offset + i, (uint8_t *)((uint32_t)BGD + this->block_size * i));
    }

#ifdef DEBUG_BLOCK_DESCRIPTORS
    char * bg_buffer = kmalloc()(this->block_size * sizeof(char));
	for (uint32_t i = 0; i < BGDS; ++i) {
		serialLog()("[*]Block Group Descriptor #%d @ %d", i, this->bgd_offset + i * SB->blocks_per_group);
		serialLog()("[*]\tBlock Bitmap @ %d", BGD[i].block_bitmap); {
			serialLog()("[*]\t\tExamining block bitmap at %d", BGD[i].block_bitmap);
			read_block(this, BGD[i].block_bitmap, (uint8_t *)bg_buffer);
			uint32_t j = 0;
			while (BLOCKBIT(j)) {
				++j;
			}
			serialLog()("[*]\t\tFirst free block in group is %d", j + BGD[i].block_bitmap - 2);
		}
		serialLog()("[*]\tInode Bitmap @ %d", BGD[i].inode_bitmap); {
			serialLog()("[*]\t\tExamining inode bitmap at %d", BGD[i].inode_bitmap);
			read_block(this, BGD[i].inode_bitmap, (uint8_t *)bg_buffer);
			uint32_t j = 0;
			while (BLOCKBIT(j)) {
				++j;
			}
			serialLog()("[*]\t\tFirst free inode in group is %d", j + this->inodes_per_group * i + 1);
		}
		serialLog()("[*]\tInode Table  @ %d", BGD[i].inode_table);
		serialLog()("[*]\tFree Blocks =  %d", BGD[i].free_blocks_count);
		serialLog()("[*]\tFree Inodes =  %d", BGD[i].free_inodes_count);
	}
	free(bg_buffer);
#endif

    ext2_inodetable_t *root_inode = read_inode(this, 2);
    RN = (fs_node_t *)kmalloc(sizeof(fs_node_t));
    if (!ext2_root(this, root_inode, RN)) {
        return NULL;
    }
    return RN;
}

fs_node_t * ext2_fs_mount(char * device, char * mount_path) {

    char * arg = strdup(device);
    char * argv[10];
    int argc = tokenize(arg, ",", argv);

    fs_node_t * dev = kopen(argv[0], 0);
    if (!dev) {
        serialLog("[!] failed to open %s", device);
        return NULL;
    }

    int flags = 0;

    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i],"nocache")) {
            flags |= EXT2_FLAG_NOCACHE;
        } else {
            serialLog("[x] Unrecognized option to ext2 driver: %s", argv[i]);
        }
    }

    fs_node_t * fs = mount_ext2(dev, flags);

    free(arg);
    return fs;
}

void ext2_initialize(void) {
    vfs_register("ext2", ext2_fs_mount);
}