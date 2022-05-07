

read_verilog -library xil_defaultlib -sv {
  ../hdl/rv_types.svh
  ../hdl/pkg_rv_decode.sv
  ../hdl/rv_alu.sv
  ../hdl/rv_muldiv.sv
  ../hdl/rv_regf.sv
  ../hdl/rv_core.sv
  ../hdl/dpram.sv
  ./rvc.sv
  ../../rv_io/rv_sio.sv
  ../../rv_io/rv_pwm.sv
  ../../rv_io/rv_xadcif.sv
}

read_verilog -library xil_defaultlib {
  ../hdl/dpram_h.v
}

set_property file_type "Verilog Header" [get_files ../hdl/rv_types.svh]

read_ip { ../ip/clk_gen.xcix ../ip/xadcif.xcix }
generate_target {Synthesis} [get_ips  {clk_gen xadcif} ]

