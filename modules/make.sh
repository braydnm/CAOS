#!/bin/bash

FILE=$1

NAME=${FILE%%.*}
OUTPUT=$2

gcc -ffreestanding -static -m32 -fno-pie -c -nostdlib -O2 -std=c99 -fno-stack-protector -finline-functions -ffreestanding -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-format -pedantic -fno-omit-frame-pointer -D_KERNEL_ -c -o "${OUTPUT}" $FILE
