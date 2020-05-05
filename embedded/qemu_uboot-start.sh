#!/bin/bash
arch=vexpress-a9
memory_size=512M
uboot_path=/home/inspire/u-boot-2020.04/u-boot
qemu-system-arm -M ${arch} -m ${memory_size} -kernel ${uboot_path} -nographic
