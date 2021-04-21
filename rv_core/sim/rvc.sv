
//
// 2021/4

`timescale 1ns/1ns
`include "rv_types.svh"

module rvc #( parameter debug = 0 ) (
  input  logic clk,
  input  logic xreset,
  input  u8_t  pin,
  output u8_t  pout
  );


 u32_t i_adr;   // insn addr
 u32_t i_dr;    // insn read data
 logic i_re;    // insn read enable
 logic i_rdy;   // insn data ready

 u32_t d_adr;   // mem addr
 u32_t d_dr;    // mem read data
 u4_t  d_we;    // mem write enable
 u32_t d_dw;    // mem write data
 logic d_re;    // mem read enable
 logic d_rdy;   // mem data ready

 u32_t imm;
 u32_t d_dr1;
 logic pin_en;

 assign i_rdy = 1'b1;
 assign d_rdy = 1'b1;

 assign d_dr = pin_en ? u32_t'(pin) : d_dr1;

// synthesis translate_off
  integer STDERR;
  initial begin
    STDERR = $fopen("stderr.out", "w");
  end
  
// synthesis translate_on


  always @(posedge clk) begin
    if(d_we[0] && d_adr == 32'hffff0000)
      pout <= d_dw[7:0];
// synthesis translate_off
    else if(d_we[0] && d_adr == 32'hffff0004)
      $fwrite(STDERR, "%c", d_dw[7:0]);
//      $write("%c", d_dw[7:0]);
// synthesis translate_on

    if(d_re && d_adr == 32'hffff0000)
      pin_en <= 1'b1;
    else
      pin_en <= 1'b0;
  end

  rv_core #(.Nregs(16), .debug(debug)) u_rv_core (
    .*
  );

  dpram #(.ADDR_WIDTH(13), .init_file_u("prog_u.mem"), .init_file_l("prog_l.mem")) u_dpram (
       .clk  (clk),

       .enaA (1'b1),      // read port
       .addrA(i_adr[14:1]),	// half word address
       .doutA(i_dr),
       
       .enaB (1'b1),      // read write port
       .weB  (d_we),
       .addrB(d_adr[14:2]),
       .dinB (d_dw),
       .doutB(d_dr1)
       );

endmodule



