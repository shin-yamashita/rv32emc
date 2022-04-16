#!/bin/bash

source xilinx_env.sh

vivado -mode tcl << EOF

open_checkpoint rev/post_route.dcp
start_gui
report_timing_summary -delay_type min_max -report_unconstrained -check_timing_verbose -max_paths 10 -nworst 2 -input_pins -name timing_1

EOF


