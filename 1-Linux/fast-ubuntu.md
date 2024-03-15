---
title: ubuntu 快速配置
date: 2022/10/26
---
```
sudo cp /etc/apt/sources.list /etc/apt/sources.list.bak

sudo vi /etc/apt/sources.list
//配置内容如下 https://www.yisu.com/ask/4042.html
deb http://mirrors.aliyun.com/ubuntu/ focal main restricted universe multiverse 
deb http://mirrors.aliyun.com/ubuntu/ focal-security main restricted universe multiverse 
deb http://mirrors.aliyun.com/ubuntu/ focal-updates main restricted universe multiverse 
deb http://mirrors.aliyun.com/ubuntu/ focal-proposed main restricted universe multiverse 
deb http://mirrors.aliyun.com/ubuntu/ focal-backports main restricted universe multiverse 
deb-src http://mirrors.aliyun.com/ubuntu/ focal main restricted universe multiverse 
deb-src http://mirrors.aliyun.com/ubuntu/ focal-security main restricted universe multiverse 
deb-src http://mirrors.aliyun.com/ubuntu/ focal-updates main restricted universe multiverse 
deb-src http://mirrors.aliyun.com/ubuntu/ focal-proposed main restricted universe multiverse 
deb-src http://mirrors.aliyun.com/ubuntu/ focal-backports main restricted universe multiverse focal

sudo apt update

sudo apt install gcc g++ cmake git vim tmux make vim-gtk3 python3-dev libncurses-dev perl binutils flex bison libelf-dev libssl-dev -y
```

[virtualbox shared folders](https://askubuntu.com/questions/890729/this-location-could-not-be-displayed-you-do-not-have-the-permissions-necessary)