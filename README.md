## CAOS
CAOS is an operating system I wrote as a response to a challenge that I could not write one and it has grown from there. This is the second rewrite of the OS and it has been a work in progress for a year and half year.

## Getting Started

The operating system can be compiled by simply running

    make

The OS has only been tested on qemu and can be run by running 

    qemu-system-i386(.exe)  -hda  disk.img

## CAOS In a Nutshell

Currently the operating system uses grub to boot and supports the multiboot specification however I hope to write my own bootloader soon.

The initialization process is as follows:
 - Read the multiboot structure passed by GRUB
 - Initialize the GDT
 - Install interrupts (IRQs, ISRs, etc)
 - Initialize a primitive form of paging
 - Initialize a primitive memory manager
 - Initialize the virtual filesystem and ext2 filesystem to read from the ramdisk
 - Load the module files from the ramdisk and load them into memory as callable functions from the command line using a primitive ELF parser
 - Mount the main hard disk
 - Wait for user input and handle it accordingly calling functions
## Next Steps
 - Clean up code
 - Write a bootloader
 - Improve memory management
 - Add syscalls/tasking/gnu library for executables
 - Add a gui (this might be a problem)
 - Write a better memory manager instead of using liballoc plugged in
