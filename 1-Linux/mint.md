---
title: 从零开始构建Linux Mint镜像
date: 2023/11/19
---

主机：Ubuntu 20.04 Desktop

构建Mint版本：Linux Mint 21.2(victoria)

主要参考：https://github.com/mvallim/live-custom-ubuntu-from-scratch

# 获取Linux Mint软件包列表

Linux Mint 是在Ubuntu的基础上增加了来自Mint的开源软件包，故可以从Ubuntu22.04(jammy)内核出发构建Linux Mint。首先要知道Linux Mint 21.2镜像中有那些软件包，二、iso解压文件分析 中得到的casper/filesystem.mannifest文件列出了iso文件系统中包含的所有软件包：

对此文件做如下处理，得到后续可以直接传给apt安装的软件包列表：

<img src="mint/image (9).png" alt="image (9)" style="zoom: 67%;" />

1. 为了尽量减少软件包版本不对导致安装出错，删除所有不包含mint字符串的版本号。一个实现方法是在vscode中ctrl+H打开替换，再打开正则表达式选项，用\t((?!mint).)*$替换空。

<img src="mint/image.png" alt="image" style="zoom: 50%;" />

2. ubiquity是系统安装程序软件包，如果直接把版本号删除，会导致后续安装成ubuntu的安装程序，但列表中的版本不够新，把这三个包的mint11后缀改为mint12。

   <img src="mint/image (10).png" alt="image (10)" style="zoom:67%;" />

3. 从列表中删除这几个特定版本内核的软件包的这几行。

<img src="mint/image (11).png" alt="image (11)" style="zoom:67%;" />

4. 把文件中的制表符\t都替换为=，为了匹配apt安装软件包的格式。

最后得到的结果保存为filesystem.manifest备用。[参考](https://www.djhuang.top/20220114/filesystem.manifest)

# 宿主机安装前置软件

```

sudo apt-get install \
  binutils \
  debootstrap \
  squashfs-tools \
  xorriso \
  grub-pc-bin \
  grub-efi-amd64-bin \
  mtools
```

# debootstrap 构建基础文件系统

[debootstrap](https://packages.debian.org/debootstrap) 是一个可以快速获得基本 Debian 系统的一个工具。

```
mkdir $HOME/mint-from-scratch
```

从Ubuntu jammy出发构造镜像，并为文件系统挂载过程中会用到的设备：

```
sudo debootstrap  --arch=amd64  --variant=minbase  jammy  $HOME/mint-from-scratch/chroot  https://mirrors.tuna.tsinghua.edu.cn/ubuntu/

sudo mount --bind /dev $HOME/mint-from-scratch/chroot/dev
sudo mount --bind /run $HOME/mint-from-scratch/chroot/run

sudo cp **/filesystem.manifest $HOME/mint-from-scratch/chroot/ # 把之前得到的软件包列表复制到文件系统中
```

# chroot中安装Linux Mint软件包

chroot是起源于Unix系统的一个操作，作用于正在运行的进程和它的子进程，改变它的根目录。一个运行在这个环境下，经由chroot设置根目录的程序，它不能够对这个指定根目录之外的文件进行访问动作。

进入chroot环境：

```
sudo chroot $HOME/mint-from-scratch/chroot
```

设置挂载点、home和local：

```
mount none -t proc /proc
mount none -t sysfs /sys
mount none -t devpts /dev/pts
export HOME=/root
export LC_ALL=C
```

修改hostname、软件包换源：

```
echo "mint" > /etc/hostname

cat <<EOF > /etc/apt/sources.list.d/official-package-repositories.list
deb https://mirrors.tuna.tsinghua.edu.cn/linuxmint/ victoria main upstream import backport
EOF

cat <<EOF > /etc/apt/sources.list
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ jammy main restricted universe multiverse
# deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ jammy main restricted universe multiverse
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ jammy-updates main restricted universe multiverse
# deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ jammy-updates main restricted universe multiverse
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ jammy-backports main restricted universe multiverse
# deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ jammy-backports main restricted universe multiverse
deb http://security.ubuntu.com/ubuntu/ jammy-security main restricted universe multiverse
# deb-src http://security.ubuntu.com/ubuntu/ jammy-security main restricted universe multiverse
# deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ jammy-proposed main restricted universe multiverse
# deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ jammy-proposed main restricted universe multiverse
EOF
```

```
apt update
```

添加Linux Mint仓库的pgp key、安装systemd：

```
apt install -y gnupg
apt-key adv --keyserver keyserver.ubuntu.com --recv-keys A6616109451BBBF2
apt update

apt install -y libterm-readline-gnu-perl systemd-sysv
```

设置machine-id和divert：

```
dbus-uuidgen > /etc/machine-id
ln -fs /etc/machine-id /var/lib/dbus/machine-id

dpkg-divert --local --rename --add /sbin/initctl
ln -s /bin/true /sbin/initctl
```

```
apt -y upgrade
```

安装软件包：

```
apt install -y  sudo  ubuntu-standard  casper  discover  laptop-detect  os-prober  network-manager  resolvconf  net-tools  wireless-tools  wpagui  locales  grub-common  grub-gfxpayload-lists  grub-pc  grub-pc-bin  grub2-common $(cat filesystem.manifest) # 这里是之前获得的软件包列表
```

Configure keyboard：

<img src="mint/image (1).png" alt="image (1)" style="zoom:50%;" />

<img src="mint/image (2).png" alt="image (2)" style="zoom:50%;" />

Console setup：

<img src="mint/image (3).png" alt="image (3)" style="zoom:50%;" />

Generate locales：

```
dpkg-reconfigure locales
```

<img src="mint/image (4).png" alt="image (4)" style="zoom:50%;" />

<img src="mint/image (5).png" alt="image (5)" style="zoom:50%;" />

Reconfigure resolvconf：

```
dpkg-reconfigure resolvconf
```

<img src="mint/image (6).png" alt="image (6)" style="zoom:50%;" />

<img src="mint/image (7).png" alt="image (7)" style="zoom:50%;" />

Configure network-manager：

```
cat <<EOF > /etc/NetworkManager/NetworkManager.conf
[main]
rc-manager=resolvconf
plugins=ifupdown,keyfile
dns=dnsmasq

[ifupdown]
managed=false
EOF
```

Reconfigure network-manager：

```
dpkg-reconfigure network-manager
```

# 退出chroot环境

```
truncate -s 0 /etc/machine-id

rm /sbin/initctl    # Remove the diversion
dpkg-divert --rename --remove /sbin/initctl

apt-get clean
rm -rf /tmp/* ~/.bash_history
umount /proc
umount /sys
umount /dev/pts
export HISTSIZE=0
exit
```

```
sudo umount $HOME/mint-from-scratch/chroot/dev
sudo umount $HOME/mint-from-scratch/chroot/run
```

# 创建镜像目录

```
cd $HOME/mint-from-scratch

mkdir -p image/{casper,isolinux,install}
sudo cp chroot/boot/vmlinuz-**-**-generic image/casper/vmlinuz
sudo cp chroot/boot/initrd.img-**-**-generic image/casper/initrd

wget --progress=dot https://www.memtest86.com/downloads/memtest86-usb.zip -O image/install/memtest86-usb.zip
unzip -p image/install/memtest86-usb.zip memtest86-usb.img > image/install/memtest86
rm -f image/install/memtest86-usb.zip
```

# GRUB启动配置

```
cd $HOME/mint-from-scratch
touch image/mint
```

```
cat <<EOF > image/isolinux/grub.cfg

search --set=root --file /mint

insmod all_video

set default="0"
set timeout=30

menuentry "Try Mint FS without installing" {
   linux /casper/vmlinuz boot=casper nopersistent toram quiet splash ---
   initrd /casper/initrd
}

menuentry "Install Mint FS" {
   linux /casper/vmlinuz boot=casper only-ubiquity quiet splash ---
   initrd /casper/initrd
}

menuentry "Check disc for defects" {
   linux /casper/vmlinuz boot=casper integrity-check quiet splash ---
   initrd /casper/initrd
}

menuentry "Test memory Memtest86+ (BIOS)" {
   linux16 /install/memtest86+
}

menuentry "Test memory Memtest86 (UEFI, long load time)" {
   insmod part_gpt
   insmod search_fs_uuid
   insmod chain
   loopback loop /install/memtest86
   chainloader (loop,gpt1)/efi/boot/BOOTX64.efi
}
EOF
```

```
sudo chroot chroot dpkg-query -W --showformat='${Package} ${Version}\n' | sudo tee image/casper/filesystem.manifest
sudo cp -v image/casper/filesystem.manifest image/casper/filesystem.manifest-desktop
sudo sed -i '/ubiquity/d' image/casper/filesystem.manifest-desktop
sudo sed -i '/casper/d' image/casper/filesystem.manifest-desktop
sudo sed -i '/discover/d' image/casper/filesystem.manifest-desktop
sudo sed -i '/laptop-detect/d' image/casper/filesystem.manifest-desktop
sudo sed -i '/os-prober/d' image/casper/filesystem.manifest-desktop
```

# 构建镜像

压缩文件系统、获取文件系统大小：

```
cd $HOME/mint-from-scratch
sudo mksquashfs chroot image/casper/filesystem.squashfs
printf $(sudo du -sx --block-size=1 chroot | cut -f1) > image/casper/filesystem.size
```

创建image/README.diskdefines文件：

```
cat <<EOF > image/README.diskdefines
#define DISKNAME  Mint from scratch
#define TYPE  binary
#define TYPEbinary  1
#define ARCH  amd64
#define ARCHamd64  1
#define DISKNUM  1
#define DISKNUM1  1
#define TOTALNUM  0
#define TOTALNUM0  1
EOF
```

进入image目录：

```
cd $HOME/mint-from-scratch/image
```

Create a grub UEFI image：

```
grub-mkstandalone \
   --format=x86_64-efi \
   --output=isolinux/bootx64.efi \
   --locales="" \
   --fonts="" \
   "boot/grub/grub.cfg=isolinux/grub.cfg"
```

Create a FAT16 UEFI boot disk image containing the EFI bootloader：

```
(
   cd isolinux && \
   dd if=/dev/zero of=efiboot.img bs=1M count=10 && \
   sudo mkfs.vfat efiboot.img && \
   LC_CTYPE=C mmd -i efiboot.img efi efi/boot && \
   LC_CTYPE=C mcopy -i efiboot.img ./bootx64.efi ::efi/boot/
)
```

Create a grub BIOS image：

```
grub-mkstandalone \
   --format=i386-pc \
   --output=isolinux/core.img \
   --install-modules="linux16 linux normal iso9660 biosdisk memdisk search tar ls" \
   --modules="linux16 linux normal iso9660 biosdisk search" \
   --locales="" \
   --fonts="" \
   "boot/grub/grub.cfg=isolinux/grub.cfg"
```

Combine a bootable Grub cdboot.img：

```
cat /usr/lib/grub/i386-pc/cdboot.img isolinux/core.img > isolinux/bios.img
```

Generate md5sum.txt：

```
sudo /bin/bash -c "(find . -type f -print0 | xargs -0 md5sum | grep -v -e 'md5sum.txt' -e 'bios.img' -e 'efiboot.img' > md5sum.txt)"
```

Create iso from the image directory using the command-line：

```
sudo xorriso \
   -as mkisofs \
   -iso-level 3 \
   -full-iso9660-filenames \
   -volid "Mint from scratch" \
   -output "../mint-from-scratch.iso" \
   -eltorito-boot boot/grub/bios.img \
      -no-emul-boot \
      -boot-load-size 4 \
      -boot-info-table \
      --eltorito-catalog boot/grub/boot.cat \
      --grub2-boot-info \
      --grub2-mbr /usr/lib/grub/i386-pc/boot_hybrid.img \
   -eltorito-alt-boot \
      -e EFI/efiboot.img \
      -no-emul-boot \
   -append_partition 2 0xef isolinux/efiboot.img \
   -m "isolinux/efiboot.img" \
   -m "isolinux/bios.img" \
   -graft-points \
      "/EFI/efiboot.img=isolinux/efiboot.img" \
      "/boot/grub/bios.img=isolinux/bios.img" \
      "."
```

上级目录得到制作完成Linux Mint镜像。

<img src="mint/image (8).png" alt="image (8)" style="zoom: 67%;" />
