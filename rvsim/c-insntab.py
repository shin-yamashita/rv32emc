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

c_tab = []

with open(sys.argv[1], "r") as f:
  reader = csv.reader(f)
  for row in reader:
    if row[0] == "#2":
      break
  print("// ", end='')
  cols = []
  for i in range(1,12):
    print("%s, \t"%row[i],end='')
    cols.append(row[i])
  print()
  c_tab.append(cols)	# c_tab[0] labels

  for row in reader:
    cols = []
    c = row[1].lower()
    if c:
      print(' {"%s",\t'%c, end='')	# mnemonic
      cols.append(c)
      c = 'type_%s'%row[2]
      print('%-7s, '%c, end='')		# type
      cols.append(c)
      for i in range(3,12):
        c = row[i].upper()
        if c == "": c = "__"
        c = c.replace("0X","0x")
        print("%s,\t"%c, end='')
        cols.append(c)
      print(" },")
      c_tab.append(cols)

print("""
};

#define Ncopc    (sizeof(c_optab)/sizeof(c_optab_t))

""")

#------ generate 'rv_exp_cinsn.sv' expand C-insn to full insn ----------------

pre = """
//
// rv_exp_cinsn.sv
// expand C-insn to full insn
// genarated by ./c-insntab.py %s
//

`include "rv_types.svh"

typedef enum u4_t {
  type_CR, type_CI, type_CSS, type_CIW, type_CL, type_CS, type_CB, type_CJ
} citype_t;

typedef enum u4_t {
  CX_NA, CX0, CX1, CX2, CRS1, CRS2, CRS1D, CRS2D
} cregs_t;

typedef enum u4_t {
  CIMM_NA, CIMM0, CIMM1, CIMM2, CIMM3, CIMM4, CIMM5, CIMM6, CIMM7, CIMM8, CIMM9, CIMM10
} cimm_t;

typedef struct {
  citype_t itype;
  cimm_t   imm;
  cregs_t  rs2;
  cregs_t  rs1;
  cregs_t  rd;
  logic [6:0] func7;
  logic [2:0] func3;
  logic [6:0] opc;
} c_insn_t;

function u64_t exp_cinsn (input u16_t ir);
  u32_t exir, c_imm;
  c_insn_t c_dec;
"""%sys.argv[1]

post = """
  case (c_dec.imm)
  CIMM0:  c_imm = 'h00000000;
  CIMM1:  c_imm = u32_t'({ir[8:7],ir[12:9],2'b0});
  CIMM2:  c_imm = u32_t'({ir[3:2],ir[12],ir[6:4],2'b0});
  CIMM3:  c_imm = u32_t'(s32_t'({ir[12],ir[6:5],ir[2],ir[11:10],ir[4:3],1'b0}));
  CIMM4:  c_imm = u32_t'({ir[12],ir[6:2]});
  CIMM5:  c_imm = u32_t'(s32_t'({ir[12],ir[8],ir[10:9],ir[6],ir[7],ir[2],ir[5:3],1'b0}));
  CIMM6:  c_imm = u32_t'(s32_t'({ir[12],ir[6:2],12'b0}));
  CIMM7:  c_imm = u32_t'(s32_t'({ir[12],ir[4:3],ir[5],ir[2],ir[6],4'b0}));
  CIMM8:  c_imm = u32_t'({ir[5],ir[12:10],ir[6],2'b0});
  CIMM9:  c_imm = u32_t'({ir[10:7],ir[12],ir[5],ir[6],2'b0});
  CIMM10: c_imm = u32_t'(s32_t'({ir[12],ir[6:2]}));
  default: c_imm = 'h00000000;
  endcase

  case (c_dec.rs1)
  CX0:   exir[19:15] = 5'h0;
  CX2:   exir[19:15] = 5'h2;
  CRS1D: exir[19:15] = {2'h2,ir[9:7]};
  CRS1:  exir[19:15] = ir[11:7];
  default: exir[19:15] = 5'h0;
  endcase

  case (c_dec.rs2)
  CX0:   exir[24:20] = 5'h0;
  CRS2D: exir[24:20] = {2'h2,ir[4:2]};
  CRS2:  exir[24:20] = ir[6:2];
  default: exir[24:20] = 5'h0;
  endcase

  case (c_dec.rd)
  CX0:   exir[11:7] = 5'h0;
  CX1:   exir[11:7] = 5'h1;
  CX2:   exir[11:7] = 5'h2;
  CRS1:  exir[11:7] = ir[11:7];
  CRS1D: exir[11:7] = {2'h2,ir[9:7]};
  CRS2D: exir[11:7] = {2'h2,ir[4:2]};
  CRS2:  exir[11:7] = ir[11:7];
  default: exir[11:7] = 5'h0;
  endcase

  exir[31:25] = c_dec.func7;
  exir[14:12] = c_dec.func3;
  exir[1:0] = 2'h3;

  return {exir, c_imm};

endfunction

"""


def maskedcode(mask, code):
  m = format(int(mask, 16), '016b')
  c = format(int(code, 16), '016b')
  mc = ''
  for i in range(16):
    if m[i] == '0':
      mc += '?'
    else:
      mc += c[i]
  return mc

with open('rv_exp_cinsn.sv', 'w') as f:
  print(pre, file=f)
  print("""  case (ir[15:0]) inside""", file=f)
  mnemonic,itype,mask,opcode,imm,rs2,rs1,rd,func7,func3,opc = c_tab[0]
  print("                    // : c_dec = '{", end='', file=f)
  for c in [itype,imm,rs2,rs1,rd,func7,func3,opc] :
    print("%8s,"%c, end='', file=f)
  print("};  // mnemonic", file=f)

  for cols in c_tab[1:]:
    mnemonic,itype,mask,opcode,imm,rs2,rs1,rd,func7,func3,opc = cols
    print("  16'b%s : c_dec = '{" % maskedcode(mask,opcode), end='', file=f)
    imm = imm.replace("0x0","CIMM0").replace("__", "CIMM_NA")
    rs2 = rs2.replace("__", "CX_NA")
    rs1 = rs1.replace("__", "CX_NA")
    rd = rd.replace("__", "CX_NA")
    func7 = func7.replace("__", "7'h0").replace("0x", "7'h")
    func3 = "3'h" + func3.replace("__", "0")
    opc = opc.replace("0x", "7'h")
    for i,c in enumerate([itype,imm,rs2,rs1,rd,func7,func3,opc]) :
      dlm = ',' if i < 7 else ' '
      print("%8s"%c, end=dlm, file=f)
    print("};  // ", mnemonic, file=f)

  print("""  default :              c_dec = '{type_CR,  CIMM_NA,   CX_NA,   CX_NA,   CX_NA,    7'h0,    3'h0,   7'h00 };
  endcase""", file=f)
  print(post, file=f)



