
//
// 2019/11/08 200MHz MET

`timescale 1ns/1ns

`include "rv_types.svh"
import  pkg_rv_decode::*;

module tb_alu ();

  logic clk = 0;
  logic xreset, rdy, cmpl, mulop;
  logic cmpl1, mulop1;
  u32_t rrd1, rrd2, rwdat, rwdatx, rwref, rwrefx, csr_rd;
  alu_t  alu;
  int exc;

  string cmd;
  bit exop, compare_en;
  logic match, matchx, almost;
  int fdiff;

  int fd, rv;
  int count, noerr, noerrf;
  
  always #5       // 100MHz
    clk <= !clk;

  initial begin
    fd = $fopen("alutest.vec", "r");
    rdy <= 1;
    xreset <= 0;
    # 40
    @(posedge clk);
    #1
    xreset <= 1;
  end

  always @(posedge clk) begin
    #1
    cmpl1 <= cmpl;
    mulop1 <= mulop;
    rwrefx <= rwref;
    if(!xreset) begin
      rrd1 <= 'd0;
      rrd2 <= 'd0;
      exc <= 0;
      exop <= 0;
    end else  begin
      if($feof(fd)) begin
        $display("%4d,%4d / %4d test passed", noerr, noerrf, count);
        # 50 $finish;
      end else if(!exop) begin
        $fgets(cmd, fd);
        rv <= $sscanf(cmd, "%d %d %d %d", alu, rrd1, rrd2, rwref);
        case(alu)
        DIV,DIVU,REM,REMU: begin
          exop <= 1;
          exc <= 16;
        end
        FADD,FSUB: begin
          exop <= 1;
          exc <= 1;
        end
        FDIV: begin
          exop <= 1;
          exc <= 16;
        end
        default: 
          exop <= 0;
        endcase
      end else begin  // exop state
        if(exc) begin
          exc <= exc - 1;
        end else begin
          exop <= 0;
        end
      end
    end
  end

  import "DPI-C"  pure function int alu_check(int alu, int r1, int r2 , int rwd, int refi);

  assign match  = mulop || exop ? 1'bz : rwref == rwdat;  // normal or multi ex cycle op
  assign matchx = mulop1 ? rwrefx == rwdatx : 1'bz;       // mul : +1 latency op

  assign almost = ((match == 1'b1) || (match == 1'b0 && (fdiff > -2 && fdiff < 2))) === 1'b1 ? 1'b1 : 1'bz;
  
  assign compare_en = rv > 0; // vector data exists

alu_t alu1;
u32_t rrd1d, rrd2d;

  always @(posedge clk) begin
    alu1 <= alu;
    rrd1d <= rrd1;
    rrd2d <= rrd2;
    if(!xreset) begin
      count <= 0;
      noerr <= 0;
      noerrf <= 0;
    end else if(compare_en) begin
      count <= count + (exop || mulop ? 0 : 1) + (mulop1 ? 1 : 0);
      noerr <= noerr + (match === 1'b1 ? 1 : 0) + (matchx === 1'b1 ? 1 : 0);
      noerrf <= noerrf + (almost === 1'b1 ? 1 : 0) + (matchx === 1'b1 ? 1 : 0);
      if(!(mulop || exop))
        alu_check(alu, rrd1, rrd2, rwdat, rwref);
      if(mulop1)
        alu_check(alu1, rrd1d, rrd2d, rwdatx, rwrefx);
    end
  end

  rv_alu u_rv_alu (
  .clk    (clk),  //input  logic clk,
  .xreset (xreset),  //input  logic xreset,
  .rdy    (rdy), //input  logic rdy,
  .alu    (alu), //input  alu_t alu,
  .rrd1   (rrd1),  //nput  u32_t rrd1,
  .rrd2   (rrd2),  //nput  u32_t rrd2,
  .csr_rd  (csr_rd), //nput  u32_t csr_rd,
  .rwdat  (rwdat), //utput u32_t rwdat,
  .rwdatx (rwdatx), //utput u32_t rwdatx,
  .cmpl   (cmpl), //utput logic cmpl,
  .mulop  (mulop) //output logic mulop
  );

endmodule


