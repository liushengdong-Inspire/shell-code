arch=vexpress-a9
memory_size=512M
kernel_path=/home/inspire/linux-5.6.8/arch/arm/boot/zImage
dtb_path=/home/inspire/linux-5.6.8/arch/arm/boot/dts/vexpress-v2p-ca9.dtb
system_file=a9rootfs.ext3
sudo qemu-system-arm -M ${arch} -m ${memory_size} -kernel ${kernel_path} -dtb ${dtb_path} -nographic -append "root=/dev/mmcblk0 rw console=ttyAMA0" -sd ${system_file}
