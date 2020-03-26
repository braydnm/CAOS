#ifndef TYPES_H
#define TYPES_H

#include "stdint.h"

#ifndef null
#define null 0
#endif

#ifndef NULL
    #define NULL 0
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#ifndef bool
#ifndef __cplusplus
typedef _Bool bool;
#endif
#endif

#ifndef UCHAR_MAX
#define UCHAR_MAX 255
#endif

#define vectorType(T) struct{T* data; size_t length, capacity;}

typedef vectorType(char*) string_vector_t;

struct map_node_t;
typedef struct map_node_t map_node_t;

typedef struct {
    map_node_t **buckets;
    unsigned nbuckets, nnodes;
} map_base_t;

typedef struct {
    unsigned bucketidx;
    map_node_t *node;
} map_iter_t;


#define map_t(T)\
  struct { map_base_t base; T *ref; T tmp; }

typedef struct page {
    unsigned int present  :1;
    unsigned int rw       :1;
    unsigned int user     :1;
    unsigned int accessed :1;
    unsigned int dirty    :1;
    unsigned int unused   :7;
    unsigned int frame    :20;
} page_t;

typedef struct page_table {
    page_t pages[1024];
} page_table_t;

typedef struct page_directory {
    uintptr_t phy_tables[1024];
    page_table_t *tables[1024];
    uintptr_t phy_addr;
} page_dir_t;

typedef unsigned long useconds_t;
typedef long suseconds_t;
typedef long time_t;

#endif