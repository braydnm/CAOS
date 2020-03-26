#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [[ "$OSTYPE" == "linux-gnu" ]]; then
        sudo mount -t auto -o loop,offset=1048576 "${DIR}/disk.img" "${DIR}/build/mnt"
elif [[ "$OSTYPE" == "darwin"* ]]; then
        disk=$(hdiutil attach -nomount "${DIR}/disk.img" | head -n1 | cut -d " " -f1)
        echo "$disk" > "mounted_disk"
        fuse-ext2 -o force "${disk}s1" "${DIR}/build/mnt"        
fi
