//
// Created by x3vikan on 3/18/18.
//

#ifndef OS_VA_ARGS_H
#define OS_VA_ARGS_H

typedef unsigned char* args_list;

#define itemSize int

/* round up width of objects pushed on stack. The expression before the
& ensures that we get 0 for objects of size 0. */
#define args_size(type) ((sizeof(type)+sizeof(itemSize) -1) & ~(sizeof(itemSize)-1))

// first arg is the first argument(before the ...)
#define args_start(start, firstArg) (start=((args_list)&(firstArg)+args_size(firstArg)))

#define args_end(arg)

#define get_args(arg, type) (arg += args_size(type), *((type *)(arg - args_size(type))))

#endif //OS_VA_ARGS_H
