
# 開発環境

## Host

### Ubuntu 20.04

- Vivado 2020.2  /opt/Xilinx/Vivado  
  Ultra96-v2 board file : https://github.com/Avnet/bdf/archive/master.zip  
  sudo mv bdf-master/* /opt/Xilinx/Vivado/2020.2/data/boards/board_files/  



## Target

### Ultra96-v2

- PYNQ V2.7  
[Pre-built SD image v2.7 for Ultra96 version 2](https://github.com/Avnet/Ultra96-PYNQ/releases)  
Download : [Ultra96v1-2.7.0.zip](https://github.com/Avnet/Ultra96-PYNQ/releases/download/v2.7.0/Ultra96v1-2.7.0.zip)  
Use [Etcher](https://www.balena.io/etcher/) to image it onto 32GB Micro SD Card  

PYNQ Linux, based on Ubuntu 20.04 pynq ttyPS0  
pynq login: xilinx (automatic login)  
Welcome to PYNQ Linux, based on Ubuntu 20.04 (GNU/Linux 5.4.0-xilinx-v2020.2 aarch64)  


## riscv gcc (rv32emc)

git clone --recursive https://github.com/riscv/riscv-gnu-toolchain  
cd riscv-gnu-toolchain  

sudo apt install gawk texinfo bison flex  

sudo mkdir /opt/rv32e  
sudo chown shin:shin /opt/rv32e  

./configure --prefix=/opt/rv32e --disable-linux --with-arch=rv32emac --with-abi=ilp32e  
make newlib  

/opt/rv32e 


rvsim  

sudo apt install binutils-dev  libncurses-dev  libreadline-dev  

### gcc ver 11.1.0 => NG, ver 9.2.0 is OK





