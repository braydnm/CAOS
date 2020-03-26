//
// Created by x3vikan on 3/26/18.
//

#ifndef OS_QSORT_H
#define OS_QSORT_H


#include "../kernel/kernel.h"

typedef int(*sortFunction)(const void*, const void*, void*);
#define MAX_THRESHOLD 4
#define CHAR_BIT 8


#define min(x, y) ((x) < (y) ? (x) : (y))

// byte swap
#define SWAP(a, b, size)						      \
  do									      \
    {									      \
      register size_t __size = (size);					      \
      register char *__a = (a), *__b = (b);				      \
      do								      \
	{								      \
	  char __tmp = *__a;						      \
	  *__a++ = *__b;						      \
	  *__b++ = __tmp;						      \
	} while (--__size > 0);						      \
} while (0)

typedef struct{
    char* lo;
    char* hi;
} stackNode;

#define STACK_SIZE	(CHAR_BIT * sizeof(size_t))
#define PUSH(low, high)	((void) ((top->lo = (low)), (top->hi = (high)), ++top))
#define	POP(low, high)	((void) (--top, (low = top->lo), (high = top->hi)))
#define	STACK_NOT_EMPTY	(stack < top)



void qsort(const void* basePtr, size_t totalElems, size_t size, sortFunction func, void* arg);

#endif //OS_QSORT_H
