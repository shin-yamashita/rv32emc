
//
// 2019/11/08 200MHz MET

`timescale 1ns/1ns
`include "rv_types.svh"

parameter SYS_exit = 93;

module tb_rvc #(parameter debug = 1) (
  input  u8_t  pin,
  output u8_t  pout
  );

 logic clk = 1;
 logic xreset;
 logic rxd, txd;

 always #5       // 100MHz
        clk <= !clk;

 initial begin
   xreset = 1'b0;
   #50
   @(posedge clk)
   xreset = 1'b1;

 end

 u32_t ir, rwdat;

  // ddr3 Inouts
  wire [15:0]   ddr3_dq;
  wire [1:0]    ddr3_dqs_n;
  wire [1:0]    ddr3_dqs_p;
  // Outputs
  wire [13:0]   ddr3_addr;
  wire [2:0]    ddr3_ba;
  wire          ddr3_reset_n;
  wire          ddr3_ras_n;
  wire          ddr3_cas_n;
  wire          ddr3_we_n;
  wire          ddr3_ck_p;
  wire          ddr3_ck_n;
  wire          ddr3_cke;
  wire          ddr3_cs_n;
  wire [1:0]    ddr3_dm;
  wire          ddr3_odt;

  // xadc ports
  wire vauxp1;
  wire vauxn1;
  wire vauxp2;
  wire vauxn2;
  wire vauxp4;
  wire vauxn4;
  wire vauxp5;
  wire vauxn5;
  wire vauxp9;
  wire vauxn9;
  wire vauxp10;
  wire vauxn10;

u12_t led;

 rvc #(.debug(debug)) u_rvc (.clk, .pin, .pout, .rxd, .txd, .* );


 ddr3_model u_ddr3_model (
    .rst_n  (ddr3_reset_n),
    .ck     (ddr3_ck_p),
    .ck_n   (ddr3_ck_n),
    .cke    (ddr3_cke),
    .cs_n   (ddr3_cs_n),
    .ras_n  (ddr3_ras_n),
    .cas_n  (ddr3_cas_n),
    .we_n   (ddr3_we_n),
    .dm_tdqs (ddr3_dm),
    .ba     (ddr3_ba),
    .addr   (ddr3_addr),
    .dq     (ddr3_dq),
    .dqs    (ddr3_dqs_p),
    .dqs_n  (ddr3_dqs_n),
    .tdqs_n (),
    .odt    (ddr3_odt)
 );



// bit i_dr_match, d_dr_match;
// assign i_dr_match = u_rvc.i_dr == u_rvc.i_dr_b;
// assign d_dr_match = u_rvc.d_dr1 == u_rvc.d_dr1_b;

 assign ir = u_rvc.u_rv_core.IR;
 assign rwdat = u_rvc.u_rv_core.rwdat[0];

 always@(posedge clk) begin
   if(ir == 32'h00000073 && rwdat == SYS_exit) begin
     $display("*** ecall %d", rwdat);
     # 50 $finish;
   end
 end

endmodule



