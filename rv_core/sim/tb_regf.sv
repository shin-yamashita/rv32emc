
//
// 2019/11/08 200MHz MET

parameter TLEN = 32;

module tb_regf (
  input logic clk,
  output logic out,
  output logic out1,
  output logic out2
  );

  logic [TLEN-1:0] rs1, rs2, rd, rrs1, rrs2;
  logic [TLEN-1:0] frd1, frd2, frd3;
  logic [TLEN-1:0] pc, ir;
  logic [4:0]  ars1, ars2, ard;
  logic [4:0]  fard1, fard2, fard3; 

  logic [TLEN-1:0] dina, dinb, douta, doutb;
  logic [13:0] addra, addrb;
  logic [3:0] web;

  logic [11:0] imm;

  assign imm = ir[31:20];

  always @(posedge clk) begin
    rd <= rd + 32'd1;
    ars1 <= rd[4:0];
    ars2 <= rd[9:5];
    ard <= rd[14:10];
    out <= frd3 > 3;
    out1 <= douta < 32'd100;
    out2 <= doutb > 32'd100;
    ir <= douta;
    pc <= rs1 == rs2 ? pc + 32'd4 : pc + imm;
  end

  rv_regf u_rv_regf (
    .clk (clk),

    .ars1(ars1),
    .ars2(ars2),
    .ard (ard),

    .rs1 (rs1),
    .rs2 (rs2),
    .rd  (rd),

    .fard1 (fard1),      // forwarding
    .fard2 (fard2),
    .fard3 (fard3),
    .frd1 (frd1),
    .frd2 (frd2),
    .frd3 (frd3)
  );

  always @(posedge clk) begin
    rrs1 <= rs1;
    rrs2 <= rs2;
    frd1 <= rrs1 + rrs2;
    frd2 <= frd1;
    frd3 <= frd2;
    fard1 <= ard;
    fard2 <= fard1;
    fard3 <= fard2;
  end

  assign dinb = frd2;
  assign web = fard1[3:0];
  assign addra = frd3[13:0];
  assign addrb = frd2[13:0];

  dpram #(
    .NUM_COL    (4),
    .COL_WIDTH  (8),
//    .ADDR_WIDTH (14),
    .ADDR_WIDTH (12),
    .DATA_WIDTH (TLEN)
    ) u_dpram (
       .clk   (clk),	//
       .enaA  (1'b1), 	//
       .addrA (addra),	// input [ADDR_WIDTH-1:0] .addrA
       .doutA (douta),	// output .reg [DATA_WIDTH-1:0] .doutA
       
       .enaB  (1'b1),
       .weB   (web),	// input [NUM_COL-1:0] .weB
       .addrB (addrb),	// input [ADDR_WIDTH-1:0] .addrB
       .dinB  (dinb),	// input [DATA_WIDTH-1:0] .dinB
       .doutB (doutb)	// output reg [DATA_WIDTH-1:0] .doutB
    );

endmodule


