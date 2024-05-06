# xdc3.log

# create_clock -name clk -period 10.000 [get_ports clk]; # primary clock   already exists

## ??? ## rename cclk <= clk48_clk_gen , when read_xdc cclk error -> then impl cclk OK
create_generated_clock -name cclk -source [get_pins u_clk_gen/inst/plle2_adv_inst/CLKIN1] -master_clock [get_clocks clk] [get_pins u_clk_gen/inst/plle2_adv_inst/CLKOUT2] ;  # rename from clk48_clk_gen

set_input_delay  -clock [get_clocks cclk] -min -add_delay 5.000 [get_ports rxd]
set_input_delay  -clock [get_clocks cclk] -max -add_delay 5.000 [get_ports rxd]
set_output_delay -clock [get_clocks cclk] -min -add_delay 0.000 [get_ports txd]
set_output_delay -clock [get_clocks cclk] -max -add_delay 0.000 [get_ports txd]

set_input_delay  -clock [get_clocks cclk] -min -add_delay 5.000 [get_ports pin[*]]
set_input_delay  -clock [get_clocks cclk] -max -add_delay 5.000 [get_ports pin[*]]
set_output_delay -clock [get_clocks cclk] -min -add_delay 0.000 [get_ports pout[*]]
set_output_delay -clock [get_clocks cclk] -max -add_delay 0.000 [get_ports pout[*]]

set_output_delay -clock [get_clocks cclk] -min -add_delay 0.000 [get_ports led[*]]
set_output_delay -clock [get_clocks cclk] -max -add_delay 0.000 [get_ports led[*]]

set_multicycle_path -through [get_cells -hierarchical umul* ] -setup 2 -start
set_multicycle_path -through [get_cells -hierarchical umul* ] -hold 1 -start
set_multicycle_path -through [get_cells -hierarchical sumul* ] -setup 2 -start
set_multicycle_path -through [get_cells -hierarchical sumul* ] -hold 1 -start

set_false_path -from [get_cells {u_rv_adcif/adcdata_reg[*][*]}] -to [get_cells {u_rv_adcif/adcdata_cs_reg[*][*]}]
set_false_path -from [get_cells {u_rv_adcif/dvalid_reg}] -to [get_cells {u_rv_adcif/dvalid_cs_reg}]

set_false_path -from  [get_clocks cclk] -to [get_clocks clk_pll_i]
set_false_path -from  [get_clocks clk_pll_i] -to [get_clocks cclk]


