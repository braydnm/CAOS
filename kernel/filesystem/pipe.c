#include "../kernel.h"

#define DEBUG_PIPES 0

uint32_t read_pipe(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
uint32_t write_pipe(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
void open_pipe(fs_node_t *node, unsigned int flags);
void close_pipe(fs_node_t *node);

size_t pipe_unread(pipe_device_t* pipe){
    if (pipe->read_ptr == null)
        return 0;
    if (pipe->read_ptr>pipe->write_ptr)
        return pipe->size - pipe->read_ptr + pipe->write_ptr;
    else
        return pipe->write_ptr-pipe->read_ptr;
}

int pipe_size(fs_node_t* node){
    return pipe_unread((pipe_device_t*)node->device);
}

size_t pipe_available(pipe_device_t* pipe){
    if (pipe->read_ptr == pipe->write_ptr)
        return pipe->size - 1;

    if (pipe->read_ptr > pipe->write_ptr)
        return pipe->read_ptr - pipe->write_ptr - 1;
    else
        return (pipe->size - pipe->write_ptr) + pipe->read_ptr - 1;
}

int pip_unsize(fs_node_t* node){
    return pipe_available((pipe_device_t*)node->device);
}

void pipe_increment_read(pipe_device_t* pipe){
    if (++pipe->read_ptr == pipe->size)
        pipe->read_ptr = 0;
}

void pipe_increment_write(pipe_device_t* pipe){
    if (++pipe->write_ptr == pipe->size)
        pipe->write_ptr = 0;
}

void pipe_increment_read_amount(pipe_device_t* pipe, size_t size){
    pipe->read_ptr+=size;
    pipe->read_ptr%=pipe->size;
}

void pipe_increment_write_amount(pipe_device_t* pipe, size_t size){
    pipe->write_ptr+=size;
    pipe->write_ptr%=pipe->size;
}

uint32_t read_pipe(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer){
    assert(node->device!=0 && "Trying to read closed pipe");

    pipe_device_t* pipe = (pipe_device_t*)node->device;

#if DEBUG_PIPES
    if (pipe->size > 300) { /* Ignore small pipes (ie, keyboard) */
		debug_print(INFO, "[debug] Call to read from pipe 0x%x", node->device);
		debug_print(INFO, "        Unread bytes:    %d", pipe_unread(pipe));
		debug_print(INFO, "        Total size:      %d", pipe->size);
		debug_print(INFO, "        Request size:    %d", size);
		debug_print(INFO, "        Write pointer:   %d", pipe->write_ptr);
		debug_print(INFO, "        Read  pointer:   %d", pipe->read_ptr);
		debug_print(INFO, "        Buffer address:  0x%x", pipe->buffer);
	}
#endif

    if (pipe->dead){
        serialLog("[!] Pipe is dead");
        return 0;
    }

    size_t collected = 0;
    while (pipe_unread(pipe) > 0 && collected < size) {
        buffer[collected] = pipe->buffer[pipe->read_ptr];
        pipe_increment_read(pipe);
        collected++;
    }

    return collected;
}

uint32_t write_pipe(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer){
    assert(node->device!=0 && "Trying to write to dead pipe");
    pipe_device_t* pipe = (pipe_device_t*)node->device;

#if DEBUG_PIPES
    if (pipe->size > 300) { /* Ignore small pipes (ie, keyboard) */
		debug_print(INFO, "[debug] Call to write to pipe 0x%x", node->device);
		debug_print(INFO, "        Available space: %d", pipe_available(pipe));
		debug_print(INFO, "        Total size:      %d", pipe->size);
		debug_print(INFO, "        Request size:    %d", size);
		debug_print(INFO, "        Write pointer:   %d", pipe->write_ptr);
		debug_print(INFO, "        Read  pointer:   %d", pipe->read_ptr);
		debug_print(INFO, "        Buffer address:  0x%x", pipe->buffer);
		debug_print(INFO, " Write: %s", buffer);
	}
#endif

    if (pipe->dead){
        serialLog("[!] Pipe is dead");
        return 0;
    }

    size_t written = 0;
    while (pipe_available(pipe) > 0 && written < size) {
        pipe->buffer[pipe->write_ptr] = buffer[written];
        pipe_increment_write(pipe);
        written++;
    }
    return written;
}

void open_pipe(fs_node_t* node, unsigned int flags){
    assert(node->device != 0 && "Attempted to open a fully-closed pipe.");

    /* Retreive the pipe object associated with this file node */
    pipe_device_t * pipe = (pipe_device_t *)node->device;

    /* Add a reference */
    pipe->refcount++;
}

void close_pipe(fs_node_t* node){
    assert(node->device != 0 && "Attempted to close an already fully-closed pipe.");

    /* Retreive the pipe object associated with this file node */
    pipe_device_t * pipe = (pipe_device_t *)node->device;

    /* Drop one reference */
    pipe->refcount--;
}

int pipe_check(fs_node_t* node){
    return pipe_unread((pipe_device_t*)node->device)>0?0:1;
}

fs_node_t * make_pipe(size_t size) {
    fs_node_t * fnode = kmalloc(sizeof(fs_node_t));
    pipe_device_t * pipe = kmalloc(sizeof(pipe_device_t));
    memset(fnode, 0, sizeof(fs_node_t));
    memset(pipe, 0, sizeof(pipe_device_t));

    fnode->device = 0;
    fnode->name[0] = '\0';
    snprintf(fnode->name, "[pipe]");
    fnode->uid   = 0;
    fnode->gid   = 0;
    fnode->mask  = 0666;
    fnode->flags = FS_PIPE;
    fnode->read  = read_pipe;
    fnode->write = write_pipe;
    fnode->open  = open_pipe;
    fnode->close = close_pipe;
    fnode->readdir = NULL;
    fnode->finddir = NULL;
    fnode->ioctl   = NULL; /* TODO ioctls for pipes? maybe */
    fnode->get_size = pipe_size;

    fnode->selectcheck = pipe_check;

    fnode->atime = now();
    fnode->mtime = fnode->atime;
    fnode->ctime = fnode->atime;

    fnode->device = pipe;

    pipe->buffer    = kmalloc(size);
    pipe->write_ptr = 0;
    pipe->read_ptr  = 0;
    pipe->size      = size;
    pipe->refcount  = 0;
    pipe->dead      = 0;

    pipe->wait_queue_writers = list_create();
    pipe->wait_queue_readers = list_create();

    return fnode;
}