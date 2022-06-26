C_SOURCES = $(shell find . -not -path "./cmake-build-debug*" -not -path "./modules*" -not -path "./cmdModules*" -type f -name '*.c')  #$(wildcard kernel/*.c drivers/*.c signalHandling/*.c utils/*.c)
HEADERS = $(shell find . -not -path "./cmake-build-debug*" -not -path "./modules*" -not -path "./cmdModules*" -type f -name '*.h')   #$(wildcard kernel/*.h drivers/*.h signalHandling/*.h utils/*.h)

OUTPUT_DIR = build

# Nice syntax for file extension replacement
OBJ = $(patsubst %.c, ${OUTPUT_DIR}/%.o, ${C_SOURCES})


KERNEL_MODULE_SOURCES = $(shell find modules/ -type f -name "*.c")
KERNEL_MODULE_OBJ = $(patsubst %.c, ${OUTPUT_DIR}/%.ko, ${KERNEL_MODULE_SOURCES})

COMMAND_MODULE_SOURCES = $(shell find cmdModules/ -type f -name "*.c")
COMMAND_MODULE_OBJ = $(patsubst %.c, ${OUTPUT_DIR}/%.cmd, ${COMMAND_MODULE_SOURCES})

AS_OBJ = ${OUTPUT_DIR}/signalHandling/interruptAccept.o ${OUTPUT_DIR}/kernel/boot.o

# Change this if your cross-compiler is somewhere else
# export PATH := $(shell pwd)/toolchain/bin:$(PATH)

CC = toolchain/bin/i386-elf-gcc
NM = toolchain/bin/i386-elf-nm
AS = toolchain/bin/i386-elf-as
LD = toolchain/bin/i386-elf-ld
# -g: Use debugging symbols in gcc
CFLAGS = -finline-functions -fno-stack-protector -nostdinc -ffreestanding -g

#KCFLAGS  = -std=c99
KCFLAGS += -finline-functions -ffreestanding -fno-stack-protector -fno-pie
KCFLAGS += -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-format
KCFLAGS += -pedantic -fno-omit-frame-pointer
KCFLAGS += -D_KERNEL_
KCFLAGS += -DKERNEL_GIT_TAG=$(shell util/make-version)
KASFLAGS = --32

current_dir = $(shell pwd)

UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S), Darwin)
        MOUNT_COMMAND = sudo fuse-ext2 -o force
	else
		MOUNT_COMMAND = sudo mount
    endif

image: ${COMMAND_MODULE_OBJ} ${KERNEL_MODULE_OBJ} CAOS
	mkdir -p ${current_dir}/mnt/
	${MOUNT_COMMAND} ramdisk.img ${current_dir}/mnt/
	# sudo cp build/modules/*.ko ${current_dir}/mnt/modules/
	sudo cp build/cmdModules/*.cmd ${current_dir}/mnt/commands/
	sudo umount ${current_dir}/mnt/
	./mountDisk.sh
	sudo cp CAOS ${current_dir}/mnt/boot
	sudo cp ramdisk.img ${current_dir}/mnt/boot
	./unmountDiskImage.sh
	# rm CAOS

# ${OUTPUT_DIR}/modules/%.ko: modules/%.c
# 	mkdir -p $(@D)
# 	./modules/make.sh $< $@

${OUTPUT_DIR}/cmdModules/%.cmd: cmdModules/%.c
	mkdir -p $(@D)
	./cmdModules/make.sh $< $@

CAOS: ${AS_OBJ} ${OBJ} build/kernel/symbols.o
	${CC} -T kernel/linker.ld ${KCFLAGS} -g -nostdlib -o $@ ${AS_OBJ} ${OBJ} build/kernel/symbols.o
	#ld -T kernel/linker.ld -melf_i386 -o $@ $^

build/kernel/symbols.o: ${AS_OBJ} ${OBJ} genSymbols.py
	rm -f kernel/symbols.o
	${CC} -m32 -T kernel/linker.ld ${KCFLAGS} -nostdlib -o CAOS ${AS_OBJ} ${OBJ}
	${NM} CAOS -g | python genSymbols.py > kernel/symbols.S
	${AS} kernel/symbols.S -o $@
	rm -f CAOS

# Generic rules for wildcards
# To make an object, always compile from its .c
$(OUTPUT_DIR)/%.o: %.c ${HEADERS}
	mkdir -p $(@D)
	echo $(PATH)
	${CC} ${KCFLAGS} -nostdlib -g -c -o $@ $<

$(OUTPUT_DIR)/%.o: %.asm
	mkdir -p $(@D)
	nasm $< -f elf32 -o $@

$(OUTPUT_DIR)/%.o: %.s
	mkdir -p $(@D)
	${AS} --32 $< -o $@

$(OUTPUT_DIR)/%.o: %.yasm
	mkdir -p $(@D)
	yasm $< -f elf32 -o $@

clean:
	rm -rf build/*
