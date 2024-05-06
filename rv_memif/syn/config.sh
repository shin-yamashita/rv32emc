#!/bin/bash

source xilinx_env.sh

bitfn=rev/rvc.bit
#bitfn=./rvcchg.bit

#hw_server=e6520         # remote machine name
hw_server=localhost    # local

if [ $# -ge 1 ]; then
  bitfn=$1      # argv[1] : bit file 
fi

echo "**** configure " $bitfn

vivado -mode tcl <<EOF

open_hw_manager
connect_hw_server -url $hw_server:3121 -allow_non_jtag
current_hw_target [get_hw_targets */xilinx_tcf/Digilent/210319B26865A]
set_property PARAM.FREQUENCY 15000000 [get_hw_targets */xilinx_tcf/Digilent/210319B26865A]
open_hw_target
current_hw_device [get_hw_devices xc7a35t_0]
refresh_hw_device -update_hw_probes false [lindex [get_hw_devices xc7a35t_0] 0]
set_property PROBES.FILE {} [get_hw_devices xc7a35t_0]
set_property FULL_PROBES.FILE {} [get_hw_devices xc7a35t_0]
set_property PROGRAM.FILE { $bitfn } [get_hw_devices xc7a35t_0]
program_hw_devices [get_hw_devices xc7a35t_0]
refresh_hw_device [lindex [get_hw_devices xc7a35t_0] 0]

exit

EOF
