

create_generated_clock -name cclk -source [get_pins u_clk_gen/inst/plle2_adv_inst/CLKIN1] -master_clock [get_clocks clk] [get_pins u_clk_gen/inst/plle2_adv_inst/CLKOUT1]; # 60MHz
#create_generated_clock -name cclk -source [get_pins u_clk_gen/inst/plle2_adv_inst/CLKIN1] -master_clock [get_clocks clk] [get_pins u_clk_gen/inst/plle2_adv_inst/CLKOUT2]; # 48MHz


set_input_delay -clock [get_clocks cclk] -min -add_delay 5.000 [get_ports rxd]
set_input_delay -clock [get_clocks cclk] -max -add_delay 5.000 [get_ports rxd]
set_output_delay -clock [get_clocks cclk] -min -add_delay 0.000 [get_ports txd]
set_output_delay -clock [get_clocks cclk] -max -add_delay 0.000 [get_ports txd]

set_input_delay -clock [get_clocks cclk] -min -add_delay 5.000 [get_ports pin[*]]
set_input_delay -clock [get_clocks cclk] -max -add_delay 5.000 [get_ports pin[*]]
set_output_delay -clock [get_clocks cclk] -min -add_delay 0.000 [get_ports pout[*]]
set_output_delay -clock [get_clocks cclk] -max -add_delay 0.000 [get_ports pout[*]]

set_output_delay -clock [get_clocks cclk] -min -add_delay 0.000 [get_ports led[*]]
set_output_delay -clock [get_clocks cclk] -max -add_delay 0.000 [get_ports led[*]]


set_false_path -from [get_cells {u_rv_adcif/adcdata_reg[*][*]}] -to [get_cells {u_rv_adcif/adcdata_cs_reg[*][*]}]
set_false_path -from [get_cells {u_rv_adcif/dvalid_reg}] -to [get_cells {u_rv_adcif/dvalid_cs_reg}]
