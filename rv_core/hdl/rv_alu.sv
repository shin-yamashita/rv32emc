//
// rv_alu.sv
//
// alu
// 32 bit x 32 arithmetic logic unit
//
// 2019/11
//

`include "rv_types.svh"
import  pkg_rv_decode::*;

module rv_alu (
  input  logic clk,
  input  logic xreset,
  input  logic rdy,
  input  alu_t alu,
  input  u32_t rrd1,
  input  u32_t rrd2,
  output u32_t rwdat,
  output logic cmpl
  );

  u32_t rwd;
  rv_muldiv u_rv_muldiv(	// multiply / divide unit
    .clk(clk), .xreset(xreset), .rdy(rdy),
    .alu(alu), .rrd1(rrd1), .rrd2(rrd2),
    .rwdat(rwd), .cmpl(cmpl)
  );

  always_comb begin
  case(alu)	// rrd1 op rrd2
  ADD:    rwdat = rrd1 + rrd2;  
  S2:     rwdat = rrd2;	
  SLT:    rwdat = s32_t'(rrd1) < s32_t'(rrd2);	
  SLTU:   rwdat = rrd1 < rrd2;	
  XOR:    rwdat = rrd1 ^ rrd2;	
  OR:     rwdat = rrd1 | rrd2;	
  AND:    rwdat = rrd1 & rrd2;	
  SLL:    rwdat = rrd1 << (rrd2 & 6'h3f);	
  SRL:    rwdat = rrd1 >> (rrd2 & 6'h3f);	
  SRA:    rwdat = s32_t'(rrd1) >>> (rrd2 & 6'h3f);	
  SUB:    rwdat = rrd1 - rrd2;	

  MUL:    rwdat = rwd;
  MULH:   rwdat = rwd;
  MULHSU: rwdat = rwd;
  MULHU:  rwdat = rwd;

  DIV:    rwdat = rwd;
  DIVU:   rwdat = rwd;
  REM:    rwdat = rwd;
  REMU:   rwdat = rwd;
/*
  FLD:
  FST:
  FMADD:
  FMSUB:
  FADD:
  FSUB:
  FMUL:
  FDIV:
  FSQRT:
  FSGN:
  FMIN:
  FMAX:
  FCVT:
--*/
  default: rwdat = 'd0;
    //    printf("ill ALU operation %d.\n", alu);	
  endcase
  end

endmodule


