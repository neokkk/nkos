#!/bin/bash

set -ex

ARCH=$(uname -m)

BASE_DIR=/usr/local/src
PREFIX=/usr/local/$ARCH-elf
TARGET_TRIPLE=$ARCH-elf

CC=/usr/bin/clang
CXX=/usr/bin/clang++

COMMON_CFLAGS="-nostdlibinc -O2 -D__ELF__ -D_LDBL_EQ_DBL -D_GNU_SOURCE -D_POSIX_TIMERS -I$PREFIX/include"
COMMON_CXXFLAGS="-lpthread -I/usr/include -I/usr/include/c++/11 -I/usr/include/$ARCH-linux-gnu -I/usr/include/$ARCH-linux-gnu/c++/11 -I/usr/include/llvm-14/llvm/Support -I/usr/include/llvm-18/llvm/Support -I$BASE_DIR/llvm-project/libcxx/include"

cd $BASE_DIR
git clone --depth 1 --branch fix-build https://github.com/uchan-nos/newlib-cygwin.git

cd $BASE_DIR
mkdir -p build_newlib
cd build_newlib

../newlib-cygwin/newlib/configure \
  CC=$CC \
  CC_FOR_BUILD=$CC \
  CFLAGS="-fPIC $COMMON_CFLAGS" \
  --target=$TARGET_TRIPLE --prefix=$PREFIX --disable-multilib --disable-newlib-multithread

make -j 4
make install

cd $BASE_DIR
git clone --depth 1 https://github.com/llvm/llvm-project.git

cd $BASE_DIR
mkdir -p build_libcxxabi
cd build_libcxxabi

cmake -S llvm -G Ninja "Unix Makefiles" \
  -DCMAKE_PROJECT_NAME=libcxxabi \
  -DCMAKE_INSTALL_PREFIX=$PREFIX \
  -DCMAKE_CXX_COMPILER=$CXX \
  -DCMAKE_CXX_FLAGS="$COMMON_CFLAGS $COMMON_CXXFLAGS -D_LIBCPP_HAS_NO_THREADS" \
  -DCMAKE_C_COMPILER=$CC \
  -DCMAKE_C_FLAGS="$COMMON_CFLAGS -D_LIBCPP_HAS_NO_THREADS" \
  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
  -DCMAKE_UILD_TYPE=RelWithDebInfo \
  -DCMAKE_BUILD_TYPE=Release \
  -DLIBCXXABI_LIBCXX_INCLUDES="$BASE_DIR/llvm-project/libcxx/include" \
  -DLIBCXXABI_ENABLE_EXCEPTIONS=False \
  -DLLVM_ENABLE_RUNTIMES="compiler-rt;libcxx;libcxxabi" \
  -DLIBCXXABI_ENABLE_THREADS=False \
  -DLIBCXXABI_TARGET_TRIPLE=$TARGET_TRIPLE \
  -DLIBCXXABI_ENABLE_SHARED=False \
  -DLIBCXXABI_ENABLE_STATIC=True \
  -DLIBCXXABI_USE_LLVM_UNWINDER=OFF \
  -DLIBCXX_HARDENING_MODE=none \
  $BASE_DIR/llvm-project/libcxxabi

cmake --build . --target install

cd $BASE_DIR
mkdir build_libcxx

cmake -S llvm -G Ninja "Unix Makefiles" \
  -DCMAKE_PROJECT_NAME=libcxx \
  -DCMAKE_INSTALL_PREFIX=$PREFIX \
  -DCMAKE_CXX_COMPILER=$CXX \
  -DCMAKE_CXX_FLAGS="$COMMON_CFLAGS $COMMON_CXXFLAGS" \
  -DCMAKE_CXX_COMPILER_TARGET=$TARGET_TRIPLE \
  -DCMAKE_C_COMPILER=$CC \
  -DCMAKE_C_FLAGS="$COMMON_CFLAGS" \
  -DCMAKE_C_COMPILER_TARGET=$TARGET_TRIPLE \
  -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY \
  -DCMAKE_UILD_TYPE=RelWithDebInfo \
  -DCMAKE_BUILD_TYPE=Release \
  -DLIBCXX_CXX_ABI=libcxxabi \
  -DLIBCXX_CXX_ABI_INCLUDE_PATHS="$BASE_DIR/llvm-project/libcxxabi/include" \
  -DLIBCXX_CXX_ABI_LIBRARY_PATH="$PREFIX/lib" \
  -DLIBCXX_ENABLE_EXCEPTIONS=False \
  -DLIBCXX_ENABLE_FILESYSTEM=False \
  -DLIBCXX_ENABLE_MONOTONIC_CLOCK=False \
  -DLIBCXX_ENABLE_RTTI=False \
  -DLIBCXX_ENABLE_THREADS=False \
  -DLIBCXX_ENABLE_SHARED=False \
  -DLIBCXX_ENABLE_STATIC=True \
  $BASE_DIR/llvm-project/libcxx

cmake --build . --target install

cd $BASE_DIR
wget https://download.savannah.gnu.org/releases/freetype/freetype-2.10.1.tar.gz
tar xf freetype-2.10.1.tar.gz

cd $BASE_DIR
mkdir build_freetype
cd build_freetype
../freetype-2.10.1/configure \
  CC=$CC \
  CFLAGS="-fPIC $COMMON_CFLAGS" \
  --host=$TARGET_TRIPLE --prefix=$PREFIX

make -j 4
make install

rm $PREFIX/lib/libfreetype.la
rm -rf $PREFIX/lib/pkgconfig
rm -rf $PREFIX/share