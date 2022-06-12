
//
// 2022/04  for Arty-A7 example
//

`timescale 1ns/1ns
`include "rv_types.svh"

module rvc #( parameter debug = 0 ) (
  input  logic clk,
//  input  logic xreset,
  input  u8_t  pin,
  output u8_t  pout,
  input  logic rxd,
  output logic txd,

  input vauxp1, // analog signal connection
  input vauxn1,
  input vauxp2,
  input vauxn2,
  input vauxp4,
  input vauxn4,
  input vauxp5,
  input vauxn5,
  input vauxp9,
  input vauxn9,
  input vauxp10,
  input vauxn10,

  output u12_t led  // LED drive pwm
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

 u32_t d_dr0, d_dr1, d_dr2, d_dr3, d_dr4;
 logic pin_en;
 logic enaB, re1;

 assign i_rdy = 1'b1;
 assign d_rdy = 1'b1;
 assign d_be = 1'b0;

int rst_cnt = 0;
logic xreset, cclk;

u8_t  pin_s;

clk_gen u_clk_gen
   (
    // Clock out ports
    .clk80(clk80),     // output clk80
    .clk60(clk60),     // output clk60
    .clk48(clk48),     // output clk48
    .clk_sys(sys_clk),     // output clk_out4 100M

    // Status and control signals
    .reset(1'b0), // input reset
    .locked(locked),       // output locked
   // Clock in ports
    .clk_in1(clk));      // input clk_in1

  assign cclk = clk60;

// synthesis translate_off
  integer STDERR;
  initial begin
    STDERR = $fopen("stderr.out", "w");
  end
// synthesis translate_on

  always @(posedge cclk) begin
    if(rst_cnt < 1024) begin
        xreset <= 1'b0;
        rst_cnt <= rst_cnt + 1;
    end else begin
        xreset <= 1'b1;
    end

    if(d_we[0] && d_adr == 32'hffff0000)  // 8bit pararell output port
      pout <= d_dw[7:0];
// synthesis translate_off
    else if(d_we[0] && d_adr == 32'hffff0004) // debug _write()
      $fwrite(STDERR, "%c", d_dw[7:0]);
//      $write("%c", d_dw[7:0]);
// synthesis translate_on

    pin_s <= pin; // async
    if(d_re && d_adr == 32'hffff0000) // 8bit pararell input port
      d_dr0 <= u32_t'(pin_s);
    else
      d_dr0 <= 'd0;
  end

  assign d_dr = d_dr0 | d_dr1 | d_dr2 | d_dr3 | d_dr4;

  rv_core #(.Nregs(16), .debug(debug)) u_rv_core (
    .clk  (cclk),     // input  logic clk,
    .xreset(xreset),  // input  logic xreset,

    .i_adr(i_adr),    // output u32_t i_adr,   // insn addr
    .i_dr (i_dr),     // input  u32_t i_dr,    // insn read data
    .i_re (i_re),     // output logic i_re,    // insn read enable
    .i_rdy(i_rdy),    // input  logic i_rdy,   // insn data ready

    .d_adr(d_adr),    // output u32_t d_adr,   // mem addr
    .d_dr (d_dr),     // input  u32_t d_dr,    // mem read data
    .d_re (d_re),     // output logic d_re,    // mem read enable
    .d_dw (d_dw),     // output u32_t d_dw,    // mem write data
    .d_we (d_we),     // output u4_t  d_we,    // mem write enable
    .d_rdy(d_rdy),    // input  logic d_rdy,   // mem data ready
    .d_be (d_be),     // input  logic d_be,    // mem bus big endian

    .irq  (irq)       // input  logic irq  // interrupt request
  );

  // 32bit word dual port RAM 
  localparam RAMSIZE = 128 * 1024; // 128kB 
//  localparam RAMSIZE = 64 * 1024; // 64kB
//  localparam RAMSIZE = 32 * 1024; // 32kB
  localparam Nb = $clog2(RAMSIZE);  //

  assign enaB = (d_re || (d_we != 'd0)) && d_adr < u32_t'(1 << Nb);

  dpram #(.ADDR_WIDTH(Nb-2),
          .init_file_u("prog_u.mem"), // upper 16bit (31:16) initial data
          .init_file_l("prog_l.mem")  // lower 16bit  (15:0) initial data
          ) u_dpram (
    .clk  (cclk),

    .enaA (1'b1),     // read only port
    .addrA(i_adr[Nb-1:1]),  // half (16bit) word address
    .doutA(i_dr),           // 16bit aligned 32bit read data (instruction)

    .enaB (enaB),     // read write port
    .weB  (d_we),           // [3:0] byte write enable
    .addrB(d_adr[Nb-1:2]),  // 32bit word address
    .dinB (d_dw),           // 32bit write data
    .doutB(d_dr1)           // 32bit read data
  );

// peripheral

  logic cs_sio, dsr, txen;
// ffff0020  async serial terminal
  assign cs_sio = {d_adr[31:5],5'h0} == 32'hffff0020;
  assign dsr = 1'b0;

  always_ff@(posedge cclk) begin
    re1 <= cs_sio & d_re;
  end

  rv_sio u_rv_sio (
    .clk  (cclk),
    .xreset(xreset),
    .adr  (d_adr[4:0]),
    .cs   (cs_sio), .rdy  (d_rdy),
    .we   (d_we),   .re   (d_re),   .irq  (irq),
    .dw   (d_dw),   .dr   (d_dr2),
    .txd  (txd),    .rxd  (rxd),    .dsr  (dsr),  .dtr  (dtr),  .txen (txen)
  );

  logic cs_adcif;
  u12_t device_temp;
  logic adclk, ui_clk;
// ffff0040 XADC 
  assign cs_adcif = {d_adr[31:5],5'h0} == 32'hffff0040;
  assign adclk = sys_clk;
  assign ui_clk = clk80;

  rv_adcif u_rv_adcif(
      .clk    (cclk),   // cpu bus clock
      .xreset (xreset),
      // bus
      .adr    (d_adr[4:0]),
      .cs     (cs_adcif), .rdy  (d_rdy),
      .we     (d_we),     .re   (d_re),
      .dw     (d_dw),     .dr   (d_dr3),
      // XADC clock
      .adclk  (adclk),     // XADC dclk 100MHz
      .vauxp1 (vauxp1),      .vauxn1 (vauxn1),      .vauxp2 (vauxp2),      .vauxn2 (vauxn2),
      .vauxp4 (vauxp4),      .vauxn4 (vauxn4),      .vauxp5 (vauxp5),      .vauxn5 (vauxn5),
      .vauxp9 (vauxp9),      .vauxn9 (vauxn9),      .vauxp10(vauxp10),      .vauxn10(vauxn10),
      // MIG 
      .ui_clk (ui_clk),    // MIG ui_clk
      .device_temp(device_temp)  // temperature data for MIG
    );

  logic cs_pwm;
  // ffff0060 PWM for RGB LED
  assign cs_pwm = {d_adr[31:5],5'h0} == 32'hffff0060;

  rv_pwm u_rv_pwm(
      .clk    (cclk),   // cpu bus clock
      .xreset (xreset),
      // bus
      .adr    (d_adr[4:0]),
      .cs     (cs_pwm),   .rdy  (d_rdy),
      .we     (d_we),     .re   (d_re),
      .dw     (d_dw),     .dr   (d_dr4),

      // LED pwm out
      .led    (led) // u12_t
    );

endmodule



