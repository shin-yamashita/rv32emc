#
# open post_route checkpoint and re-route
#

set proj design_1
set outputDir ./rev

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

open_checkpoint $outputDir/post_route.dcp 

#runPPO 2 1 ; # run 2 post-route iterations and enable phys_opt_design
runPPO 2 0 ; # run 4 post-route iterations and disable phys_opt_design

write_checkpoint -force $outputDir/post_route
report_timing_summary -file $outputDir/post_route_timing_summary.rpt
report_timing -sort_by group -max_paths 100 -path_type summary -file $outputDir/post_route_timing.rpt
report_clock_utilization -file $outputDir/clock_util.rpt
report_utilization -file $outputDir/post_route_util.rpt
report_utilization -hierarchical -file $outputDir/post_route_area.rpt
report_power -file $outputDir/post_route_power.rpt
report_drc -file $outputDir/post_imp_drc.rpt


#write_verilog -force $outputDir/bft_impl_netlist.v
#write_xdc -no_fixed_only -force $outputDir/bft_impl.xdc
#
# STEP#5: generate a bitstream
# 

write_bitstream -force $outputDir/$proj.bit
write_hwdef -force -file $outputDir/$proj.hwdef

exit

