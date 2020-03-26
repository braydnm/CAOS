#!/bin/bash

BINUTILS_VERSION="2.34"
GCC_VERSION="9.3.0"

mkdir -p toolchain

wget "https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz"
wget "https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz"

tar -xzvf binutils-$BINUTILS_VERSION.tar.xz
tar -xzvf gcc-$GCC_VERSION.tar.xz

cd binutils-$BINUTILS_VERSION
./configure --prefix="$(pwd)/../toolchain" --target=i386-elf --disable-nls
make -j 8
make install

cd ../gcc-$GCC_VERSION
./configure --prefix="$(pwd)/../toolchain" --target=i386-elf --disable-nls --enable-languages=c --without-headers
make all-gcc -j 8
make install-gcc

cd ..
sudo rm -r gcc-$GCC_VERSION*
sudo rm -r binutils-$BINUTILS_VERSION*