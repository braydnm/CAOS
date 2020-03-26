#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [[ "$OSTYPE" == "linux-gnu" ]]; then
        sudo umount "${DIR}/mnt"
elif [[ "$OSTYPE" == "darwin"* ]]; then
        umount mnt/
        disk=$(cat "mounted_disk")
        hdiutil detach "${disk}"
        rm mounted_disk    
fi