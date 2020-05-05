#!/bin/bash
arch=vexpress-a9
memory_size=512M
kernel_path=/home/inspire/linux-5.6.8/arch/arm/boot/zImage
dtb_path=//home/inspire/linux-5.6.8/arch/arm/boot/dts/vexpress-v2p-ca9.dtb
qemu-system-arm  -M ${arch} -m ${memory_size} -kernel ${kernel_path} -dtb ${dtb_path} -nographic -append "console=ttyAMA0"
