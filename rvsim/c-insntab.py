#!/usr/bin/env python3

import sys
import csv

if(len(sys.argv) < 2):
  print("*** %s .csv > c-optab.h"%sys.argv[0])
  sys.exit(0)


print('''//
// rv32 Compressed insn table 
// genarated by ./c-insntab.py %s
//
'''%sys.argv[1])

print('''
typedef enum _citype {
  type_CR, type_CI, type_CSS, type_CIW, type_CL, type_CS, type_CB, type_CJ
} citype_t;

typedef enum _cregs {
  CX0=0, CX1, CX2, CRS1, CRS2, CRS1D, CRS2D
} cregs_t;

typedef enum _cimm {
  CIMM1=1, CIMM2, CIMM3, CIMM4, CIMM5, CIMM6, CIMM7, CIMM8, CIMM9, CIMM10
} cimm_t;

typedef struct _c_optab {
  char *mnemonic;
  citype_t type;
  u32      mask;
  u32      opcode;
  cimm_t   imm;
  cregs_t  rs2;
  cregs_t  rs1;
  cregs_t  rd;
  u32      func7;
  u32      func3;
  u32      opc;
} c_optab_t;

#define isCinsn(x)	(((x)&0x3)<3)

static c_optab_t c_optab[] = {
''')

#2,mnemonic,type,mask,opcode,imm,rs2,rs1,rd,func7,func3,opc,

with open(sys.argv[1], "r") as f:
  reader = csv.reader(f)
  for row in reader:
    if row[0] == "#2":
      break
  print("// ", end='')
  for i in range(1,12):
    print("%s, \t"%row[i],end='')
  print()
  for row in reader:
    c = row[1].lower()
    if c:
      print(' {"%s",\t'%c, end='')	# mnemonic
      c = 'type_%s'%row[2]
      print('%-7s, '%c, end='')		# type
      for i in range(3,12):
        c = row[i].upper()
        if c == "": c = "__"
        c = c.replace("0X","0x")
        print("%s,\t"%c, end='')
      print(" },")

print("""
};

#define Ncopc    (sizeof(c_optab)/sizeof(c_optab_t))

""")
