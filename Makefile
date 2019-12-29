C_SOURCES = $(shell find . -not -path "./cmake-build-debug*" -not -path "./modules*" -not -path "./cmdModules*" -type f -name '*.c')  #$(wildcard kernel/*.c drivers/*.c signalHandling/*.c utils/*.c)
HEADERS = $(shell find . -not -path "./cmake-build-debug*" -not -path "./modules*" -not -path "./cmdModules*" -type f -name '*.h')   #$(wildcard kernel/*.h drivers/*.h signalHandling/*.h utils/*.h)
# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o}

KERNEL_MODULE_SOURCES = $(shell find modules/ -type f -name "*.c")
KERNEL_MODULE_OBJ = ${KERNEL_MODULE_SOURCES:.c=.ko}

COMMAND_MODULE_SOURCES = $(shell find cmdModules/ -type f -name "*.c")
COMMAND_MODULE_OBJ = ${COMMAND_MODULE_SOURCES:.c=.cmd}

AS_OBJ = signalHandling/interruptAccept.o kernel/boot.o

# Change this if your cross-compiler is somewhere else
CC = gcc -m32 -fno-pie -march=i686
NM = i686-elf-nm
# -g: Use debugging symbols in gcc
CFLAGS = -finline-functions -fno-stack-protector -nostdinc -ffreestanding -g

KCFLAGS  = -std=c99
KCFLAGS += -finline-functions -ffreestanding -fno-stack-protector
KCFLAGS += -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-format
KCFLAGS += -pedantic -fno-omit-frame-pointer
KCFLAGS += -D_KERNEL_
KCFLAGS += -DKERNEL_GIT_TAG=$(shell util/make-version)
KASFLAGS = --32

current_dir = $(shell pwd)

image: ${COMMAND_MODULE_OBJ} ${KERNEL_MODULE_OBJ} CAOS
	sudo mount ramdisk.img ${current_dir}/mnt/
	sudo cp modules/*.ko ${current_dir}/mnt/modules/
	sudo cp cmdModules/*.cmd ${current_dir}/mnt/commands/
	sudo umount ${current_dir}/mnt/
	./mountDisk.sh
	sudo cp CAOS ${current_dir}/mnt/boot
	sudo cp ramdisk.img ${current_dir}/mnt/boot
	./unmountDiskImage.sh
	rm CAOS

modules/%.ko: modules/%.c
	./modules/make.sh $<

cmdModules/%.cmd: cmdModules/%.c
	./cmdModules/make.sh $<

CAOS: ${AS_OBJ} ${OBJ} kernel/symbols.o
	gcc -m32 -march=i686 -T kernel/linker.ld ${KCFLAGS} -g -nostdlib -o $@ ${AS_OBJ} ${OBJ} kernel/symbols.o
	#ld -T kernel/linker.ld -melf_i386 -o $@ $^

kernel/symbols.o: ${AS_OBJ} ${OBJ} genSymbols.py
	-rm -f kernel/symbols.o
	gcc -m32 -T kernel/linker.ld ${KCFLAGS} -nostdlib -o CAOS ${AS_OBJ} ${OBJ}
	${NM} CAOS -g | python genSymbols.py > kernel/symbols.S
	as --32 kernel/symbols.S -o $@
	-rm -f CAOS

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	gcc -m32 ${KCFLAGS} -nostdlib -g -c -o $@ $<

%.o: %.asm
	nasm $< -f elf32 -o $@

%.o: %.s
	as --32 $< -o $@
	#yasm $< -f elf32 -o $@

%.o: %.yasm
	yasm $< -f elf32 -o $@

clean:
	#$(MAKE) clean -C boot
	rm -rf ${OBJ}
