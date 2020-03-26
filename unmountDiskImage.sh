#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [[ "$OSTYPE" == "linux-gnu" ]]; then
        sudo umount "${DIR}/build/mnt"
elif [[ "$OSTYPE" == "darwin"* ]]; then
        umount "${DIR}/build/mnt"
        disk=$(cat "mounted_disk")
        hdiutil detach "${disk}"
        rm mounted_disk    
fi