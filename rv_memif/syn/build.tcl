#
# build.tcl
# non-project batch flow
#
# NOTE:  typical usage would be "vivado -mode tcl -source build.tcl" 
#
# STEP#0: define output directory area.
#

proc runPPO { {numIters 1} {enablePhysOpt 1} } {
  for {set i 0} {$i < $numIters} {incr i} {
    place_design -post_place_opt
      if {$enablePhysOpt != 0} {
      phys_opt_design
    }
    route_design
    if {[get_property SLACK [get_timing_paths ]] >= 0} {break}; #stop if timing is met
  }
}

create_project -in_memory -part xc7a35ticsg324-1L
set_property board_part digilentinc.com:arty-a7-35:part0:1.1 [current_project]

set_property source_mgmt_mode All [current_project]

set CWD [pwd]
set proj rvc
set outputDir ./rev
file mkdir $outputDir
#
# STEP#1: setup design sources and constraints
#
source read_hdl.tcl
#source read_ip.tcl

read_xdc timing.xdc
read_xdc arty-a7-pinassign.xdc
#read_xdc dont_touch.xdc

### error? set_property file_type "Verilog Header" [get_files ../hdl/acc/logic_types.svh]

#
# STEP#2: run synthesis, report utilization and timing estimates, write checkpoint design
#
synth_design -top $proj -part xc7a35ticsg324-1L -include_dirs {$CWD/../hdl/}
write_checkpoint -force $outputDir/post_synth
#report_timing_summary -file $outputDir/post_synth_timing_summary.rpt
#report_power -file $outputDir/post_synth_power.rpt
#
# STEP#3: run placement and logic optimzation, report utilization and timing estimates, write checkpoint design
#

set_param drc.disableLUTOverUtilError 1

opt_design
#opt_design -directive ExploreArea
#opt_design -directive ExploreSequentialArea
write_debug_probes -force $outputDir/debug.ltx

place_design
#place_design -directive ExtraNetDelay_high
phys_opt_design

#write_checkpoint -force $outputDir/post_place
#report_timing_summary -file $outputDir/post_place_timing_summary.rpt

#place_design -post_place_opt

#
# STEP#4: run router, report actual utilization and timing, write checkpoint design, run drc, write verilog and xdc out
#
route_design

runPPO 2 1 ; # run 2 post-route iterations and enable phys_opt_design

#place_design -post_place_opt
#phys_opt_design
#route_design

write_checkpoint -force $outputDir/post_route
report_timing_summary -file $outputDir/post_route_timing_summary.rpt
report_timing -sort_by group -max_paths 100 -path_type summary -file $outputDir/post_route_timing.rpt
report_clock_utilization -file $outputDir/clock_util.rpt
report_utilization -file $outputDir/post_route_util.rpt
report_utilization -hierarchical -file $outputDir/post_route_area.rpt
report_power -file $outputDir/post_route_power.rpt
# report_drc -file $outputDir/post_imp_drc.rpt


#write_verilog -force $outputDir/bft_impl_netlist.v
#write_xdc -no_fixed_only -force $outputDir/bft_impl.xdc
#
# STEP#5: generate a bitstream
# 

source write_mmi.tcl

startgroup
set_property CONFIG_VOLTAGE 3.3 [current_design]
set_property CFGBVS VCCO [current_design]
set_property BITSTREAM.CONFIG.CONFIGRATE 22 [current_design]
set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 4 [current_design]
set_property config_mode SPIx4 [current_design]
endgroup

write_bitstream -force $outputDir/$proj.bit
write_cfgmem -format mcs -interface SPIx4 -size 16 -loadbit "up 0 $outputDir/$proj.bit" -force -file $outputDir/$proj.mcs

exit

