---
title: Linux 下 objdump 指令和 readelf 指令
date: 2022/5/23
---
# objdump
`-h`: 各个段的基本信息（段表）

<img src="Linux-elf-command/image-20220526193418335.png" alt="image-20220526193418335" style="zoom:80%;" />

`-s`: 所有段的内容十六进制打印

<img src="Linux-elf-command/image-20220526193501337.png" alt="image-20220526193501337" style="zoom:80%;" />

`-S`: 显示所有内容

<img src="Linux-elf-command/image-20220526193526961.png" alt="image-20220526193526961" style="zoom: 67%;" />

`-d`: 所有包含指令的段反汇编

<img src="Linux-elf-command/image-20220526193605188.png" alt="image-20220526193605188" style="zoom:67%;" />

`-disassemble=<NAME>`：显示某段反汇编的内容

<img src="Linux-elf-command/image-20220608151127703.png" alt="image-20220608151127703" style="zoom:67%;" />

`-g`: 显示调试信息

<img src="Linux-elf-command/image-20220526193707677.png" alt="image-20220526193707677" style="zoom:67%;" />

`-r`: 显示重定位信息

<img src="Linux-elf-command/image-20220526193738821.png" alt="image-20220526193738821" style="zoom:67%;" />

# readelf
`-h`: 读取elf文件头的内容

<img src="Linux-elf-command/image-20220526193755785.png" alt="image-20220526193755785" style="zoom:67%;" />

`-S`: 全部段的信息（段表）

<img src="Linux-elf-command/image-20220526193811593.png" alt="image-20220526193811593" style="zoom: 67%;" />

`-s`: 符号表

<img src="Linux-elf-command/image-20220526193825769.png" alt="image-20220526193825769" style="zoom:67%;" />

`-l`：程序头，执行视图

<img src="Linux-elf-command/image-20220608154232614.png" alt="image-20220608154232614" style="zoom:67%;" />

`-d`：查看动态链接相关信息

<img src="Linux-elf-command/image-20220610145213673.png" alt="image-20220610145213673" style="zoom:67%;" />

# ldd

查看程序主模块或共享库依赖于其它什么模块

<img src="Linux-elf-command/image-20220610150022818.png" alt="image-20220610150022818" style="zoom:67%;" />
