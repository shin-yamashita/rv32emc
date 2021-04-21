
//
// rv_dec_insn.sv
// insn decode table
// genarated by ./insntab.py RV-insn.csv

`include "rv_types.svh"

typedef enum u3_t {
  type_U, type_UJ, type_I, type_SB, type_S, type_R, type_RF
} itype_t;

typedef enum u3_t {
  I, M, F, E, C
} ex_t;

typedef enum u3_t {
  SI, HI, QI, SHI, SQI
} wmode_t;

typedef enum u6_t {
  A_NA, S2, ADD, SLT, SLTU, XOR, OR, AND, SLL, SRL, SRA, SUB,
  MUL, MULH, MULHSU, MULHU, DIV, DIVU, REM, REMU,
  FLD, FST, FMADD, FMSUB, FADD, FSUB, FMUL, FDIV, FSQRT, FSGN, FMIN, FMAX, FCVT
} alu_t;

typedef enum u4_t {
  R_NA, X0, RS1, RS2, RD, IMM, PC, RM, WE, RE, MDR, ALU, INC, JMP, BRA, SHAMT
} regs_t;

typedef struct {
  itype_t itype;
  ex_t    ex;
  alu_t   alu;
  wmode_t mode;
  regs_t  mar;
  regs_t  ofs;
  regs_t  mwe;
  regs_t  rrd1;
  regs_t  rrd2;
  regs_t  rwa;
  regs_t  rwd;
  regs_t  pc;
  u5_t    excyc;
} f_insn_t;

function f_insn_t dec_insn(input u32_t ir);
  f_insn_t f_dec;

  case ({ir[31:25],ir[14:12],ir[6:0]}) inside
                     // : f_dec = '{   type,     ex,    alu,   mode,    mar,    ofs,    mwe,   rrd1,   rrd2,    rwa,    rwd,     pc,  excyc }; // mnemonic
  17'b???????0000000011 : f_dec = '{ type_I,      I,   A_NA,    SQI,    RS1,    IMM,     RE,   R_NA,   R_NA,     RD,    MDR,   R_NA,   5'd0 }; // "lb"
  17'b???????0010000011 : f_dec = '{ type_I,      I,   A_NA,    SHI,    RS1,    IMM,     RE,   R_NA,   R_NA,     RD,    MDR,   R_NA,   5'd0 }; // "lh"
  17'b???????0100000011 : f_dec = '{ type_I,      I,   A_NA,     SI,    RS1,    IMM,     RE,   R_NA,   R_NA,     RD,    MDR,   R_NA,   5'd0 }; // "lw"
  17'b???????1000000011 : f_dec = '{ type_I,      I,   A_NA,     QI,    RS1,    IMM,     RE,   R_NA,   R_NA,     RD,    MDR,   R_NA,   5'd0 }; // "lbu"
  17'b???????1010000011 : f_dec = '{ type_I,      I,   A_NA,     HI,    RS1,    IMM,     RE,   R_NA,   R_NA,     RD,    MDR,   R_NA,   5'd0 }; // "lhu"
  17'b???????0000001111 : f_dec = '{ type_I,      I,   A_NA,     SI,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   5'd0 }; // "fence"
  17'b???????0010001111 : f_dec = '{ type_I,      I,   A_NA,     SI,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   5'd0 }; // "fence.i"
  17'b???????0000010011 : f_dec = '{ type_I,      I,    ADD,     SI,   R_NA,   R_NA,   R_NA,    RS1,    IMM,     RD,    ALU,   R_NA,   5'd0 }; // "addi"
  17'b???????0100010011 : f_dec = '{ type_I,      I,    SLT,     SI,   R_NA,   R_NA,   R_NA,    RS1,    IMM,     RD,    ALU,   R_NA,   5'd0 }; // "slti"
  17'b???????0110010011 : f_dec = '{ type_I,      I,   SLTU,     SI,   R_NA,   R_NA,   R_NA,    RS1,    IMM,     RD,    ALU,   R_NA,   5'd0 }; // "sltiu"
  17'b???????1000010011 : f_dec = '{ type_I,      I,    XOR,     SI,   R_NA,   R_NA,   R_NA,    RS1,    IMM,     RD,    ALU,   R_NA,   5'd0 }; // "xori"
  17'b???????1100010011 : f_dec = '{ type_I,      I,     OR,     SI,   R_NA,   R_NA,   R_NA,    RS1,    IMM,     RD,    ALU,   R_NA,   5'd0 }; // "ori"
  17'b???????1110010011 : f_dec = '{ type_I,      I,    AND,     SI,   R_NA,   R_NA,   R_NA,    RS1,    IMM,     RD,    ALU,   R_NA,   5'd0 }; // "andi"
  17'b00000000010010011 : f_dec = '{ type_R,      I,    SLL,     SI,   R_NA,   R_NA,   R_NA,    RS1,  SHAMT,     RD,    ALU,   R_NA,   5'd0 }; // "slli"
  17'b00000001010010011 : f_dec = '{ type_R,      I,    SRL,     SI,   R_NA,   R_NA,   R_NA,    RS1,  SHAMT,     RD,    ALU,   R_NA,   5'd0 }; // "srli"
  17'b01000001010010011 : f_dec = '{ type_R,      I,    SRA,     SI,   R_NA,   R_NA,   R_NA,    RS1,  SHAMT,     RD,    ALU,   R_NA,   5'd0 }; // "srai"
  17'b??????????0010111 : f_dec = '{ type_U,      I,    ADD,     SI,   R_NA,    IMM,   R_NA,     PC,    IMM,     RD,    ALU,   R_NA,   5'd0 }; // "auipc"
  17'b???????0000100011 : f_dec = '{ type_S,      I,   A_NA,     QI,    RS1,    IMM,     WE,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   5'd0 }; // "sb"
  17'b???????0010100011 : f_dec = '{ type_S,      I,   A_NA,     HI,    RS1,    IMM,     WE,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   5'd0 }; // "sh"
  17'b???????0100100011 : f_dec = '{ type_S,      I,   A_NA,     SI,    RS1,    IMM,     WE,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   5'd0 }; // "sw"
  17'b00000000000110011 : f_dec = '{ type_R,      I,    ADD,     SI,   R_NA,   R_NA,   R_NA,    RS1,    RS2,     RD,    ALU,   R_NA,   5'd0 }; // "add"
  17'b01000000000110011 : f_dec = '{ type_R,      I,    SUB,     SI,   R_NA,   R_NA,   R_NA,    RS1,    RS2,     RD,    ALU,   R_NA,   5'd0 }; // "sub"
  17'b00000000010110011 : f_dec = '{ type_R,      I,    SLL,     SI,   R_NA,   R_NA,   R_NA,    RS1,    RS2,     RD,    ALU,   R_NA,   5'd0 }; // "sll"
  17'b00000000100110011 : f_dec = '{ type_R,      I,    SLT,     SI,   R_NA,   R_NA,   R_NA,    RS1,    RS2,     RD,    ALU,   R_NA,   5'd0 }; // "slt"
  17'b00000000110110011 : f_dec = '{ type_R,      I,   SLTU,     SI,   R_NA,   R_NA,   R_NA,    RS1,    RS2,     RD,    ALU,   R_NA,   5'd0 }; // "sltu"
  17'b00000001000110011 : f_dec = '{ type_R,      I,    XOR,     SI,   R_NA,   R_NA,   R_NA,    RS1,    RS2,     RD,    ALU,   R_NA,   5'd0 }; // "xor"
  17'b00000001010110011 : f_dec = '{ type_R,      I,    SRL,     SI,   R_NA,   R_NA,   R_NA,    RS1,    RS2,     RD,    ALU,   R_NA,   5'd0 }; // "srl"
  17'b01000001010110011 : f_dec = '{ type_R,      I,    SRA,     SI,   R_NA,   R_NA,   R_NA,    RS1,    RS2,     RD,    ALU,   R_NA,   5'd0 }; // "sra"
  17'b00000001100110011 : f_dec = '{ type_R,      I,     OR,     SI,   R_NA,   R_NA,   R_NA,    RS1,    RS2,     RD,    ALU,   R_NA,   5'd0 }; // "or"
  17'b00000001110110011 : f_dec = '{ type_R,      I,    AND,     SI,   R_NA,   R_NA,   R_NA,    RS1,    RS2,     RD,    ALU,   R_NA,   5'd0 }; // "and"
  17'b00000010000110011 : f_dec = '{ type_R,      M,    MUL,     SI,   R_NA,   R_NA,   R_NA,    RS1,    RS2,     RD,    ALU,   R_NA,   5'd0 }; // "mul"
  17'b00000010010110011 : f_dec = '{ type_R,      M,   MULH,     SI,   R_NA,   R_NA,   R_NA,    RS1,    RS2,     RD,    ALU,   R_NA,   5'd0 }; // "mulh"
  17'b00000010100110011 : f_dec = '{ type_R,      M, MULHSU,     SI,   R_NA,   R_NA,   R_NA,    RS1,    RS2,     RD,    ALU,   R_NA,   5'd0 }; // "mulhsu"
  17'b00000010110110011 : f_dec = '{ type_R,      M,  MULHU,     SI,   R_NA,   R_NA,   R_NA,    RS1,    RS2,     RD,    ALU,   R_NA,   5'd0 }; // "mulhu"
  17'b00000011000110011 : f_dec = '{ type_R,      M,    DIV,     SI,   R_NA,   R_NA,   R_NA,    RS1,    RS2,     RD,    ALU,   R_NA,  5'd16 }; // "div"
  17'b00000011010110011 : f_dec = '{ type_R,      M,   DIVU,     SI,   R_NA,   R_NA,   R_NA,    RS1,    RS2,     RD,    ALU,   R_NA,  5'd16 }; // "divu"
  17'b00000011100110011 : f_dec = '{ type_R,      M,    REM,     SI,   R_NA,   R_NA,   R_NA,    RS1,    RS2,     RD,    ALU,   R_NA,  5'd16 }; // "rem"
  17'b00000011110110011 : f_dec = '{ type_R,      M,   REMU,     SI,   R_NA,   R_NA,   R_NA,    RS1,    RS2,     RD,    ALU,   R_NA,  5'd16 }; // "remu"
  17'b??????????0110111 : f_dec = '{ type_U,      I,    ADD,     SI,   R_NA,    IMM,   R_NA,     X0,    IMM,     RD,    ALU,   R_NA,   5'd0 }; // "lui"
  17'b???????0001100011 : f_dec = '{type_SB,      I,   A_NA,     SI,   R_NA,    IMM,   R_NA,    RS1,    RS2,   R_NA,   R_NA,    BRA,   5'd0 }; // "beq"
  17'b???????0011100011 : f_dec = '{type_SB,      I,   A_NA,     SI,   R_NA,    IMM,   R_NA,    RS1,    RS2,   R_NA,   R_NA,    BRA,   5'd0 }; // "bne"
  17'b???????1001100011 : f_dec = '{type_SB,      I,   A_NA,     SI,   R_NA,    IMM,   R_NA,    RS1,    RS2,   R_NA,   R_NA,    BRA,   5'd0 }; // "blt"
  17'b???????1011100011 : f_dec = '{type_SB,      I,   A_NA,     SI,   R_NA,    IMM,   R_NA,    RS1,    RS2,   R_NA,   R_NA,    BRA,   5'd0 }; // "bge"
  17'b???????1101100011 : f_dec = '{type_SB,      I,   A_NA,     SI,   R_NA,    IMM,   R_NA,    RS1,    RS2,   R_NA,   R_NA,    BRA,   5'd0 }; // "bltu"
  17'b???????1111100011 : f_dec = '{type_SB,      I,   A_NA,     SI,   R_NA,    IMM,   R_NA,    RS1,    RS2,   R_NA,   R_NA,    BRA,   5'd0 }; // "bgeu"
  17'b???????0001100111 : f_dec = '{ type_I,      I,     S2,     SI,   R_NA,    IMM,   R_NA,    RS1,    INC,     RD,    ALU,    JMP,   5'd0 }; // "jalr"
  17'b??????????1101111 : f_dec = '{type_UJ,      I,     S2,     SI,   R_NA,    IMM,   R_NA,   R_NA,    INC,     RD,    ALU,    JMP,   5'd0 }; // "jal"
  17'b00000000001110011 : f_dec = '{ type_I,      E,   A_NA,     SI,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   5'd0 }; // "ecall"
  17'b00000000001110011 : f_dec = '{ type_I,      E,   A_NA,     SI,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   5'd0 }; // "ebreak"
  17'b???????0011110011 : f_dec = '{ type_I,      C,   A_NA,     SI,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   5'd0 }; // "csrrw"
  17'b???????0101110011 : f_dec = '{ type_I,      C,   A_NA,     SI,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   5'd0 }; // "csrrs"
  17'b???????0111110011 : f_dec = '{ type_I,      C,   A_NA,     SI,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   5'd0 }; // "csrrc"
  17'b???????1011110011 : f_dec = '{ type_I,      C,   A_NA,     SI,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   5'd0 }; // "csrrwi"
  17'b???????1101110011 : f_dec = '{ type_I,      C,   A_NA,     SI,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   5'd0 }; // "csrrsi"
  17'b???????1111110011 : f_dec = '{ type_I,      C,   A_NA,     SI,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   5'd0 }; // "csrrci"
  17'b00000000001110011 : f_dec = '{ type_R,      E,   A_NA,     SI,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   5'd0 }; // "uret"
  17'b00010000001110011 : f_dec = '{ type_R,      E,   A_NA,     SI,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   5'd0 }; // "sret"
  17'b00110000001110011 : f_dec = '{ type_R,      E,   A_NA,     SI,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   5'd0 }; // "mret"
  default               : f_dec = '{ type_I,      I,   A_NA,     SI,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   5'd0 };
  endcase

  return f_dec;

endfunction

