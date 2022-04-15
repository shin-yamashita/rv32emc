
# 開発環境

## Host

- Ubuntu 20.04 上で開発を行った

### FPGA 開発ツール

- Vivado/2020.2  
  /opt/Xilinx/Vivado にインストール  

- Vivado_Lab/2020.2  
  /opt/Xilinx/Vivado_Lab にインストール  
  ターゲットボードを USB で接続するマシンが Disk 容量不足で Vivado がインストールできないので、Vivado_Lab をインストールして Host から remote で制御した  

### クロスコンパイラ riscv32 gcc (for rv32emc)

RISC-V の RV32EMC に対応したクロスコンパイラのビルド、インストール  

```bash
$ sudo apt install autoconf automake autotools-dev curl libmpc-dev libmpfr-dev libgmp-dev gawk \
        build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev  
                        # gcc の build で必要となるツール Prerequisites  
$ sudo mkdir /opt/rv32e # cross tool のインストール先  

$ git clone --branch rvv-0.9.x --recursive https://github.com/riscv/riscv-gnu-toolchain  
$ cd riscv-gnu-toolchain  

$ patch -p1 < riscv-gdb-sim.patch   # run の修正 patch

$ ./configure --prefix=/opt/rv32e --disable-linux --with-arch=rv32emac --with-abi=ilp32e  
$ make newlib   # ツール群を /opt/rv32e/ にインストール、書き込み権限が必要
```

Download: [riscv-gdb-sim.patch](refdoc/riscv-gdb-sim.patch)  

??? "gdb のシミュレータ run の修正 patch"
    cross gdb のインストール時、シミュレータ `run` がインストールされる。  
    `run` の syscall は RV32E に対応しておらず、write(), exit() などが実行されない。  
    newlib では syscall は以下のファイルで定義されており、RV32E のみ syscall_id を渡すレジスタが異なる。  
    `riscv-newlib/libgloss/riscv/internal_syscall.h`  
        RV32E の syscall_id = "t0"   
        その他の syscall_id = "a7"  
    このため、run を RV32E の syscall に対応させる patch を用意した。  
    `run` 実行時、 `--model RV32E` option をつけることで RV32E のシミュレーションができる。  


インストールされるツール、ライブラリ  
```
/opt/rv32e/bin/riscv32-unknown-elf-*                          # コンパイラ、リンカ、gdb etc
/opt/rv32e/riscv32-unknown-elf/include/*.h                    # newlib 標準ヘッダ
/opt/rv32e/riscv32-unknown-elf/lib/libc.a libm.a ...          # newlib 標準ライブラリ
/opt/rv32e/riscv32-unknown-elf/lib/ldscripts/elf32lriscv.x*   # リンカスクリプト
/opt/rv32e/lib/gcc/riscv32-unknown-elf/9.2.0/libgcc.a         # gcc 下位関数(soft-float etc)
```

クロスコンパイラで RV32EMC に対応したバイナリを生成するには、コンパイル flag `-march=rv32emc -mabi=ilp32e` を与える  

```bash
$ /opt/rv32e/bin/riscv32-unknown-elf-gcc -g -Wall -O2 -march=rv32emc -mabi=ilp32e -c c_source.c  
```


!!! Note
    **rv_core は gcc ver 9.2.0 で動作確認を行った**  
    gcc ver 11.1.0 では動作不具合が発生したが、原因不明  


### rvsim (ISS) 

```bash
$ sudo apt install binutils-dev libreadline8 libtinfo6 libncurses-dev  libreadline-dev  

$ git clone https://github.com/shin-yamashita/rv32emc
$ cd rv32emc/rvsim
$ make
$ sudo make install   # -> /usr/local/bin/rvsim
```

## Target board

### Ultra96-v2

ボード情報: [AVNET Ultra96](https://www.96boards.org/product/ultra96/)  

- PYNQ V2.7  
  [Pre-built SD image v2.7 for Ultra96 version 2](https://github.com/Avnet/Ultra96-PYNQ/releases)  
  Download : [Ultra96v1-2.7.0.zip](https://github.com/Avnet/Ultra96-PYNQ/releases/download/v2.7.0/Ultra96v1-2.7.0.zip)  
  Use [Etcher](https://www.balena.io/etcher/) to image it onto 32GB Micro SD Card  
  ```
  PYNQ Linux, based on Ubuntu 20.04 pynq ttyPS0  
  pynq login: xilinx (automatic login)  
  Welcome to PYNQ Linux, based on Ubuntu 20.04 (GNU/Linux 5.4.0-xilinx-v2020.2 aarch64)  
  ```

### Arty-A7-35T

ボード情報: [Digilent Arty-A7 Reference](https://digilent.com/reference/programmable-logic/arty-a7/start)







