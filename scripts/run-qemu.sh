#!/bin/bash

set -e

if [ $# -lt 1 ]; then
  echo "Please input <image_name>"
  exit
fi

if ! which qemu-img >/dev/null 2>&1; then
  echo "Please install qemu-utils package"
  exit
fi

root=$(dirname $(dirname "$0"))
kernel_dir="$root/kernel"

img_name="$kernel_dir/$1"
efi_file="$kernel_dir/BOOTX64.EFI"

echo "img_name: $img_name"
echo "efi_file: $efi_file"

if [ -e $img_name ]; then
  rm $img_name
fi

qemu-img create -f raw $img_name 200M
mkfs.fat -n nkos -s 2 -f 2 -R 32 -F 32 $img_name

mount_point="$root/mnt"

if [ ! -d $mount_point ]; then
  echo "Created mnt dir"
  mkdir -p $mount_point
fi

sudo mount -o loop $img_name $mount_point

boot_dir="$mount_point/EFI/BOOT"

if [ ! -d $boot_dir ]; then
  echo "Created mnt/EFI/BOOT dir"
  sudo mkdir -p $boot_dir
fi

sudo cp $efi_file "$kernel_dir/kernel.elf" "$boot_dir/"
ls $boot_dir

sudo umount $mount_point

qemu-system-x86_64 \
  -drive if=pflash,file="$kernel_dir/OVMF_CODE.fd" \
  -drive if=pflash,file="$kernel_dir/OVMF_VARS.fd" \
  -hda $img_name \
	-monitor stdio
