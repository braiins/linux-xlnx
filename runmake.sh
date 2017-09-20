#!/bin/sh

MAKE_JOBS=$2

#git co inno
#make distclean
#make ARCH=arm xilinx_zynq_defconfig
cp -rvf ./inno_files/config ./.config

mkdir -p include/generated
inno_files/inno_ver_generate.py include/inno_version.h

make -j${MAKE_JOBS} ARCH=arm CROSS_COMPILE=arm-xilinx-linux-gnueabi- UIMAGE_LOADADDR=0x8000 uImage
#cp -rvf arch/arm/boot/uImage /home/Users/wangp/work/bin/ 

