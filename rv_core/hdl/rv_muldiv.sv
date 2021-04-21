//
// rv_muldiv.sv
//
// alu muliply / divide unit
//
// 2021/4
//

`include "rv_types.svh"
import  pkg_rv_decode::*;

module rv_muldiv (
  input  logic clk,
  input  logic xreset,
  input  logic rdy,
  input  alu_t alu,
  input  u32_t rrd1,
  input  u32_t rrd2,
  output u32_t rwdat,
  output logic cmpl
  );

  typedef struct {
    u32_t A, B;
    u64_t R;
  } div_t;

  logic sgn, msgn;
  div_t Q;

  u64_t umul;
  s64_t smul, sumul;

  always_comb begin
    umul  <= rrd1 * rrd2;	// u32*u32-> u64
    smul  <= 64'(signed'(rrd1) * signed'(rrd2));	// s32*s32-> s64
    sumul <= 64'(signed'(rrd1) * signed'({1'b0,rrd2}));	// s32*u32-> s64

    case(alu)	// rrd1 op rrd2
    MUL:    rwdat = umul[31:0];	// u32*u32 & 0xffffffff
    MULH:   rwdat = smul[63:32];	// s32*s32 >> 32
    MULHSU: rwdat = sumul[63:32];	// s32*u32 >> 32
    MULHU:  rwdat = umul[63:32];	// u32*u32 >> 32

    DIV:    rwdat = sgn ? 32'd0 - Q.A : Q.A;	// rrd1 / rrd2
    DIVU:   rwdat = Q.A;
    REM:    rwdat = msgn ? 2'd0 - Q.B : Q.B;	// rrd1 % rrd2
    REMU:   rwdat = Q.B;
    default: rwdat = 'd0;
    //    printf("ill ALU operation %d.\n", alu);	
    endcase
  end


  // 2bit divide
  function div_t div_sub(div_t q);
    for(int i = 0; i < 2; i++) begin
      q.R = {1'b0, q.R[63:1]};
      if((q.R[63:32] == 'd0) && (q.B >= q.R[31:0])) begin
        q.B = q.B - q.R[31:0];
        q.A = {q.A[30:0], 1'b1};
      end else begin
        q.A = {q.A[30:0], 1'b0};
      end
    end
    return q;
  endfunction

  typedef enum logic {Idle, Calc} state_t;
  state_t st;
  u5_t exc;

  always_ff@(posedge clk) begin
    if(!xreset) begin
      st <= Idle;
      exc <= 'd0;
      cmpl <= 1'b0;
    end else begin
      if(rdy && st == Idle) begin
        exc <= 'd16;
        cmpl <= 1'b0;
        case (alu)
        DIV,REM : begin
            sgn <= rrd1[31] ^ rrd2[31];
            msgn <= rrd1[31];
            Q.R <= {32'((rrd2[31] ? (32'd0 - rrd2) : rrd2)), 32'd0};
            Q.B <= rrd1[31] ? (32'd0 - rrd1) : rrd1;
            Q.A <= 'd0;
            st <= Calc;
          end
        DIVU,REMU : begin
            sgn <= 1'b0;
            msgn <= 1'b0;
            Q.R <= {rrd2, 32'd0};
            Q.B <= rrd1;
            Q.A <= 'd0;
            st <= Calc;
          end
        default: begin
            st <= Idle;
          end
        endcase
      end else if(st == Calc) begin
        Q <= div_sub(Q);
        exc <= exc - 'd1;
        if(exc == 0) begin
          st <= Idle;
        end
        cmpl <= exc == 2 ? 1'b1 : 1'b0;
      end
    end
  end

endmodule


