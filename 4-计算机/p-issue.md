# QEMU 测试

## 1. GCC 工具链

工具链安装路径：

```bash
$ export RISCV=/home/*username*/riscv-build
```

获取工具链：

```bash
$ cd ~
$ git clone https://github.com/riscv-collab/riscv-gnu-toolchain.git
$ git clone https://github.com/plctlab/riscv-gcc.git -b riscv-gcc-p-ext
$ git clone https://github.com/plctlab/riscv-binutils-gdb.git -b riscv-binutils-p-ext
```

配置：

```bash
$ cd riscv-gnu-toolchain
$ ./configure --prefix=$RISCV --with-arch=rv64imafd_zicsr_zifencei_zpn --with-abi=lp64d --with-gcc-src=/home/*username*/riscv-gcc --with-binutils-src=/home/*username*/riscv-binutils-gdb
```

--prefix指定工具链安装的位置，--with-gcc 和 --with-binutils-src 分别指定 clone 的 gcc 和 gdb 目录。

编译：

```bash
$ make linux -j16
$ export PATH=$PATH:$RISCV/bin
```

## 2. QEMU

```bash
$ cd ~
$ git clone git@y.riscv.ac.cn:whale/plct-qemu.git -b plct-rvp
$ cd plct-qemu
$ ./configure --target-list=riscv64-softmmu,riscv32-softmmu,riscv64-linux-user,riscv32-linux-user
$ make -j16
```

## 3. gdb-multiarch

用于调试 qemu 的运行。

```bash
$ cd ~
$ apt download gdb-multiarch
$ dpkg -x gdb-multiarch_12.1-0ubuntu1~22.04_amd64.deb gdb-multiarch
$ export PATH=$PATH:/home/*username*/gdb-multiarch/usr/bin
$ echo 'set auto-load safe-path /' >> ~/.gdbinit
```

## 4. 编译测试运行

**1）在新建目录中创建这些文件 ：**

Makefile :

```Makefile
ARCH    = riscv64-unknown-linux-gnu
CC      = $(ARCH)-gcc
FLAGS   = -nostartfiles -g
LD      = $(ARCH)-ld
OBJCOPY = $(ARCH)-objcopy
OBJDUMP = $(ARCH)-objdump
QEMU	= /home/*username*/plct-qemu/build/qemu-system-riscv64


all: clean hello.img

hello.img: hello.elf
	$(OBJCOPY) hello.elf -I binary hello.img
	$(OBJDUMP) -d hello.elf > hello.asm

hello.elf: hello.o link.ld Makefile
	$(LD) -T link.ld -o hello.elf hello.o

hello.o: hello.s
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -f *.o hello.elf hello.img hello.asm

run: hello.img
	$(QEMU) -cpu rv64,x-p=true -M virt -bios none -serial stdio -display none -kernel hello.img

gdb:
	$(QEMU) -cpu rv64,x-p=true -s -S -M virt -bios none -serial stdio -display none -kernel hello.img
```

link.ld :

```bash
SECTIONS
{
  . = 0x80000000;
  .text : { *(.text) }
  .data : { *(.data) }
  .bss  : { *(.bss) }
}
```

hello.s:

```assembly
.global _start

_start:
    li s6, 0x0001000100010001
    li s7, 0x0002000200020002
    add16 s8, s6, s7
    
    # run only one instance
    csrr    t0, mhartid
    bnez    t0, forever
    
    # prepare for the loop
    li      s1, 0x10000000  # UART output register   
    la      s2, hello       # load string start addr into s2
    addi    s3, s2, 13      # set up string end addr in s3

loop:
    lb      s4, 0(s2)       # load next byte at s2 into s4
    sb      s4, 0(s1)       # write byte to UART register 
    addi    s2, s2, 1       # increase s2
    blt     s2, s3, loop    # branch back until end addr (s3) reached

forever:
    wfi
    j       forever


.section .data

hello:
  .string "hello world!\n"
```

.gdbinit :

```bash
file hello.elf
target remote localhost:1234
b _start
c
```

**2）直接运行：**

```bash
$ make run
hello world!
```

**3）gdb 调试：**

```bash
$ make gdb
```

在当前目录另开一个终端：

```bash
$ gdb-multiarch
```

调试汇编指令：

 ```
 0x0000000000001000 in ?? ()
 Breakpoint 1 at 0x80000000: file hello.s, line 5.
 
 Breakpoint 1, _start () at hello.s:5
 5           li s6, 0x0001000100010001
 (gdb) n
 6           li s7, 0x0002000200020002
 (gdb) n
 7           add16 s8, s6, s7
 (gdb) n
 10          csrr    t0, mhartid
 (gdb) info reg s6 s7 s8
 s6             0x1000100010001  281479271743489
 s7             0x2000200020002  562958543486978
 s8             0x3000300030003  844437815230467
 ```

[参考仓库](https://github.com/dajunHuang/riscv-hello-uart)







