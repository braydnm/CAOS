#ifndef OS_MEM_H
#define OS_MEM_H

#include "../kernel.h"

extern int init_paging(uint32_t mem);
extern void paging_fini();
extern page_dir_t *copy_page_dir(page_dir_t *);

uint32_t mem_used();

extern void mark_region(uint64_t addt);

extern uintptr_t virtual_to_physical(uintptr_t p);

extern void make_dma_frame(page_t *, int, int, uintptr_t);
extern void free_frame(page_t *);

extern void heap_install(void);

/* frames */
void switch_page_dir(page_dir_t *p);
extern void set_nframes(uint32_t n);
extern uint32_t get_nframes();
extern uint32_t first_frame();
extern void set_frame(uintptr_t frame);
extern void clear_frame(uintptr_t frame);
extern int test_frame(uintptr_t frame);

extern uintptr_t mm_alloc_pages(int n);
extern uintptr_t mm_free_pages(void *, int);

/* alloc */
extern void *kmalloc(size_t s);
extern void *kvmalloc(size_t s);
extern void *kmalloc_p(size_t s, uintptr_t *p);
extern void *kvmalloc_p(size_t, uintptr_t *p);
extern void free(void *);
extern void* realloc(void*, size_t);
extern void* calloc(size_t s, size_t num);

void prealloc_start(uintptr_t addr);

#endif //OS_MEM_H
