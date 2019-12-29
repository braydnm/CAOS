//
// Created by x3vikan on 3/23/18.
//

#ifndef OS_SORT_H
#define OS_SORT_H

#include "../kernel/kernel.h"

static __inline char	*med3(char *, char *, char *, int (*)(const void *, const void *));
static __inline void	swapfunc(char *, char *, size_t, int);

#define min(a,b)    (a<b)?a:b

#define swapCodes(T, param1, param2, size){\
    size_t i = (size)/sizeof(T);\
    T* ptr1 = (T*) param1;\
    T* ptr2 = (T*) param2;\
    do {\
		TYPE	t = *ptr1;\
		*ptr1++ = *ptr2;\
		*ptr2++ = t;\
        } while (--size > 0);\
}



#endif //OS_SORT_H
