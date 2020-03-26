#!/bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
CC=gcc
CXX=g++
LD=ld.bfd
PREFIX="$SCRIPT_DIR/toolchain"
TARGET=i386-elf
PATH="$PREFIX/bin:$PATH"

mkdir -p "$SCRIPT_DIR/toolchain/tmp"

pushd "$SCRIPT_DIR/toolchain/tmp"

    wget "https://ftp.gnu.org/gnu/binutils/binutils-2.37.tar.gz"
    wget "https://ftp.gnu.org/gnu/gcc/gcc-11.3.0/gcc-11.3.0.tar.gz"

    tar -xvf binutils-2.37.tar.gz
    mkdir -p binutils-2.37/build
    pushd binutils-2.37/build
        ../configure --prefix=$PREFIX --target=$TARGET --disable-nls --disable-werror
        make -j $(nproc)
        make install
    popd

    tar -xvf gcc-11.3.0.tar.gz
    mkdir -p gcc-11.3.0/build
    pushd gcc-11.3.0
        ./contrib/download_prerequisites
        pushd build
            ../configure --target=$TARGET --prefix="$PREFIX" --disable-nls
            #make -j $(nproc) all
            make -j $(nproc) all-target-libgcc CFLAGS_FOR_TARGET="-mcmodel=large"
            make install-gcc
            make install-target-libgcc
            #make install
        popd
    popd

popd

BIN_PATH="$PREFIX/bin"

rm -rf "$SCRIPT_DIR/toolchain/tmp"
