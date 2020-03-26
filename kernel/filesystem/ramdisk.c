#include <kernel.h>

static uint32_t read_ramdisk(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
static uint32_t write_ramdisk(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
static void     open_ramdisk(fs_node_t *node, unsigned int flags);
static void     close_ramdisk(fs_node_t *node);

static uint32_t read_ramdisk(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {

    if (offset > node->length) {
        return 0;
    }

    if (offset + size > node->length) {
        unsigned int i = node->length - offset;
        size = i;
    }

    memcpy(buffer, (void *)(node->inode + offset), size);

    return size;
}

static uint32_t write_ramdisk(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    if (offset > node->length) {
        return 0;
    }

    if (offset + size > node->length) {
        unsigned int i = node->length - offset;
        size = i;
    }

    memcpy((void *)(node->inode + offset), buffer, size);
    return size;
}

static void open_ramdisk(fs_node_t * node, unsigned int flags) {
    return;
}

static void close_ramdisk(fs_node_t * node) {
    return;
}

static int ioctl_ramdisk(fs_node_t * node, int request, void * argp) {
    switch (request) {
        case 0x4001:
                /* Clear all of the memory used by this ramdisk */
                for (uintptr_t i = node->inode; i < node->inode + node->length; i += 0x1000) {
                    clear_frame(i);
                }
                /* Mark the file length as 0 */
                node->length = 0;
                return 0;
        default:
            return -EINVAL;
    }
}

static fs_node_t * ramdisk_device_create(int device_number, uintptr_t location, size_t size) {
    fs_node_t * fnode = kmalloc(sizeof(fs_node_t));
    memset(fnode, 0x00, sizeof(fs_node_t));
    fnode->inode = location;
    snprintf(fnode->name, "ram%d", device_number);
    fnode->uid = 0;
    fnode->gid = 0;
    fnode->mask    = 0770;
    fnode->length  = size;
    fnode->flags   = FS_BLOCKDEVICE;
    fnode->read    = read_ramdisk;
    fnode->write   = write_ramdisk;
    fnode->open    = open_ramdisk;
    fnode->close   = close_ramdisk;
    fnode->ioctl   = ioctl_ramdisk;
    return fnode;
}

static int last_device_number = 0;
fs_node_t * ramdisk_mount(uintptr_t location, size_t size) {
    fs_node_t * ramdisk = ramdisk_device_create(last_device_number, location, size);
    if (ramdisk) {
        char tmp[64];
        snprintf(tmp, "/dev/%s", ramdisk->name);
        serialLog("[*] Mounting ramdisk at %s", tmp);
        vfs_mount(tmp, ramdisk);
        last_device_number += 1;
        return ramdisk;
    }

    return NULL;
}
