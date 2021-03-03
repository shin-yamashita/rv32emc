

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

//  mnemonic, 	type, 	mask, 	opcode, 	imm, 	rs2, 	rs1, 	rd, 	func7, 	func3, 	opc, 	
 {"c.ai4sp",	type_CIW,  0xE003,	0x0000,	CIMM9,	__,	CX2,	CRS2D,	__,	0,	0x13,	 },
 {"c.lw",	type_CL,  0xE003,	0x4000,	CIMM8,	__,	CRS1D,	CRS2D,	__,	2,	0x03,	 },
 {"c.flw",	type_CL,  0xE003,	0x6000,	CIMM8,	__,	CRS1D,	CRS2D,	__,	2,	0x07,	 },
 {"c.sw",	type_CS,  0xE003,	0xC000,	CIMM8,	CRS2D,	CRS1D,	__,	__,	2,	0x23,	 },
 {"c.fsw",	type_CS,  0xE003,	0xE000,	CIMM8,	CRS2D,	CRS1D,	__,	__,	2,	0x27,	 },
 {"c.nop",	type_CR,  0xFFFF,	0x0001,	0x0,	__,	CX0,	CX0,	__,	0,	0x13,	 },
 {"c.addi",	type_CI,  0xE003,	0x0001,	CIMM10,	__,	CRS1,	CRS1,	__,	0,	0x13,	 },
 {"c.jal",	type_CJ,  0xE003,	0x2001,	CIMM5,	__,	__,	CX1,	__,	__,	0x6F,	 },
 {"c.li",	type_CI,  0xE003,	0x4001,	CIMM10,	__,	CX0,	CRS1,	__,	0,	0x13,	 },
 {"c.ai16sp",	type_CI,  0xEF83,	0x6101,	CIMM7,	__,	CX2,	CX2,	__,	0,	0x13,	 },
 {"c.lui",	type_CI,  0xE003,	0x6001,	CIMM6,	__,	__,	CRS1,	__,	__,	0x37,	 },
 {"c.srli",	type_CI,  0xEC03,	0x8001,	CIMM4,	__,	CRS1D,	CRS1D,	0x00,	5,	0x13,	 },
 {"c.srai",	type_CI,  0xEC03,	0x8401,	CIMM4,	__,	CRS1D,	CRS1D,	0x20,	5,	0x13,	 },
 {"c.andi",	type_CI,  0xEC03,	0x8801,	CIMM10,	__,	CRS1D,	CRS1D,	__,	7,	0x13,	 },
 {"c.sub",	type_CL,  0xFC63,	0x8C01,	__,	CRS2D,	CRS1D,	CRS1D,	0x20,	0,	0x33,	 },
 {"c.xor",	type_CL,  0xFC63,	0x8C21,	__,	CRS2D,	CRS1D,	CRS1D,	0x00,	4,	0x33,	 },
 {"c.or",	type_CL,  0xFC63,	0x8C41,	__,	CRS2D,	CRS1D,	CRS1D,	0x00,	6,	0x33,	 },
 {"c.and",	type_CL,  0xFC63,	0x8C61,	__,	CRS2D,	CRS1D,	CRS1D,	0x00,	7,	0x33,	 },
 {"c.j",	type_CJ,  0xE003,	0xA001,	CIMM5,	__,	__,	CX0,	__,	__,	0x6F,	 },
 {"c.beqz",	type_CB,  0xE003,	0xC001,	CIMM3,	__,	CRS1D,	CX0,	__,	0,	0x63,	 },
 {"c.bnez",	type_CB,  0xE003,	0xE001,	CIMM3,	__,	CRS1D,	CX0,	__,	1,	0x63,	 },
 {"c.slli",	type_CI,  0xE003,	0x0002,	CIMM4,	__,	CRS1,	CRS1,	0x00,	1,	0x13,	 },
 {"c.lwsp",	type_CI,  0xE003,	0x4002,	CIMM2,	__,	CX2,	CRS1,	__,	2,	0x03,	 },
 {"c.flwsp",	type_CI,  0xE003,	0x6002,	CIMM2,	__,	CX2,	CRS1,	__,	2,	0x07,	 },
 {"c.jr",	type_CI,  0xF07F,	0x8002,	__,	__,	CRS1,	CX0,	__,	0,	0x67,	 },
 {"c.mv",	type_CR,  0xF003,	0x8002,	__,	CRS2,	CX0,	CRS1,	0x00,	0,	0x33,	 },
 {"c.ebreak",	type_CI,  0xFFFF,	0x9002,	__,	__,	__,	__,	__,	0,	0x73,	 },
 {"c.jalr",	type_CR,  0xF07F,	0x9002,	0x0,	__,	CRS1,	CX1,	__,	0,	0x67,	 },
 {"c.add",	type_CR,  0xF003,	0x9002,	__,	CRS2,	CRS1,	CRS1,	0x00,	0,	0x33,	 },
 {"c.swsp",	type_CSS,  0xE003,	0xC002,	CIMM1,	CRS2,	CX2,	__,	__,	2,	0x23,	 },
 {"c.fswsp",	type_CSS,  0xE003,	0xE002,	CIMM1,	CRS2,	CX2,	__,	__,	2,	0x27,	 },

};

#define Ncopc    (sizeof(c_optab)/sizeof(c_optab_t))


