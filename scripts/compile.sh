#!/bin/bash

set -e

ARCH=$(uname -m)
echo "ARCH: $ARCH"

if [ $# -lt 1 ]; then
	echo "Please input target file"
	exit 1
fi

root=$(dirname $(dirname "$0"))
echo "root: $root"

target_file=$1
echo "target_file: $target_file"
file_path="${target_file%/*}"
echo "file_path: $file_path"
options=$2
echo "options: $options"

CXXFLAGS="-O2 -Wall -g --target=$ARCH-elf
-ffreestanding -fno-exceptions -fno-rtti -mno-red-zone -nostdlibinc -std=c++17
-I/usr/include -I/usr/include/c++/11 -I/usr/include/$ARCH-linux-gnu -I/usr/include/$ARCH-linux-gnu/c++/11
-D__ELF__ -D_LDBL_EQ_DBL -D_GNU_SOURCE -D_POSIX_TIMERS"

if [[ -n $options ]]; then
	CXXFLAGS="$CXXFLAGS $options"
fi
echo "CXXFLAGS: $CXXFLAGS"

output_file="${target_file%.cpp}.o"
echo "output_file: $output_file"

clang++ $CXXFLAGS -o $output_file -c $target_file

ld.lld --entry KernelMain -z norelro --image-base 0x100000 --static -o "$file_path/kernel.elf" $output_file
