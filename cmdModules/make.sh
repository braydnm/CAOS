#!/bin/bash

FILE=$1

NAME=${FILE%%.*}

gcc -ffreestanding -m32 -fno-pie -c -nostdlib -O2 -std=c99 -fno-stack-protector -finline-functions -ffreestanding -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-format -pedantic -fno-omit-frame-pointer -D_KERNEL_ -c -o $NAME.cmd $FILE
