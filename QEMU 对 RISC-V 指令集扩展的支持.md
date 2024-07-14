# QEMU 对 RISC-V 指令集扩展的支持

## 一、扩展入口

### 1. 初始化

#### 单字符扩展

RISC-V 指令集有的是用单字符表示（IMAFD），有的是 Z 开头的字符串表示（Zfa，Zfinx），单字符扩展在以下几个地方被定义。

表示单字符扩展的字符的编码：[target/riscv/cpu.h#L58](https://github.com/qemu/qemu/blob/3f044554b94fc0756d5b3cdbf84501e0eea0e629/target/riscv/cpu.h#L58)

```c
/*
 * Update misa_bits[], misa_ext_info_arr[] and misa_ext_cfgs[]
 * when adding new MISA bits here.
 */
#define RVI RV('I')
#define RVE RV('E') /* E and I are mutually exclusive */
#define RVM RV('M')
#define RVA RV('A')
#define RVF RV('F')
#define RVD RV('D')
```

包含所有可用单字符扩展的数组：[target/riscv/cpu.c#L44](https://github.com/qemu/qemu/blob/3f044554b94fc0756d5b3cdbf84501e0eea0e629/target/riscv/cpu.c#L44)

```c
/* RISC-V CPU definitions */
static const char riscv_single_letter_exts[] = "IEMAFDQCBPVH";
const uint32_t misa_bits[] = {RVI, RVE, RVM, RVA, RVF, RVD, RVV,
                              RVC, RVS, RVU, RVH, RVJ, RVG, RVB, 
                              RVP, 0};
```

默认是否使能单字符扩展：[target/riscv/tcg/tcg-cpu.c#L1019](https://github.com/qemu/qemu/blob/3f044554b94fc0756d5b3cdbf84501e0eea0e629/target/riscv/tcg/tcg-cpu.c#L1019)

```c
static const RISCVCPUMisaExtConfig misa_ext_cfgs[] = {
    MISA_CFG(RVA, true),
    MISA_CFG(RVC, true),
    MISA_CFG(RVD, true),
    MISA_CFG(RVF, true),
    MISA_CFG(RVI, true),
	... ...
    MISA_CFG(RVJ, false),
    MISA_CFG(RVV, false),
    MISA_CFG(RVG, false),
    MISA_CFG(RVB, false),
};
```

单字符扩展相关配置信息：[target/riscv/cpu.c#L1393](https://github.com/qemu/qemu/blob/3f044554b94fc0756d5b3cdbf84501e0eea0e629/target/riscv/cpu.c#L1393)

```c
static const MISAExtInfo misa_ext_info_arr[] = {
    MISA_EXT_INFO(RVA, "a", "Atomic instructions"),
    MISA_EXT_INFO(RVC, "c", "Compressed instructions"),
    MISA_EXT_INFO(RVD, "d", "Double-precision float point"),
    MISA_EXT_INFO(RVF, "f", "Single-precision float point"),
	... ...
    MISA_EXT_INFO(RVJ, "x-j", "Dynamic translated languages"),
    MISA_EXT_INFO(RVV, "v", "Vector operations"),
    MISA_EXT_INFO(RVG, "g", "General purpose (IMAFD_Zicsr_Zifencei)"),
    MISA_EXT_INFO(RVB, "b", "Bit manipulation (Zba_Zbb_Zbs)")
};
```

其中`MISA_EXT_INFO` 的第三个参数是扩展的描述信息，第二个参数是用来使能扩展的  qemu  命令行选项，如配置 cpu 为 64 位且使能 j 扩展，则 qemu 启动参数需要加上 `-cpu rv64,x-j=true`，其中"x-"表示该扩展为实验性扩展。

#### 其它扩展

RISC-V 单字符扩展以外的扩展和单字符扩展的子扩展由字符串来表示，这些扩展在以下几个地方被定义：

处理器扩展选项：[target/riscv/cpu_cfg.h#L39](https://github.com/qemu/qemu/blob/3f044554b94fc0756d5b3cdbf84501e0eea0e629/target/riscv/cpu_cfg.h#L39)

```c
struct RISCVCPUConfig {
    bool ext_zba;
    bool ext_zbb;
    bool ext_zbc;
    bool ext_zbkb;
    bool ext_zbkc;
    bool ext_zbkx;
	... ...
```

扩展配置信息：[target/riscv/cpu.c#L1467](https://github.com/qemu/qemu/blob/3f044554b94fc0756d5b3cdbf84501e0eea0e629/target/riscv/cpu.c#L1467)

```c
const RISCVCPUMultiExtConfig riscv_cpu_extensions[] = {
    /* Defaults for standard extensions */
    MULTI_EXT_CFG_BOOL("sscofpmf", ext_sscofpmf, false),
    MULTI_EXT_CFG_BOOL("zifencei", ext_zifencei, true),
    MULTI_EXT_CFG_BOOL("zicsr", ext_zicsr, true),
    MULTI_EXT_CFG_BOOL("zihintntl", ext_zihintntl, true),
    MULTI_EXT_CFG_BOOL("zihintpause", ext_zihintpause, true),
    ... ...
```

其中`MULTI_EXT_CFG_BOOL` 的第三个参数表示该扩展默认是否被使能，第一个参数是用来使能扩展的 qemu 命令行选项，用法和单字符扩展相同。

另一个扩展入口：[target/riscv/cpu.c#L100](https://github.com/qemu/qemu/blob/3f044554b94fc0756d5b3cdbf84501e0eea0e629/target/riscv/cpu.c#L100C1-L105C70)

```c
const RISCVIsaExtData isa_edata_arr[] = {
    ISA_EXT_DATA_ENTRY(zic64b, PRIV_VERSION_1_12_0, ext_zic64b),
    ISA_EXT_DATA_ENTRY(zicbom, PRIV_VERSION_1_12_0, ext_zicbom),
    ISA_EXT_DATA_ENTRY(zicbop, PRIV_VERSION_1_12_0, ext_zicbop),
    ISA_EXT_DATA_ENTRY(zicboz, PRIV_VERSION_1_12_0, ext_zicboz),
    ISA_EXT_DATA_ENTRY(ziccamoa, PRIV_VERSION_1_11_0, has_priv_1_11),
    ... ...
```

### 2. 扩展依赖判断

不同的指令集扩展之间有的会发生冲突，有的之间有依赖性，他们之间一致性需要被满足。这些关系在 [`riscv_cpu_validate_set_extensions`](https://github.com/qemu/qemu/blob/3f044554b94fc0756d5b3cdbf84501e0eea0e629/target/riscv/tcg/tcg-cpu.c#L406) 中被判断。

例如因为 B 扩展被使能，且它有子扩展，所以在 `riscv_cpu_validate_set_extensions` 函数中调用 [riscv_cpu_validate_b](https://github.com/qemu/qemu/blob/3f044554b94fc0756d5b3cdbf84501e0eea0e629/target/riscv/tcg/tcg-cpu.c#L373) 函数，这个函数自动使能 B 扩展的几个子扩展。

```c
static void riscv_cpu_validate_b(RISCVCPU *cpu)
{
const char *warn_msg = "RVB mandates disabled extension %s";

if (!cpu->cfg.ext_zba) {
    if (!cpu_cfg_ext_is_user_set(CPU_CFG_OFFSET(ext_zba))) {
        cpu->cfg.ext_zba = true;
    } else {
        warn_report(warn_msg, "zba");
    }
}
```

例如因为 I 指令集和 E 指令集不能同时被使能，`riscv_cpu_validate_set_extensions` 函数中有如下语句：

```c
if (riscv_has_ext(env, RVI) && riscv_has_ext(env, RVE)) {
    error_setg(errp, "I and E extensions are incompatible");
    return;
}
```

例如因为 zvknc 扩展依赖于 zvkn 和 zvbc 扩展，`riscv_cpu_validate_set_extensions` 函数中有如下语句：

```c
if (cpu->cfg.ext_zvknc) {
    cpu_cfg_ext_auto_update(cpu, CPU_CFG_OFFSET(ext_zvkn), true);
    cpu_cfg_ext_auto_update(cpu, CPU_CFG_OFFSET(ext_zvbc), true);
}
```

如果某些型号的 CPU 默认启用了某些扩展，那么也需要在这些 CPU 的初始化函数中进行对应的初始化。以 C906 的初始化函数 `rv64_thead_c906_cpu_init` 为例，它默认启用了 misa 寄存器中标识的 C、S、U 标识位，以及字符 G 对应的 IMAFD_Zicsr_Zifencei 扩展组合在 misa 中的部分（IMAFD），此外还手动设置了若干个多字母扩展在 cpu->cfg 中标识是否启用的变量。

```c
static void rv64_thead_c906_cpu_init(Object *obj)
{
    CPURISCVState *env = &RISCV_CPU(obj)->env;
    RISCVCPU *cpu = RISCV_CPU(obj);

    riscv_cpu_set_misa_ext(env, RVG | RVC | RVS | RVU);
    env->priv_ver = PRIV_VERSION_1_11_0;

    cpu->cfg.ext_zfa = true;
    cpu->cfg.ext_zfh = true;
    cpu->cfg.mmu = true;
    cpu->cfg.ext_xtheadba = true;
    cpu->cfg.ext_xtheadbb = true;
    cpu->cfg.ext_xtheadbs = true;
	... ...
}
```

## 二、添加指令实现

qemu 的 TCG 通过把目标平台的指令动态翻译为 TCG 中间指令来实现对指令功能的模拟，RISC-V 指令翻译相关的文件位于 [target/riscv](https://github.com/qemu/qemu/tree/3f044554b94fc0756d5b3cdbf84501e0eea0e629/target/riscv)。

不同扩展中指令的翻译函数分布在不同的文件中，这些文件的路径写在[target/riscv/translate.c#L1090](https://github.com/qemu/qemu/blob/3f044554b94fc0756d5b3cdbf84501e0eea0e629/target/riscv/translate.c#L1090)

```c
/* Include insn module translation function */
#include "insn_trans/trans_rvi.c.inc"
#include "insn_trans/trans_rvm.c.inc"
#include "insn_trans/trans_rva.c.inc"
#include "insn_trans/trans_rvf.c.inc"
#include "insn_trans/trans_rvd.c.inc"
#include "insn_trans/trans_rvh.c.inc"
... ...
```

一条名为`***`的指令对应的翻译函数为`trans_***`，位于上面提到的文件中。例如 qemu 执行过程中遇到 b 扩展中的 bclr 指令时，就会转去执行 `target/riscv/insn_trans/trans_rvb.c.inc` 文件中的 `trans_bclr`指令翻译函数。

`trans_***` 翻译函数中，需要先判断指令属于哪一个扩展，判断对 32/64 位运行环境的要求，再用 tcg 内置函数翻译实现指令的具体功能。例如 P 扩展中的 `add64` 指令属于 `Zpsfoperand` 子扩展且只能在 32 位环境下执行，其翻译函数如下所示，函数体的前两行判断环境是否满足要求，其余为指令功能的实现。

```c
static bool trans_add64(DisasContext *ctx, arg_add64 *a)
{
    REQUIRE_32BIT(ctx);
    REQUIRE_ZPSFOPERAND(ctx);
    TCGv_i64 rs1_64 = tcg_temp_new_i64();
    TCGv_i64 rs2_64 = tcg_temp_new_i64();
    get_pair_regs(ctx, rs1_64, a->rs1);
    get_pair_regs(ctx, rs2_64, a->rs2);
    tcg_gen_add_i64(rs1_64, rs1_64, rs2_64);
    set_pair_regs(ctx, a->rd, rs1_64);
    return true;
}
```

实现指令功能用到的大部分 qemu 翻译函数位于[include/tcg/tcg-op.h](https://github.com/qemu/qemu/blob/3f044554b94fc0756d5b3cdbf84501e0eea0e629/include/tcg/tcg-op.h)，使用方法见 [qemu wiki ](https://www.qemu.org/docs/master/devel/tcg-ops.html)和其它已有指令集的实现。如果 qemu 内置的翻译函数不便于实现指令功能，qemu 还推荐使用 [helper](https://www.qemu.org/docs/master/devel/tcg-ops.html#helpers) 的方式，借助 c 语言实现翻译函数。

用到的翻译 helper 函数需要声明在 [target/riscv/helper.h](https://github.com/qemu/qemu/blob/master/target/riscv/helper.h) 中。

```c
/* Floating Point - fused */
DEF_HELPER_FLAGS_4(fmadd_s, TCG_CALL_NO_RWG, i64, env, i64, i64, i64)
DEF_HELPER_FLAGS_4(fmadd_d, TCG_CALL_NO_RWG, i64, env, i64, i64, i64)
DEF_HELPER_FLAGS_4(fmadd_h, TCG_CALL_NO_RWG, i64, env, i64, i64, i64)
DEF_HELPER_FLAGS_4(fmsub_s, TCG_CALL_NO_RWG, i64, env, i64, i64, i64)
```

翻译 helper 函数的定义所在的文件需要被添加到 [target/riscv/meson.build](https://github.com/qemu/qemu/blob/3f044554b94fc0756d5b3cdbf84501e0eea0e629/target/riscv/meson.build#L11) 中。

```c
riscv_ss.add(files(
  'cpu.c',
  'cpu_helper.c',
  'csr.c',
  'fpu_helper.c',
  ... ...
  'zce_helper.c',
  'vcrypto_helper.c'
))
```

具体使用方式可参考已有指令集的实现。