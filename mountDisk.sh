#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

sudo mount -o loop,offset=1048576 "${DIR}/disk.img" "${DIR}/mnt"
