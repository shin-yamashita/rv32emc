//
// rv_core
// 
//

`timescale 1ns/1ns
`include "rv_types.svh"

import  pkg_rv_decode::*;

module rv_core #(parameter Nregs = 16,
                 parameter debug = 0  ) (
  input  logic clk,
  input  logic xreset,

  output u32_t i_adr,	// insn addr
  input  u32_t i_dr,	// insn read data
  output logic i_re,	// insn read enable
  input  logic i_rdy,	// insn data ready

  output u32_t d_adr,	// mem addr
  input  u32_t d_dr,	// mem read data
  output logic d_re,	// mem read enable
  output u32_t d_dw,	// mem write data
  output u4_t  d_we,	// mem write enable
  input  logic d_rdy,	// mem data ready

  output u32_t imm
  );


//---- branch destination calc ----
function logic [32:0] bra_dest(logic bra_stall, f_insn_t f_dec, u3_t func3, u32_t rs1, u32_t rs2, u32_t imm, u32_t bdst, u32_t pc, u4_t pcinc);
  logic bra;
  u32_t pc_nxt;

  bra = 1'b0;
  if(bra_stall) begin
    pc_nxt = pc + pcinc;
    return {bra,pc_nxt};
  end
  if(f_dec.pc == BRA) begin
    case(func3)
    3'd0: if(rs1 == rs2) bra = 1'b1;   // beq
    3'd1: if(rs1 != rs2) bra = 1'b1;   // bne
    3'd4: if(s32_t'(rs1) < s32_t'(rs2)) bra = 1'b1;  // blt
    3'd5: if(s32_t'(rs1) >= s32_t'(rs2)) bra = 1'b1; // bge
    3'd6: if(rs1 < rs2)  bra = 1'b1;   // bltu
    3'd7: if(rs1 >= rs2) bra = 1'b1;   // bgeu
    endcase
    if(bra) begin
      pc_nxt = bdst;
    end else begin
      pc_nxt = pc + pcinc;
    end
  end else if(f_dec.pc == JMP) begin
    pc_nxt = f_dec.rrd1 == RS1 ? (rs1 + imm) : bdst;
    bra = 1'b1;
  end else begin
    pc_nxt = pc + pcinc;
  end
  return {bra,pc_nxt};        // bra_stall, bra_dest
endfunction

function logic [32:0] Reg_fwd(u5_t ix, u32_t rd, u32_t mdr, u5_t rwa[3], regs_t rwd[3], logic rwdx[3], u32_t rwdat[3]); // Register read with fowarding
  logic d_stall;
  d_stall = 1'b0;
  if(ix == 5'd0) return 'd0;
  if(rwa[0] == ix) begin
    if(rwd[0] == ALU && rwdx[0]) d_stall = 1'b1;
    else if(rwd[0] == ALU) rd = rwdat[0];
    else if(rwd[0] == MDR) d_stall = 1'b1;
  end else if(rwa[1] == ix) begin
    if(rwd[1] == ALU) rd = rwdat[1];
    else if(rwd[1] == MDR) d_stall = 1'b1;
  end else if(rwa[2] == ix) begin
    if(rwd[2] == ALU) rd = rwdat[2];
    else if(rwd[2] == MDR) rd = mdr;
  end
  return {d_stall,rd};
endfunction

logic rdy, cmpl, mulop;
logic bstall, ds1, ds2; // stall signal

assign rdy = i_rdy & d_rdy;
assign i_re = 1'b1;
assign d_re = 1'b1;

//---- register file ----
  u5_t  ars1, ars2, awd;
  u32_t wd, rs1, rs2;
  logic we;

  rv_regf #(.Nregs(Nregs)) u_rv_regf (
    .clk   (clk),
    .ars1  (ars1),   .ars2  (ars2),  .rs1   (rs1),    .rs2   (rs2),
    .awd   (rwa[2]), .we    (we),    .wd    (wd)
  );

//---- registers ---
  u32_t   IR, ir, irh;	// insn register
  u32_t   pc,  pca, pc1, bdst;
  u32_t   mar, mdr, mdr1, mdw;
  u2_t    mar1[2];
  wmode_t mmd, mmd1[2];	// mem-mode
  regs_t  mwe, mwe1[2];	// mem-we
  u32_t   rrd1, rrd2;	// regs read data
  u5_t    rwa[3];	// regs write adr
  regs_t  rwd[3];	// regs write data mode
  logic   rwdx[3];	// regs write data mode (mul op)
  u32_t   rwdat[3];	// alu out data
  u32_t   rwdat1;	// alu out data
  u32_t   rwdatx;	// alu out data (mul op)
  alu_t   alu;		// alu mode
  logic   bra_stall;	// branch stall
  logic   d_stall;	// data stall
  logic   ex_stall;	// exec stall

//---- fetch ----
  u4_t pcinc, pcinca;
  u32_t ira;
  u16_t i_dr1;
  assign ira = i_dr;
  assign i_adr = pca;

  always_ff @ (posedge clk) begin
    if(!xreset) begin
      ir <= 'd0;
      irh <= 'd0;
      i_dr1 <= 'd0;
    end else begin
      ir <= ira;
      i_dr1 <= i_dr[31:16];
      if(!(ex_stall || d_stall))
        irh <= ir;
    end
  end
  assign IR = (ex_stall || d_stall) ? irh : ir;

//---- decode ----
  logic c_insn;
  f_insn_t f_dec;
  u32_t c_imm, f_imm;
  u32_t exir, eir;
  u3_t func3;
  
  assign c_insn = IR[1:0] != 2'b11;
  assign pcinc = c_insn ? 'd2 : 'd4;
  assign pcinca = ira[1:0] != 2'b11 ? 'd2 : 'd4;
  assign {eir, c_imm} = exp_cinsn(IR[15:0]);
  assign imm  = c_insn ? c_imm : f_imm;
  assign exir = c_insn ? eir : IR;
  assign f_dec = dec_insn(exir);
  assign func3 = exir[14:12];
// : f_dec = '{   type,     ex,    alu,   mode,    mar,    ofs,    mwe,   rrd1,   rrd2,    rwa,    rwd,     pc,  excyc }; // mnemonic
  always_comb
    case (f_dec.itype)
    type_I  : f_imm = u32_t'(signed'(IR[31:20]));
    type_S  : f_imm = u32_t'(signed'({IR[31:25],IR[11:7]}));
    type_SB : f_imm = u32_t'(signed'({IR[31],IR[7],IR[30:25],IR[11:8],1'b0}));
    type_U  : f_imm = {IR[31:12],12'h0};
    type_UJ : f_imm = u32_t'(signed'({IR[31],IR[19:12],IR[20],IR[30:21],1'b0}));
    type_R  : f_imm = 'h00000000;
    type_RF : f_imm = 'h00000000;
    default : f_imm = 'h00000000;
    endcase

  assign ars1 = exir[19:15];
  assign ars2 = exir[24:20];
  assign awd  = exir[10:7];

  u32_t rrd1a, rrd2a, bdsta;
  u32_t rs1f, rs2f;

  assign {ds1,rs1f} = bra_stall ? 'd0 : Reg_fwd(ars1, rs1, mdr, rwa, rwd, rwdx, rwdat);
  assign {ds2,rs2f} = bra_stall ? 'd0 : Reg_fwd(ars2, rs2, mdr, rwa, rwd, rwdx, rwdat);

  assign rrd1a = f_dec.rrd1 == PC ? pc1 :
                (f_dec.rrd1 == X0 ? 'd0 : rs1f);
  assign rrd2a = f_dec.rrd2 == IMM ? imm :
                (f_dec.rrd2 == INC ? pc1 + pcinc :
                (f_dec.rrd2 == SHAMT ? ars2 : rs2f));
  assign bdsta = d_stall ? bdst : pc1 + imm;
  
  assign {bstall,pca} = xreset ? 
          (ds1|ds2|(ex_stall&!cmpl) ? {1'b0,pc} : bra_dest(bra_stall, f_dec, func3, rrd1a, rrd2a, imm, bdsta, pc, pcinca)) : 'd0;

//---- exec ----

  rv_alu u_rv_alu (
    .clk (clk),
    .xreset (xreset),
    .rdy (rdy),
    .alu (alu),
    .rrd1(rrd1),
    .rrd2(rrd2),
    .rwdat(rwdat[0]),	// out
    .rwdatx(rwdatx),	// out
    .cmpl(cmpl),	// out
    .mulop(mulop)	// out
  );

//---- mem ----
// mwe  R_NA, RE, WE
// mmd  SI, HI, QI, SHI, SQI
  function u32_t mem_rdata(u32_t mrd, regs_t mwe, wmode_t mmd, u2_t mar);
    u32_t rd;
    case(mmd)
    HI:  rd = u32_t'(mar[1] ? mrd[31:16] : mrd[15:0]);
    SHI: rd = u32_t'(signed'((mar[1] ? mrd[31:16] : mrd[15:0])));
    QI:
        case(mar)
        2'd0:  rd = u32_t'(mrd[7:0]);
        2'd1:  rd = u32_t'(mrd[15:8]);
        2'd2:  rd = u32_t'(mrd[23:16]);
        2'd3:  rd = u32_t'(mrd[31:24]);
        endcase
    SQI:
        case(mar)
        2'd0:  rd = u32_t'(signed'(mrd[7:0]));
        2'd1:  rd = u32_t'(signed'(mrd[15:8]));
        2'd2:  rd = u32_t'(signed'(mrd[23:16]));
        2'd3:  rd = u32_t'(signed'(mrd[31:24]));
        endcase
    default: rd = mrd;
    endcase
    return rd;
  endfunction
  
  function u32_t mem_wdata(u32_t mdw, regs_t mwe, wmode_t mmd, u2_t mar);
    u32_t wd;
    case(mmd)
    HI:   wd = mar[1] ? {mdw[15:0], 16'd0} : {16'd0, mdw[15:0]};
    SHI:  wd = mar[1] ? {mdw[15:0], 16'd0} : {16'd0, mdw[15:0]};
    QI:   wd = mdw[7:0] << {mar,3'd0};
    SQI:  wd = mdw[7:0] << {mar,3'd0};
    SI:   wd = mdw;
    endcase
    return wd;
  endfunction
  
  function u4_t mem_we(regs_t mwe, wmode_t mmd, u2_t mar);
    u4_t we;
    case(mmd)
    SI:   we = 4'b1111;
    HI:   we = mar[1] ? 4'b1100 : 4'b0011;
    SHI:  we = mar[1] ? 4'b1100 : 4'b0011;
    QI:   we = 4'b0001 << mar;
    SQI:  we = 4'b0001 << mar;
    endcase
    return mwe == WE ? we : 'd0;
  endfunction
  
  assign d_adr = mar;
  assign d_dw = mem_wdata(mdw, mwe, mmd, mar[1:0]);
  assign mdr  = mem_rdata(mdr1, mwe1[1], mmd1[1], mar1[1]);
  assign d_we = mem_we(mwe, mmd, mar[1:0]);

//---- wback ----
  always_comb begin
    case(rwd[2])	// write reg address
    ALU: begin
         we = 1'b1;
         wd = rwdat[2];
         end
    MDR: begin
         we = 1'b1;
         wd = mdr;
         end
    default: begin
         we = 1'b0;
         wd = 'd0;
         end
    endcase
  end

  assign rwdat[1] = rwdx[1] ? rwdatx : rwdat1;
  assign rwdx[0] = mulop;

  always_ff @ (posedge clk) begin
    bra_stall <= bra_stall ? 1'b0 : bstall;
    if(!xreset)
      ex_stall <= 1'b0;     
    else if(cmpl)
      ex_stall <= 1'b0;
    else if(f_dec.excyc > 0)
      ex_stall <= 1'b1;
   
    d_stall <= (ds1 | ds2);
    if(f_dec.excyc == 0 || cmpl)
      pc  <= pca;

    pc1 <= pc;
    rwdat1   <= rwdat[0];
    rwdat[2] <= rwdat[1];
    rwd[1]   <= rwd[0];
    rwd[2]   <= rwd[1];
    rwdx[1]  <= rwdx[0];
    rwdx[2]  <= rwdx[1];
    rwa[1]   <= rwa[0];
    rwa[2]   <= rwa[1];
    mmd1[0] <= mmd;
    mmd1[1] <= mmd1[0];
    mwe1[0] <= mwe;
    mwe1[1] <= mwe1[0];
    mar1[0] <= mar[1:0];
    mar1[1] <= mar1[0];
    mdr1 <= d_dr;

    if(!(bra_stall)) begin
      if(!ex_stall) begin
        rrd1 <= rrd1a;
        rrd2 <= rrd2a;
      end
      bdst <= bdsta;
      
      mar    <= ds1 | ds2 ? -2   : (f_dec.mar == RS1 ? rrd1a + imm : 'd0);
      mdw    <= ds1 | ds2 ? -1   : (f_dec.mwe == WE ? rrd2a : 'd0);
      rwa[0] <= ds1 | ds2 ? R_NA : (f_dec.rwa == RD ? awd : 'd0);
      mmd    <= ds1 | ds2 ? SI   : f_dec.mode;
      mwe    <= ds1 | ds2 ? R_NA : f_dec.mwe;
      rwd[0] <= ds1 | ds2 ? R_NA : f_dec.rwd;
      alu    <= ds1 | ds2 ? A_NA : f_dec.alu;
    end
  end

// synthesis translate_off

  function void debug_print(u32_t pc, u32_t ir, u32_t mar, u32_t mdr, u32_t rrd1, u32_t rrd2, u32_t rwdat);
    if(ir[1:0] == 2'd3)
      $display("%8h %8h  %8h %8h %8h %8h %8h", pc, ir, mar, mdr, rrd1, rrd2, rwdat);
    else
      $display("%8h     %4h  %8h %8h %8h %8h %8h", pc, ir[15:0], mar, mdr, rrd1, rrd2, rwdat);
  endfunction
  always@(posedge clk) begin
    if(debug) debug_print(pc1, IR, mar, mdr, rrd1, rrd2, rwdat[1]);
  end

// synthesis translate_on

endmodule

