#include "../kernel.h"


#define MAX_SYMLINK_DEPTH 8
#define MAX_SYMLINK_SIZE 4096

tree_t    * fs_tree = NULL; /* File system mountpoint tree */
fs_node_t * fs_root = NULL; /* Pointer to the root mount fs_node (must be some form of filesystem, even ramdisk) */

hashmap_t * fs_types = NULL;

static struct dirent * readdir_mapper(fs_node_t *node, uint32_t index) {
    tree_node_t * d = (tree_node_t *)node->device;

    if (!d) return NULL;

    if (index == 0) {
        struct dirent * dir = kmalloc(sizeof(struct dirent));
        strcpy(dir->name, ".");
        dir->ino = 0;
        return dir;
    } else if (index == 1) {
        struct dirent * dir = kmalloc(sizeof(struct dirent));
        strcpy(dir->name, "..");
        dir->ino = 1;
        return dir;
    }

    index -= 2;
    unsigned int i = 0;
    foreach(child, d->children) {
        if (i == index) {
            /* Recursively print the children */
            tree_node_t * tchild = (tree_node_t *)child->value;
            struct vfs_entry * n = (struct vfs_entry *)tchild->value;
            struct dirent * dir = kmalloc(sizeof(struct dirent));

            size_t len = strlen(n->name) + 1;
            memcpy(&dir->name, n->name, MIN(256, len));
            dir->ino = i;
            return dir;
        }
        ++i;
    }

    return NULL;
}

static fs_node_t * vfs_mapper(void) {
    fs_node_t * fnode = kmalloc(sizeof(fs_node_t));
    memset(fnode, 0x00, sizeof(fs_node_t));
    fnode->mask = 0555;
    fnode->flags   = FS_DIRECTORY;
    fnode->readdir = readdir_mapper;
    fnode->ctime   = now();
    fnode->mtime   = now();
    fnode->atime   = now();
    return fnode;
}

int selectcheck_fs(fs_node_t * node) {
    if (!node) return -ENOENT;

    if (node->selectcheck) {
        return node->selectcheck(node);
    }

    return -EINVAL;
}

int selectwait_fs(fs_node_t * node, void * process) {
    if (!node) return -ENOENT;

    if (node->selectwait) {
        return node->selectwait(node, process);
    }

    return -EINVAL;
}

uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    if (!node) return -ENOENT;

    if (node->read) {
        uint32_t ret = node->read(node, offset, size, buffer);
        return ret;
    } else {
        return -EINVAL;
    }
}

uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    if (!node) return -ENOENT;

    if (node->write) {
        uint32_t ret = node->write(node, offset, size, buffer);
        return ret;
    } else {
        return -EINVAL;
    }
}

void open_fs(fs_node_t *node, unsigned int flags) {

    if (!node) return;

    if (node->refcount >= 0) {
        node->refcount++;
    }

    if (node->open)
        node->open(node, flags);
}

void close_fs(fs_node_t *node) {
    assert(node != fs_root && "Attempted to close the filesystem root. kablooey");

    if (!node) {
        serialLog("[x] Double close? This isn't an fs_node.");
        return;
    }

    if (node->refcount == -1) return;

    node->refcount--;
    if (node->refcount == 0) {
        serialLog("[*] Node refcount [%s] is now 0: %d", node->name, node->refcount);

        if (node->close)
            node->close(node);

        free(node);
    }
}

int chmod_fs(fs_node_t *node, int mode) {
    if (node->chmod) {
        return node->chmod(node, mode);
    }
    return 0;
}

int chown_fs(fs_node_t *node, int uid, int gid) {
    if (node->chown) {
        return node->chown(node, uid, gid);
    }
    return 0;
}

struct dirent *readdir_fs(fs_node_t *node, uint32_t index) {
    if (!node) return NULL;

    if ((node->flags & FS_DIRECTORY) && node->readdir) {
        struct dirent *ret = node->readdir(node, index);
        return ret;
    } else {
        return (struct dirent *)NULL;
    }
}

fs_node_t *finddir_fs(fs_node_t *node, char *name) {
    if (!node) return NULL;

    if ((node->flags & FS_DIRECTORY) && node->finddir) {
        fs_node_t *ret = node->finddir(node, name);
        return ret;
    } else {
        serialLog("[x] Node passed to finddir_fs isn't a directory!");
        serialLog("[x] node = 0x%x, name = %s", node, name);
        return (fs_node_t *)NULL;
    }
}

int ioctl_fs(fs_node_t *node, int request, void * argp) {
    if (!node) return -ENOENT;

    if (node->ioctl) {
        return node->ioctl(node, request, argp);
    } else {
        return -EINVAL;
    }
}

int create_file_fs(char *name, uint16_t permission) {
    fs_node_t * parent;
    char *path = canonicalize_path(cwd, name);

    char * parent_path = kmalloc(strlen(path) + 4);
    snprintf(parent_path, "%s/..", path);

    char * f_path = path + strlen(path) - 1;
    while (f_path > path) {
        if (*f_path == '/') {
            f_path += 1;
            break;
        }
        f_path--;
    }

    while (*f_path == '/') {
        f_path++;
    }

    serialLog("[*] creating file %s within %s (hope these strings are good)", f_path, parent_path);

    parent = kopen(parent_path, 0);
    free(parent_path);

    if (!parent) {
        serialLog("[x] failed to open parent");
        free(path);
        return -ENOENT;
    }

//    if (!has_permission(parent, 02)) {
//        serialLog("[x] bad permissions");
//        return -EACCES;
//    }

    int ret = 0;
    if (parent->create) {
        ret = parent->create(parent, f_path, permission);
    } else {
        ret = -EINVAL;
    }

    free(path);
    free(parent);

    return ret;
}

int unlink_fs(char * name) {
    fs_node_t * parent;
    char *path = canonicalize_path(cwd, name);

    char * parent_path = kmalloc(strlen(path) + 4);
    snprintf(parent_path, "%s/..", path);

    char * f_path = path + strlen(path) - 1;
    while (f_path > path) {
        if (*f_path == '/') {
            f_path += 1;
            break;
        }
        f_path--;
    }

    while (*f_path == '/') {
        f_path++;
    }

    serialLog("[*] unlinking file %s within %s (hope these strings are good)", f_path, parent_path);

    parent = kopen(parent_path, 0);
    free(parent_path);

    if (!parent) {
        free(path);
        return -ENOENT;
    }

    int ret = 0;
    if (parent->unlink) {
        ret = parent->unlink(parent, f_path);
    } else {
        ret = -EINVAL;
    }

    free(path);
    free(parent);
    return ret;
}


int mkdir_fs(char *name, uint16_t permission) {
    fs_node_t * parent;
    char *path = canonicalize_path(cwd, name);

    char * parent_path = kmalloc(strlen(path) + 4);
    snprintf(parent_path, "%s/..", path);

    fs_node_t * this = kopen(path, 0);
    int _exists = 0;
    if (this) {
        serialLog("[x] Tried to mkdir a dir that already exists? (%s)", path);
        _exists = 1;
    }

    char * f_path = path + strlen(path) - 1;
    while (f_path > path) {
        if (*f_path == '/') {
            f_path += 1;
            break;
        }
        f_path--;
    }

    while (*f_path == '/') {
        f_path++;
    }

    serialLog("[x] creating directory %s within %s (hope these strings are good)", f_path, parent_path);

    parent = kopen(parent_path, 0);
    free(parent_path);

    if (!parent) {
        free(path);
        if (_exists) {
            return -EEXIST;
        }
        return -ENOENT;
    }

    int ret = 0;
    if (parent->mkdir) {
        ret = parent->mkdir(parent, f_path, permission);
    } else {
        ret = -EINVAL;
    }

    free(path);
    close_fs(parent);

    if (_exists) {
        return -EEXIST;
    }
    return ret;
}

fs_node_t *clone_fs(fs_node_t *source) {
    if (!source) return NULL;

    if (source->refcount >= 0) {
        source->refcount++;
    }

    return source;
}

int symlink_fs(char * target, char * name) {
    fs_node_t * parent;
    char *path = canonicalize_path(cwd, name);

    char * parent_path = kmalloc(strlen(path) + 4);
    snprintf(parent_path, "%s/..", path);

    char * f_path = path + strlen(path) - 1;
    while (f_path > path) {
        if (*f_path == '/') {
            f_path += 1;
            break;
        }
        f_path--;
    }

    serialLog("[x] creating symlink %s within %s", f_path, parent_path);

    parent = kopen(parent_path, 0);
    free(parent_path);

    if (!parent) {
        free(path);
        return -ENOENT;
    }

    int ret = 0;
    if (parent->symlink) {
        ret = parent->symlink(parent, target, f_path);
    } else {
        ret = -EINVAL;
    }

    free(path);
    close_fs(parent);

    return ret;
}

int readlink_fs(fs_node_t *node, char * buf, uint32_t size) {
    if (!node) return -ENOENT;

    if (node->readlink) {
        return node->readlink(node, buf, size);
    } else {
        return -EINVAL;
    }
}

char *canonicalize_path(char *cwd, char *input) {
    /* This is a stack-based canonicalizer; we use a list as a stack */
    list_t *out = list_create();

    /*
     * If we have a relative path, we need to canonicalize
     * the working directory and insert it into the stack.
     */
    if (strlen(input) && input[0] != PATH_SEPARATOR) {
        /* Make a copy of the working directory */
        char *path = kmalloc((strlen(cwd) + 1) * sizeof(char));
        memcpy(path, cwd, strlen(cwd) + 1);

        /* Setup tokenizer */
        char *pch;
        char *save;
        pch = strtok_r(path,PATH_SEPARATOR_STRING,&save);

        /* Start tokenizing */
        while (pch != NULL) {
            /* Make copies of the path elements */
            char *s = kmalloc(sizeof(char) * (strlen(pch) + 1));
            memcpy(s, pch, strlen(pch) + 1);
            /* And push them */
            list_insert(out, s);
            pch = strtok_r(NULL,PATH_SEPARATOR_STRING,&save);
        }
        free(path);
    }

    /* Similarly, we need to push the elements from the new path */
    char *path = kmalloc((strlen(input) + 1) * sizeof(char));
    memcpy(path, input, strlen(input) + 1);

    /* Initialize the tokenizer... */
    char *pch;
    char *save;
    pch = strtok_r(path,PATH_SEPARATOR_STRING,&save);

    /*
     * Tokenize the path, this time, taking care to properly
     * handle .. and . to represent up (stack pop) and current
     * (do nothing)
     */
    while (pch != NULL) {
        if (!strcmp(pch,PATH_UP)) {
            /*
             * Path = ..
             * Pop the stack to move up a directory
             */
            node_t * n = list_pop(out);
            if (n) {
                free(n->value);
                free(n);
            }
        } else if (!strcmp(pch,PATH_DOT)) {
            /*
             * Path = .
             * Do nothing
             */
        } else {
            /*
             * Regular path, push it
             * XXX: Path elements should be checked for existence!
             */
            char * s = kmalloc(sizeof(char) * (strlen(pch) + 1));
            memcpy(s, pch, strlen(pch) + 1);
            list_insert(out, s);
        }
        pch = strtok_r(NULL, PATH_SEPARATOR_STRING, &save);
    }
    free(path);

    /* Calculate the size of the path string */
    size_t size = 0;
    foreach(item, out) {
        /* Helpful use of our foreach macro. */
        size += strlen(item->value) + 1;
    }

    /* join() the list */
    char *output = kmalloc(sizeof(char) * (size + 1));
    char *output_offset = output;
    if (size == 0) {
        /*
         * If the path is empty, we take this to mean the root
         * thus we synthesize a path of "/" to return.
         */
        output = realloc(output, sizeof(char) * 2);
        output[0] = PATH_SEPARATOR;
        output[1] = '\0';
    } else {
        /* Otherwise, append each element together */
        foreach(item, out) {
            output_offset[0] = PATH_SEPARATOR;
            output_offset++;
            memcpy(output_offset, item->value, strlen(item->value) + 1);
            output_offset += strlen(item->value);
        }
    }

    /* Clean up the various things we used to get here */
    list_destroy(out);
    list_free(out);
    free(out);

    /* And return a working, absolute path */
    return output;
}

void vfs_install(void) {
    /* Initialize the mountpoint tree */
    fs_tree = tree_create();

    struct vfs_entry * root = kmalloc(sizeof(struct vfs_entry));

    root->name = strdup("[root]");
    root->file = NULL; /* Nothing mounted as root */
    root->fs_type = NULL;
    root->device = NULL;

    tree_set_root(fs_tree, root);

    fs_types = hashmap_create(5);
}

int vfs_register(char * name, vfs_mount_callback callback) {
    if (hashmap_get(fs_types, name)) return 1;
    hashmap_set(fs_types, name, (void *)(uintptr_t)callback);
    return 0;
}

int vfs_mount_type(char * type, char * arg, char * mountpoint) {

    vfs_mount_callback t = (vfs_mount_callback)(uintptr_t)hashmap_get(fs_types, type);
    if (!t) {
        serialLog("[x] Unknown filesystem type: %s", type);
        return -ENODEV;
    }

    fs_node_t * n = t(arg, mountpoint);

    if (!n) return -EINVAL;

    tree_node_t * node = vfs_mount(mountpoint, n);
    if (node && node->value) {
        struct vfs_entry * ent = (struct vfs_entry *)node->value;
        ent->fs_type = strdup(type);
        ent->device  = strdup(arg);
    }

    serialLog("[*] Mounted %s[%s] to %s: 0x%x", type, arg, mountpoint, n);
//    debug_print_vfs_tree();

    return 0;
}

void * vfs_mount(char * path, fs_node_t * local_root) {
    if (!fs_tree) {
        serialLog("[x] VFS hasn't been initialized, you can't mount things yet!");
        return NULL;
    }
    if (!path || path[0] != '/') {
        serialLog("[x] Path must be absolute for mountpoint.");
        return NULL;
    }


    local_root->refcount = -1;

    tree_node_t * ret_val = NULL;

    char * p = strdup(path);
    char * i = p;

    int path_len   = strlen(p);

    /* Chop the path up */
    while (i < p + path_len) {
        if (*i == PATH_SEPARATOR) {
            *i = '\0';
        }
        i++;
    }
    /* Clean up */
    p[path_len] = '\0';
    i = p + 1;

    /* Root */
    tree_node_t * root_node = fs_tree->root;

    if (*i == '\0') {
        /* Special case, we're trying to set the root node */
        struct vfs_entry * root = (struct vfs_entry *)root_node->value;
        if (root->file) {
            serialLog("[x] Path %s already mounted, unmount before trying to mount something else.", path);
        }
        root->file = local_root;
        /* We also keep a legacy shortcut around for that */
        fs_root = local_root;
        ret_val = root_node;
    } else {
        tree_node_t * node = root_node;
        char * at = i;
        while (1) {
            if (at >= p + path_len) {
                break;
            }
            int found = 0;
            serialLog("[*] Searching for %s", at);
            foreach(child, node->children) {
                tree_node_t * tchild = (tree_node_t *)child->value;
                struct vfs_entry * ent = (struct vfs_entry *)tchild->value;
                if (!strcmp(ent->name, at)) {
                    found = 1;
                    node = tchild;
                    ret_val = node;
                    break;
                }
            }
            if (!found) {
                serialLog("[x] Did not find %s, making it.", at);
                struct vfs_entry * ent = kmalloc(sizeof(struct vfs_entry));
                ent->name = strdup(at);
                ent->file = NULL;
                ent->device = NULL;
                ent->fs_type = NULL;
                node = tree_node_insert_child(fs_tree, node, ent);
            }
            at = at + strlen(at) + 1;
        }
        struct vfs_entry * ent = (struct vfs_entry *)node->value;
        if (ent->file) {
            serialLog("[x] Path %s already mounted, unmount before trying to mount something else.", path);
        }
        ent->file = local_root;
        ret_val = node;
    }

    free(p);
    return ret_val;
}

void map_vfs_directory(char * c) {
    fs_node_t * f = vfs_mapper();
    struct vfs_entry * e = vfs_mount(c, f);
    if (!strcmp(c, "/")) {
        f->device = fs_tree->root;
    } else {
        f->device = e;
    }
}

fs_node_t *get_mount_point(char * path, unsigned int path_depth, char **outpath, unsigned int * outdepth) {
    size_t depth;

    for (depth = 0; depth <= path_depth; ++depth) {
        path += strlen(path) + 1;
    }

    /* Last available node */
    fs_node_t   * last = fs_root;
    tree_node_t * node = fs_tree->root;

    char * at = *outpath;
    int _depth = 1;
    int _tree_depth = 0;

    while (1) {
        if (at >= path) {
            break;
        }
        int found = 0;
        foreach(child, node->children) {
            tree_node_t * tchild = (tree_node_t *)child->value;
            struct vfs_entry * ent = (struct vfs_entry *)tchild->value;
            if (!strcmp(ent->name, at)) {
                found = 1;
                node = tchild;
                at = at + strlen(at) + 1;
                if (ent->file) {
                    _tree_depth = _depth;
                    last = ent->file;
                    *outpath = at;
                }
                break;
            }
        }
        if (!found) {
            break;
        }
        _depth++;
    }

    *outdepth = _tree_depth;

    if (last) {
        fs_node_t * last_clone = kmalloc(sizeof(fs_node_t));
        memcpy(last_clone, last, sizeof(fs_node_t));
        return last_clone;
    }
    return last;
}




fs_node_t *kopen_recur(char *filename, uint32_t flags, uint32_t symlink_depth, char *relative_to) {
    /* Simple sanity checks that we actually have a file system */
    if (!filename) {
        return NULL;
    }

    /* Canonicalize the (potentially relative) path... */
    char *path = canonicalize_path(relative_to, filename);
    serialLog("[*] Path canonicalized is %s", path);
    /* And store the length once to save recalculations */
    size_t path_len = strlen(path);

    /* If strlen(path) == 1, then path = "/"; return root */
    if (path_len == 1) {
        /* Clone the root file system node */
        fs_node_t *root_clone = kmalloc(sizeof(fs_node_t));
        memcpy(root_clone, fs_root, sizeof(fs_node_t));

        /* Free the path */
        free(path);

        open_fs(root_clone, flags);

        /* And return the clone */
        return root_clone;
    }

    /* Otherwise, we need to break the path up and start searching */
    char *path_offset = path;
    uint32_t path_depth = 0;
    while (path_offset < path + path_len) {
        /* Find each PATH_SEPARATOR */
        if (*path_offset == PATH_SEPARATOR) {
            *path_offset = '\0';
            path_depth++;
        }
        path_offset++;
    }
    /* Clean up */
    path[path_len] = '\0';
    path_offset = path + 1;

    /*
     * At this point, the path is tokenized and path_offset points
     * to the first token (directory) and path_depth is the number
     * of directories in the path
     */

    /*
     * Dig through the (real) tree to find the file
     */
    unsigned int depth = 0;
    /* Find the mountpoint for this file */
    serialLog("[*] Path: %s, Depth: %d", path, depth);
    fs_node_t *node_ptr = get_mount_point(path, path_depth, &path_offset, &depth);
    serialLog("%s", !node_ptr?"[!] Error no mount point":"[*] Got mount point");
    if (!node_ptr) return NULL;

    do {
        /*
         * This test is a little complicated, but we basically always resolve symlinks in the
         * of a path (like /home/symlink/file) even if O_NOFOLLOW and O_PATH are set. If we are
         * on the leaf of the path then we will look at those flags and act accordingly
         */
        if ((node_ptr->flags & FS_SYMLINK) &&
            !((flags & O_NOFOLLOW) && (flags & O_PATH) && depth == path_depth)) {
            /* This ensures we don't return a path when NOFOLLOW is requested but PATH
             * isn't passed.
             */
            if ((flags & O_NOFOLLOW) && depth == path_depth - 1) {
                /* TODO(gerow): should probably be setting errno from this */
                free((void *)path);
                free(node_ptr);
                return NULL;
            }
            if (symlink_depth >= MAX_SYMLINK_DEPTH) {
                /* TODO(gerow): should probably be setting errno from this */
                free((void *)path);
                free(node_ptr);
                return NULL;
            }
            /*
             * This may actually be big enough that we wouldn't want to allocate it on
             * the stack, especially considering this function is called recursively
             */
            char symlink_buf[MAX_SYMLINK_SIZE];
            int len = readlink_fs(node_ptr, symlink_buf, sizeof(symlink_buf));
            if (len < 0) {
                /* TODO(gerow): should probably be setting errno from this */
                free((void *)path);
                free(node_ptr);
                return NULL;
            }
            if (symlink_buf[len] != '\0') {
                /* TODO(gerow): should probably be setting errno from this */
                free((void *)path);
                free(node_ptr);
                return NULL;
            }
            fs_node_t * old_node_ptr = node_ptr;
            /* Rebuild our path up to this point. This is hella hacky. */
            char * relpath = kmalloc(path_len + 1);
            char * ptr = relpath;
            memcpy(relpath, path, path_len + 1);
            for (unsigned int i = 0; depth && i < depth-1; i++) {
                while(*ptr != '\0') {
                    ptr++;
                }
                *ptr = PATH_SEPARATOR;
            }
            node_ptr = kopen_recur(symlink_buf, 0, symlink_depth + 1, relpath);
            free(relpath);
            free(old_node_ptr);
            if (!node_ptr) {
                /* Dangling symlink? */
                free((void *)path);
                return NULL;
            }
        }
        if (path_offset >= path+path_len) {
            free(path);
            open_fs(node_ptr, flags);
            return node_ptr;
        }
        if (depth == path_depth) {
            /* We found the file and are done, open the node */
            open_fs(node_ptr, flags);
            free((void *)path);
            return node_ptr;
        }
        /* We are still searching... */
        serialLog("[*]... Searching for %s", path_offset);
        fs_node_t * node_next = finddir_fs(node_ptr, path_offset);
        free(node_ptr); /* Always a clone or an unopened thing */
        node_ptr = node_next;
        /* Search the active directory for the requested directory */
        if (!node_ptr) {
            /* We failed to find the requested directory */
            free((void *)path);
            return NULL;
        }
        path_offset += strlen(path_offset) + 1;
        ++depth;
    } while (depth < path_depth + 1);
    serialLog("[*]- Not found.");
    /* We failed to find the requested file, but our loop terminated. */
    free((void *)path);
    return NULL;
}

fs_node_t *kopen(char *filename, uint32_t flags) {
    return kopen_recur(filename, flags, 0, cwd);
}

