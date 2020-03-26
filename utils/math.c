#include "math.h"

int ceil(float num) {
    if (((int)num)<num)
        num++;
    return (int)num;
}

int floor(float num) {
    return (int)num;
}