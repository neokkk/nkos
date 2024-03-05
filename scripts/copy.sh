#!/bin/bash

set -e

cd ~/Applications/edk2

source ./edksetup.sh
build

cp ./Build/NkLoaderX64/DEBUG_CLANGDWARF/X64/Loader.efi ~/Workspaces/nkos/kernel/BOOTX64.EFI
