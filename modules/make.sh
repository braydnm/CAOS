#!/bin/bash

FILE=$1

NAME=${FILE%%.*}
OUTPUT=$2

toolchain/bin/i386-elf-gcc -ffreestanding -fno-pie -c -nostdlib -O2 -std=c99 -fno-stack-protector -finline-functions -ffreestanding -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-format -pedantic -fno-omit-frame-pointer -D_KERNEL_ -Ikernel/include -c -o "${OUTPUT}" $FILE
