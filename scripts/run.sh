#!/bin/bash

set -e

root=$(dirname $(dirname "$0"))
echo "root: $root"

kernel_path="$root/kernel"
cd $kernel_path
echo "kernel_path: $kernel_path"
make clean
make

script_path="$root/scripts"
echo "script_path: $script_path"
cd $script_path
./copy.sh
./run-qemu.sh $kernel_path/disk.img
