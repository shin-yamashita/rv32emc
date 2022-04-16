#!/bin/bash

source xilinx_env.sh

updatemem -meminfo prog.mmi -data prog.elf -bit rev/rvc.bit -proc u_dpram -force -out rvcchg.bit

