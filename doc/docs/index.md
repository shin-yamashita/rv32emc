
# はじめに

組み込みに向けた、コンパクトな RISC-V CPU コアの実装例である。  
FPGAへの実装を行い、[5th AI Edge Contest](https://signate.jp/competitions/537) の課題に適用した。  

## 概要

- RISC-V 命令セットの rv32emc に準拠  
    - E : 16 x 32bit 汎用 registers  
    - M : ハードウェア乗除算器  
    - C : 命令メモリサイズ削減のため、compressed 命令対応  
- RTL (System Verilog) による実装  
- 5段パイプライン  
- 命令、データ 分離バス 32bit address range  
- 割り込み（１系統）
- 64bit タイマー (mtime)  


## 参照規格

- [The RISC-V Instruction Set Manual Volume I: Unprivileged ISA](https://github.com/riscv/riscv-isa-manual/releases/download/Ratified-IMAFDQC/riscv-spec-20191213.pdf)  
- [The RISC-V Instruction Set Manual Volume II: Privileged Architecture](https://github.com/riscv/riscv-isa-manual/releases/download/Priv-v1.12/riscv-privileged-20211203.pdf)  

## 公開ソース

- [rv32emc(github)](https://github.com/shin-yamashita/rv32emc)  

```
rv32emc
├── rv-test/    test programs for logic simulation
├── rv_core/    rv32emc core RTL sources
│   ├── hdl/    RTL (SystemVerilog)
│   ├── sim/    logic simulation
│   └── syn/    FPGA synthesis / example design for Arty-A7
├── rv_io/      serial I/O source
├── rvmon/      Monitor program for FPGA debug
│   ├── lib/    mini library (printf etc.)
│   └── app/    test applications for FPGA 
└── rvsim/      ISS sources
```

- [5th-AI-Edge-Contest(github)](https://github.com/shin-yamashita/5th-AI-Edge-Contest)  
