#include <kernel.h>
#include "ata.h"

#define SECTOR_SIZE 512

static mbr_t mbr;

struct dos_entry{
    fs_node_t* dev;
    partition_t partition;
};

static uint32_t read_part(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buf){
    struct dos_entry * dev = (struct dos_entry *)node->device;

    if (offset > dev->partition.sector_count * SECTOR_SIZE) {
        serialLog("[!] Read beyond partition!");
        return 0;
    }

    if (offset + size > dev->partition.sector_count * SECTOR_SIZE) {
        size = dev->partition.sector_count * SECTOR_SIZE - offset;
        serialLog("[!] Tried to read past end of partition, clamped to %d", size);
    }

    return read_fs(dev->dev, offset+dev->partition.lba_first_sector*SECTOR_SIZE, size, buf);
}

static uint32_t write_part(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buf){
    struct dos_entry* dev = (struct dos_entry*)node->device;
    if(offset>dev->partition.sector_count*SECTOR_SIZE)
        return 0;
    if (offset+size>dev->partition.sector_count*SECTOR_SIZE)
        size = dev->partition.sector_count*SECTOR_SIZE-offset;

    return write_fs(dev->dev, offset+dev->partition.lba_first_sector*SECTOR_SIZE, size, buf);
}

static void open_part(fs_node_t* node, unsigned int flag){
    return;
}

static void close_part(fs_node_t* node){
    return;
}

static fs_node_t* dospart_device_create(int i, fs_node_t* dev, partition_t* part){
    dev->refcount = -1;
    struct dos_entry* entry = (struct dos_entry*)kmalloc(sizeof(struct dos_entry));
    memcpy(&entry->partition, part, sizeof(partition_t));
    entry->dev = dev;

    fs_node_t* node = kmalloc(sizeof(fs_node_t));
    memset(node, 0, sizeof(fs_node_t));
    node->inode = 0;
    snprintf(node->name, "dospart%d", i);
    node->device = entry;
    node->uid = 0;
    node->gid = 0;
    node->mask = 0660;
    node->length  = entry->partition.sector_count * SECTOR_SIZE;
    node->flags = FS_BLOCKDEVICE;
    node->read = read_part;
    node->write = write_part;
    node->open = open_part;
    node->close = close_part;
    node->readdir = NULL;
    node->finddir = NULL;
    node->ioctl = NULL;
    return node;
}

static int read_partition_map(char* name){
    fs_node_t* device = kopen(name, 0);
    if (!device) return 1;

    read_fs(device, 0, SECTOR_SIZE, (uint8_t*)&mbr);
    if (mbr.signature[0] != 0x55 || mbr.signature[1] != 0xAA){
        serialLog("[!] Partition signature not found");
        return 0;
    }

    serialLog("[*] Found partition");
    for (int i = 0; i<4; i++){
        if (!(mbr.partitions[i].status & 0x80)) {
            serialLog("[*] Partition #%d: inactive", i+1);
            continue;
        }
        serialLog("[*] Partition #%d: @%d+%d", i+1, mbr.partitions[i].lba_first_sector, mbr.partitions[i].sector_count);
        fs_node_t * node = dospart_device_create(i, device, &mbr.partitions[i]);

        char tmp[64];
        snprintf(tmp, "%s%d", name, i);
        vfs_mount(tmp, node);
    }
    return 0;
}

static int dospart_init(void){
    for (char end = 'a'; end<'z'; end++){
        char name[64];
        snprintf(name, "/dev/hd%c", end);
        if (read_partition_map(name))
            break;
    }
    return 0;
}

static int dospart_finialize(void){
    return 0;
}

KERNEL_MODULE_DEF(partition, dospart_init, dospart_finialize);