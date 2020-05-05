base=`pwd`
tmpfs=/home/inspire/busybox-1.31.0/_tmpfs
sudo rm -rf ${tmpfs}
sudo rm -rf ${CONFIG_ROOTFS}
sudo rm -f a9rootfs.ext3
sudo mkdir -p ${CONFIG_ROOTFS}
sudo chmod 0777 ${CONFIG_ROOTFS}
sudo cp ${CONFIG_PREFIX}/* ${CONFIG_ROOTFS}/ -arf

cd ${CONFIG_ROOTFS} && sudo mkdir -p lib proc sys tmp root var mnt && cd ${base}

sudo cp -arf /usr/local/arm-2014.05/arm-none-linux-gnueabi/libc/lib/*so* ${CONFIG_ROOTFS}/lib

sudo cp app ${CONFIG_ROOTFS}
sudo cp examples/bootfloppy/etc ${CONFIG_ROOTFS}/ -arf
sudo sed -r "/askfirst/ s/.*/::respawn:-\/bin\/sh/" ${CONFIG_ROOTFS}/etc/inittab -i
sudo mkdir -p ${CONFIG_ROOTFS}/dev/
sudo mknod ${CONFIG_ROOTFS}/dev/tty1 c 4 1
sudo mknod ${CONFIG_ROOTFS}/dev/tty2 c 4 2pro
sudo mknod ${CONFIG_ROOTFS}/dev/tty3 c 4 3
sudo mknod ${CONFIG_ROOTFS}/dev/tty4 c 4 4
sudo mknod ${CONFIG_ROOTFS}/dev/console c 5 1
sudo mknod ${CONFIG_ROOTFS}/dev/null c 1 3
sudo dd if=/dev/zero of=a9rootfs.ext3 bs=1M count=64


sudo mkfs.ext3 a9rootfs.ext3
sudo mkdir -p ${tmpfs}
sudo chmod 777 ${tmpfs}
sudo mount -t ext3 a9rootfs.ext3 ${tmpfs}/ -o loop
sudo cp -r ${CONFIG_ROOTFS}/* ${tmpfs}/
sudo umount ${tmpfs}
