## CAOS
CAOS is an operating system I wrote as a response to a challenge that I could not write one and it has grown from there. This is the second rewrite of the OS and it has been a work in progress for a year and half year.

## Building

### Requirements
- yasm
- nasm
- gcc

### Steps
```shell
git clone https://github.com/braydnm/CAOS
cd CAOS
./build_toolchain.sh
make
```

## Running

### Requirements
- qemu

### Steps
```shell
qemu-system-i386 -serial stdio -hda ./disk.img
```
