

read_verilog -library xil_defaultlib -sv {
  ../../rv_core/hdl/rv_types.svh
  ../../rv_core/hdl/pkg_rv_decode.sv
  ../../rv_core/hdl/rv_alu.sv
  ../../rv_core/hdl/rv_fpu.sv
  ../../rv_core/hdl/rv_muldiv.sv
  ../../rv_core/hdl/rv_regf.sv
  ../../rv_core/hdl/rv_core.sv
  ../../rv_core/hdl/dpram.sv
  ../hdl/rvc.sv
  ../hdl/adrtag.sv
  ../hdl/rv_cache.sv
  ../hdl/rv_cache_unit.sv
  ../../rv_io/rv_sio.sv
  ../../rv_io/rv_xadcif.sv
  ../../rv_io/rv_pwm.sv
  ../../rv_io/rv_spi.sv
}

read_verilog -library xil_defaultlib {
  ../../rv_core/hdl/dpram_h.v
  ../hdl/dpram10m.v
  ../hdl/mem_if.v
}

set_property file_type "Verilog Header" [get_files ../../rv_core/hdl/rv_types.svh]

read_ip { ../ip/clk_gen.xcix ../ip/xadcif.xcix ../ip/axi_ic.xcix ../ip/ddr3memc/ddr3memc.xci}

generate_target {Synthesis} [get_ips  clk_gen ]
generate_target {Synthesis} [get_ips  xadcif ]
generate_target {Synthesis} [get_ips  axi_ic ]
generate_target {Synthesis} [get_ips  ddr3mem ]