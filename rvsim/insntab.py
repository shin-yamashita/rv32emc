#!/usr/bin/env python3

import sys
import csv

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
  CMP, S2, ADD, SLT, SLTU, XOR, OR, AND, SLL, SRL, SRA, SUB,
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
  int    rp;
} optab_t;

static optab_t optab[] = {
''')

#//  mnemonic,   type,   ex,     func7,  func3,  opc,    alu,    mode,   mar,    ofs,    mwe,    rrd1,   rrd2,   rwa,    rwd,    pc,     rp

table = []

with open(sys.argv[1], "r") as f:
  reader = csv.reader(f)
  for row in reader:
    if row[0] == "#2":
      break
  print("// ", end='')
  for i in range(1,17):
    print("%s, \t"%row[i], end='')
  print()
  
  for row in reader:
    c = row[1].lower()
    if c:
      line = ' { "%s",\t'%c	# mnemonic
      c = 'type_%s'%row[2]
      line += '%-7s, '%c		# type
      for i in range(3,17):
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

