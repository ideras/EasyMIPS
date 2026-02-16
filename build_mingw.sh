#!/bin/bash

if [ -z $1 ]; then
    echo "Please specify the Toolchain file path"
    exit 1
fi

mkdir build-windows
if [ $? -ne 0 ]; then
    exit 1
fi

cd build-windows
cmake -DCMAKE_TOOLCHAIN_FILE=$1 ../
