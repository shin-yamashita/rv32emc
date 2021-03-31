#!/usr/bin/env python3

import sys
import csv
import re

if(len(sys.argv) < 2):
  print("*** %s .csv > optab.h"%sys.argv[0])
  sys.exit(0)
  
print('''//
// rv32 insn table 
// genarated by ./insntab.py %s
//
'''%sys.argv[1])

print('''
#define NA      -2
#define __      -1

typedef enum _itype {
  type_U, type_UJ, type_I, type_SB, type_S, type_R, type_RF
} itype_t;

typedef enum _ex {
  I, M, F, E, C
} ex_t;

typedef enum _mode {
  SI, HI, QI, SHI, SQI
} wmode_t;

typedef enum _alu {
  A_NA, CMP, S2, ADD, SLT, SLTU, XOR, OR, AND, SLL, SRL, SRA, SUB,
  MUL, MULH, MULHSU, MULHU, DIV, DIVU, REM, REMU,
  FLD, FST, FMADD, FMSUB, FADD, FSUB, FMUL, FDIV, FSQRT, FSGN, FMIN, FMAX, FCVT
} alu_t;

const char* _alu_nam[] = {
  "--",  "CMP", "S2", "ADD", "SLT",  "SLTU", "XOR", "OR",   "AND", "SLL", "SRL", "SRA", "SUB",
  "MUL", "MULH","MHSU", "MHU",  "DIV", "DIVU", "REM", "REMU",
  "FLD", "FST", "FMAD", "FMSB", "FADD","FSUB", "FMUL","FDIV", "FSQR", "FSGN", "FMIN", "FMAX", "FCVT"
};

typedef enum _regs {
  X0, RS1, RS2, RD, IMM, PC, RM, WE, RE, MDR, ALU, INC, JMP, BRA, SHAMT
} regs_t;

const char* _regs_nam[] = {
  "--", "X0", "RS1", "RS2", "RD", "IMM", "PC", "RM", "WE", "RE", "MDR", "ALU", "INC", "JMP","BRA","SHM"
};

typedef struct _optab {
  char *mnemonic;
  itype_t type;
  ex_t   ex;
  u32    func7;
  u32    func3;
  u32    opc;
  alu_t  alu;
  wmode_t mode;
  int    mar;
  int    ofs;
  int    mwe;
  int    rrd1;
  int    rrd2;
  int    rwa;
  int    rwd;
  int    pc;
  int    excyc;
} optab_t;

static optab_t optab[] = {
''')

#//  mnemonic,   type,   ex,     func7,  func3,  opc,    alu,    mode,   mar,    ofs,    mwe,    rrd1,   rrd2,   rwa,    rwd,    pc,   excyc

table = []

with open(sys.argv[1], "r") as f:
  reader = csv.reader(f)
  for row in reader:
    if row[0] == "#2":
      break
  print("// ", end='')
  for i in range(1,18):
    print("%s, \t"%row[i], end='')
  print()
  
  for row in reader:
    c = row[1].lower()
    if c:
      line = ' { "%s",\t'%c	# mnemonic
      c = 'type_%s'%row[2]
      line += '%-7s, '%c		# type
      for i in range(3,18):
        c = row[i].upper()
        if c == "": c = "__"
        line += "%s,\t"%c
      line += " },"
      table.append((int(row[6], 16), line))
#      print line

from operator import itemgetter

table = sorted(table, key=itemgetter(0))

opc = {}
for i,line in enumerate(table):
  if(line[0] not in opc ) : opc[line[0]] = i
  print(line[1], "//", i)

print("""
};

#define Nopc    (sizeof(optab)/sizeof(optab_t))

""")


# print opc


def maskedcode(func7, func3, opc):
  op = format(int(opc, 16), '07b')
  mc = ''
  if func7 == '__':
    mc += "???????"
  else:
    mc += format(int(func7, 16), '07b')
  if func3 == '__':
    mc += "???"
  else:
    mc += format(int(func3, 16), '03b')
  mc += op
  return mc


#---- genarate rv_dec_insn.sv

#with open('/dev/stderr', 'w') as f:
with open('rv_dec_insn.sv', 'w') as f:
  print("""
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

typedef enum u8_t {
  A_NA, CMP, S2, ADD, SLT, SLTU, XOR, OR, AND, SLL, SRL, SRA, SUB,
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

  case ({ir[31:25],ir[14:12],ir[6:0]}) inside""", file=f)

  print("                     // : f_dec = '{   type,     ex,    alu,   mode,    mar,    ofs,    mwe,   rrd1,   rrd2,    rwa,    rwd,     pc,  excyc }; // mnemonic", file=f)

  opcs = {}
  for i,line in enumerate(table):
    if(line[0] not in opcs ) : opcs[line[0]] = i
  #  print(line[1], "//", i)
    cols = re.sub('[ \t{}]','',line[1]).split(',')

    mnemonic, type, ex, func7, func3, opc, alu, mode, mar, ofs, mwe, rrd1, rrd2, rwa, rwd, pc, excyc, _,_ = cols
    alu = alu.replace("__", "A_NA")
    mar = mar.replace("__", "R_NA")
    ofs = ofs.replace("__", "R_NA")
    mwe = mwe.replace("__", "R_NA")
    rrd1 = rrd1.replace("__", "R_NA")
    rrd2 = rrd2.replace("__", "R_NA")
    rwa = rwa.replace("__", "R_NA")
    rwd = rwd.replace("__", "R_NA")
    pc = pc.replace("__", "R_NA")
    excyc = "5'd" + excyc

    print("  17'b%s :" % maskedcode(func7, func3, opc),"f_dec = '{", end='', file=f)
    for j,c in enumerate([type, ex, alu, mode, mar, ofs, mwe, rrd1, rrd2, rwa, rwd, pc, excyc]):
      dlm = ',' if j < 12 else ' '
      print("%7s"%c, end=dlm, file=f)
    print("}; //", mnemonic, file=f)
  print("  default               : f_dec = '{ type_I,      I,   A_NA,     SI,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   R_NA,   5'd0 };", file=f)
  print("  endcase", file=f)
  print("""
  return f_dec;

endfunction
""", file=f)


