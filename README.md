# rv32emc


## RISC-V の RTL 実装例  

- RISC-V の ISA のうち、組み込み用途向けの EMC (32bit 16 register, Multiply/Divide, Complessed 命令) の構成で CPU core を実装した。
- Fetch/Decode/Exec/MemoryAccess/WriteBack の５段パイプライン構造とした。  
- RTL は SystemVerilog で記述した。  
- C 言語で開発した ISS をリファレンスとして RTL の開発を行った。  
- FPGA で動作を確認した。coremark を実行し、2.4(CMK/MHz) を確認した。  
- 開発した CPU core の応用として、[5th AI Edge Contest](https://signate.jp/competitions/537) の課題に適用した。  

----
## files
```
.
├── rv-test/    test programs
├── rv_core/    rv32emc core RTL sources
│   ├── hdl/    RTL (SystemVerilog)
│   ├── sim/    logic simulation
│   ├── syn/    FPGA synthesis 
├── rv_io/      serial I/O source
├── rvmon/      Monitor program for FPGA debug
└── rvsim/      ISS sources

```