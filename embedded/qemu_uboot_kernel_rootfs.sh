#!/bin/bash
arch=vexpress-a9
memory_size=512M
uboot_path=/home/inspire/u-boot-2020.04/u-boot
Macaddr=00:16:3e:00:00:0a
rootfs_path=/home/inspire/busybox-1.31.0/a9rootfs.ext3
sudo qemu-system-arm -M ${arch} -m ${memory_size} -kernel ${uboot_path} -nographic -net nic,macaddr=${Macaddr} -net tap -sd ${rootfs_path}
