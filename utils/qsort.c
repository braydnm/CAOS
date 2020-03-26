//
// Created by x3vikan on 3/26/18.
//
#include "qsort.h"

void qsort(const void* basePtr, size_t totalElems, size_t size, sortFunction func, void* arg){
    char* ptr = basePtr;
    const size_t maxThresh = MAX_THRESHOLD*size;

    if (size==0)
        return;

    if (totalElems>maxThresh){
        char* lo = basePtr;
        char* hi = &lo[size*(totalElems-1)];
        stackNode stack[STACK_SIZE];
        stackNode* top = stack;

        PUSH(null, null);

        while (STACK_NOT_EMPTY){
            char* leftPtr;
            char* rightPtr;

            char *mid = lo + size * ((hi - lo) / size >> 1);

            if ((*func) ((void *) mid, (void *) lo, arg) < 0)
                SWAP (mid, lo, size);
            if ((*func) ((void *) hi, (void *) mid, arg) < 0)
                SWAP (mid, hi, size);
            else
                goto jump_over;
            if ((*func) ((void *) mid, (void *) lo, arg) < 0)
                SWAP (mid, lo, size);
            jump_over:;

            leftPtr = lo+size;
            rightPtr = hi-size;

            do
            {
                while ((*func) ((void *) leftPtr, (void *) mid, arg) < 0)
                    leftPtr += size;

                while ((*func) ((void *) mid, (void *) rightPtr, arg) < 0)
                    rightPtr -= size;

                if (leftPtr < rightPtr)
                {
                    SWAP (leftPtr, rightPtr, size);
                    if (mid == leftPtr)
                        mid = rightPtr;
                    else if (mid == rightPtr)
                        mid = leftPtr;
                    leftPtr += size;
                    rightPtr -= size;
                }
                else if (leftPtr == rightPtr)
                {
                    leftPtr += size;
                    rightPtr -= size;
                    break;
                }
            }
            while (leftPtr <= rightPtr);

            if ((size_t) (rightPtr - lo) <= MAX_THRESHOLD)
            {
                if ((size_t) (hi - leftPtr) <= MAX_THRESHOLD)
                    /* Ignore both small partitions. */
                    POP (lo, hi);
                else
                    /* Ignore small left partition. */
                    lo = leftPtr;
            }
            else if ((size_t) (hi - leftPtr) <= MAX_THRESHOLD)
                /* Ignore small right partition. */
                hi = rightPtr;
            else if ((rightPtr - lo) > (hi - leftPtr))
            {
                /* Push larger left partition indices. */
                PUSH (lo, rightPtr);
                lo = leftPtr;
            }
            else
            {
                /* Push larger right partition indices. */
                PUSH (leftPtr, hi);
                hi = rightPtr;
            }
        }
    }

    {
        char *const end_ptr = &basePtr[size * (totalElems - 1)];
        char *tmp_ptr = basePtr;
        char *thresh = min(end_ptr, basePtr + maxThresh);
        register char *run_ptr;

        /* Find smallest element in first threshold and place it at the
           array's beginning.  This is the smallest array element,
           and the operation speeds up insertion sort's inner loop. */

        for (run_ptr = tmp_ptr + size; run_ptr <= thresh; run_ptr += size)
            if ((*func) ((void *) run_ptr, (void *) tmp_ptr, arg) < 0)
                tmp_ptr = run_ptr;

        if (tmp_ptr != basePtr)
            SWAP (tmp_ptr, basePtr, size);

        /* Insertion sort, running from left-hand-side up to right-hand-side.  */

        run_ptr = basePtr + size;
        while ((run_ptr += size) <= end_ptr)
        {
            tmp_ptr = run_ptr - size;
            while ((*func) ((void *) run_ptr, (void *) tmp_ptr, arg) < 0)
                tmp_ptr -= size;

            tmp_ptr += size;
            if (tmp_ptr != run_ptr)
            {
                char *trav;

                trav = run_ptr + size;
                while (--trav >= run_ptr)
                {
                    char c = *trav;
                    char *hi, *lo;

                    for (hi = lo = trav; (lo -= size) >= tmp_ptr; hi = lo)
                        *hi = *lo;
                    *hi = c;
                }
            }
        }
    }

}


