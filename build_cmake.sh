#!/bin/bash
rm -rf _cmake
mkdir _cmake
#for name in windows-x64 windows-x86; do
for name in linux-amd64 linux-armhf linux-i686 windows-x64 windows-x86; do
    mkdir _cmake/$name
    cd _cmake/$name
    cmake -DCMAKE_TOOLCHAIN_FILE=../../tools/cross-cmake/$name.cmake -G Ninja ../..
    cmake --build .
    cd ../..
done;


