#!/bin/bash
#
# program spi flush
#

source xilinx_env.sh

bitfn=rev/rvc.bit       # .bit file name to program
#bitfn=./rvcchg.bit

hw_server=e6520         # remote machine name
#hw_server=localhost    # local
hw_target=*/xilinx_tcf/Digilent/210319B26865A   # Arty-A7 board の USB JTAG

# Arty-A7 spi flash memory : FL128SAIF00 spansion 
spiflash=s25fl128sxxxxxx0-spi-x1_x2_x4

if [ $# -ge 1 ]; then
  bitfn=$1      # argv[1] : bit file name
fi

echo "**** program " $bitfn

# 1. convert bit file to mcs file 'prog.mcs' 
# 2. erase/program/verify flash 
# 3. reboot FPGA

vivado -mode tcl <<EOF

write_cfgmem -format mcs -interface SPIx4 -size 16 -loadbit "up 0 $bitfn" -force -file prog.mcs

open_hw_manager
connect_hw_server -url $hw_server:3121 -allow_non_jtag
current_hw_target [get_hw_targets $hw_target]
set_property PARAM.FREQUENCY 15000000 [get_hw_targets $hw_target]
open_hw_target
current_hw_device [get_hw_devices xc7a35t_0]
refresh_hw_device -update_hw_probes false [lindex [get_hw_devices xc7a35t_0] 0]
create_hw_cfgmem -hw_device [lindex [get_hw_devices xc7a35t_0] 0] [lindex [get_cfgmem_parts {$spiflash}] 0]
set_property PROGRAM.BLANK_CHECK  0 [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a35t_0] 0]]
set_property PROGRAM.ERASE  1 [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a35t_0] 0]]
set_property PROGRAM.CFG_PROGRAM  1 [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a35t_0] 0]]
set_property PROGRAM.VERIFY  1 [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a35t_0] 0]]
set_property PROGRAM.CHECKSUM  0 [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a35t_0] 0]]
refresh_hw_device [lindex [get_hw_devices xc7a35t_0] 0]
set_property PROGRAM.ADDRESS_RANGE  {use_file} [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a35t_0] 0]]
set_property PROGRAM.FILES [list "prog.mcs" ] [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a35t_0] 0]]
set_property PROGRAM.PRM_FILE {prog.prm} [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a35t_0] 0]]
set_property PROGRAM.UNUSED_PIN_TERMINATION {pull-none} [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a35t_0] 0]]
set_property PROGRAM.BLANK_CHECK  0 [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a35t_0] 0]]
set_property PROGRAM.ERASE  1 [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a35t_0] 0]]
set_property PROGRAM.CFG_PROGRAM  1 [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a35t_0] 0]]
set_property PROGRAM.VERIFY  1 [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a35t_0] 0]]
set_property PROGRAM.CHECKSUM  0 [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a35t_0] 0]]
startgroup 
create_hw_bitstream -hw_device [lindex [get_hw_devices xc7a35t_0] 0] [get_property PROGRAM.HW_CFGMEM_BITFILE [ lindex [get_hw_devices xc7a35t_0] 0]]; program_hw_devices [lindex [get_hw_devices xc7a35t_0] 0]; refresh_hw_device [lindex [get_hw_devices xc7a35t_0] 0];
program_hw_cfgmem -hw_cfgmem [ get_property PROGRAM.HW_CFGMEM [lindex [get_hw_devices xc7a35t_0] 0]]
endgroup

boot_hw_device [lindex [get_hw_devices xc7a35t_0] 0]

exit

EOF
