#!/bin/bash

source xilinx_env.sh

elf=prog.elf

if [ $# -ge 1 ]; then
  elf=$1
fi

updatemem -meminfo prog.mmi -data $elf -bit rev/rvc.bit -proc u_dpram -force -out rvcchg.bit

