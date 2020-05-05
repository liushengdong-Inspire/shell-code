#!/bin/bash
base=`pwd`
tmpfs=/home/inspire/busybox-1.31.0/_tmpfs
export PATH=/usr/local/arm-2014.05/bin:$PATH
export ARCH=arm
export SUBARCH=arm
export CROSS_COMPILE=arm-none-linux-gnueabi-
export CONFIG_PREFIX=${base}/_install
export CONFIG_ROOTFS=/home/inspire/nfs
