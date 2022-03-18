
//
// 2021/4

`timescale 1ns/1ns
`include "rv_types.svh"

module rvc #( parameter debug = 0 ) (
  input  logic clk,
  input  logic xreset,
  input  u8_t  pin,
  output u8_t  pout,
  input  logic rxd,
  output logic txd
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
 logic d_be;	// mem bus big endian

 logic irq;

 u32_t d_dr1, d_dr2;
 logic pin_en;
 logic enaB, re1;

 assign i_rdy = 1'b1;
 assign d_rdy = 1'b1;
 assign d_be = 1'b0;

 assign d_dr = pin_en ? u32_t'(pin) : 
               re1    ? d_dr2 : d_dr1;

 assign enaB = (d_re || (d_we != 'd0)) && d_adr < 32'h10000;

// synthesis translate_off
  integer STDERR;
  initial begin
    STDERR = $fopen("stderr.out", "w");
  end
// synthesis translate_on

  always @(posedge clk) begin
    if(d_we[0] && d_adr == 32'hffff0000)  // 8bit pararell output port
      pout <= d_dw[7:0];
// synthesis translate_off
    else if(d_we[0] && d_adr == 32'hffff0004) // debug _write()
      $fwrite(STDERR, "%c", d_dw[7:0]);
//      $write("%c", d_dw[7:0]);
// synthesis translate_on

    if(d_re && d_adr == 32'hffff0000) // 8bit pararell input port
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
       .addrA(i_adr[14:1]),	// half (16bit) word address
       .doutA(i_dr),
       
       .enaB (enaB),     // read write port
       .weB  (d_we),
       .addrB(d_adr[14:2]), // 32bit word address
       .dinB (d_dw),
       .doutB(d_dr1)
       );

// peripheral

  logic cs, rdy, re, dsr, txen;
  u4_t  we;
  u32_t dw, dr;

// ffff0020  async serial terminal
  assign cs = {d_adr[31:5],5'h0} == 32'hffff0020;
  assign rdy = d_rdy;
  assign dsr = 1'b0;
  assign re = d_re;

  always_ff@(posedge clk) begin
    re1 <= cs & re;
  end

  rv_sio u_rv_sio (
    .clk  (clk),
    .xreset(xreset),
    .adr  (d_adr[4:0]),
    .cs   (cs),   .rdy  (rdy),
    .we   (d_we), .re   (re),   .irq  (irq),
    .dw   (d_dw), .dr   (d_dr2),	
    .txd  (txd),  .rxd  (rxd),  .dsr  (dsr),  .dtr  (dtr),  .txen (txen)
  );


endmodule



