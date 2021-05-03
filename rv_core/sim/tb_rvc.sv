
//
// 2019/11/08 200MHz MET

`timescale 1ns/1ns
`include "rv_types.svh"

parameter SYS_exit = 93;
parameter debug = 0;

module tb_rvc (
  input  u8_t  pin,
  output u8_t  pout
  );

 logic clk = 1;
 logic xreset;

 always #5       // 100MHz
        clk <= !clk;

 initial begin
   xreset = 1'b0;
   #50
   @(posedge clk)
   xreset = 1'b1;

 end

 u32_t ir, rwdat;

 rvc #(.debug(debug)) u_rvc (.clk, .xreset, .pin, .pout);

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



