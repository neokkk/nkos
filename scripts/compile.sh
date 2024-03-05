#!/bin/bash

set -e

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

CXXFLAGS="-O2 -Wall -g --target=$(uname -m)-elf -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -std=c++17"

if [[ -n $options ]]; then
	CXXFLAGS="$CXXFLAGS $options"
fi

output_file="${target_file%.cpp}.o"
echo "output_file: $output_file"

clang++ $CXXFLAGS -o $output_file -c $target_file

ld.lld --entry KernelMain -z norelro --image-base 0x100000 --static -o "$file_path/kernel.elf" $output_file
