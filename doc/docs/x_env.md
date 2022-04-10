
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

$ ./configure --prefix=/opt/rv32e --disable-linux --with-arch=rv32emac --with-abi=ilp32e  
$ make newlib  
```

インストールされるツール、ライブラリ  
```
/opt/rv32e/bin/riscv32-unknown-elf-*                          # コンパイラ、リンカ、gdb etc
/opt/rv32e/riscv32-unknown-elf/include/*.h                    # newlib 標準ヘッダ
/opt/rv32e/riscv32-unknown-elf/lib/libc.a libm.a ...          # newlib 標準ライブラリ
/opt/rv32e/riscv32-unknown-elf/lib/ldscripts/elf32lriscv.x*   # リンカスクリプト
/opt/rv32e/lib/gcc/riscv32-unknown-elf/9.2.0/libgcc.a         # gcc 下位関数(soft-float etc)
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







