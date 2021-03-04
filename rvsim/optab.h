//
// rv32 insn table 
// genarated by ./insntab.py RV-insn.csv
//


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

// mnemonic, 	type, 	, 	func7, 	func3, 	opc, 	alu, 	mode, 	mar, 	ofs, 	mwe, 	rrd1, 	rrd2, 	rwa, 	rwd, 	pc, 	
 { "lb",	type_I , I,	__,	0,	0X03,	__,	SQI,	RS1,	IMM,	RE,	__,	__,	RD,	MDR,	__,	 }, // 0
 { "lh",	type_I , I,	__,	1,	0X03,	__,	SHI,	RS1,	IMM,	RE,	__,	__,	RD,	MDR,	__,	 }, // 1
 { "lw",	type_I , I,	__,	2,	0X03,	__,	SI,	RS1,	IMM,	RE,	__,	__,	RD,	MDR,	__,	 }, // 2
 { "lbu",	type_I , I,	__,	4,	0X03,	__,	QI,	RS1,	IMM,	RE,	__,	__,	RD,	MDR,	__,	 }, // 3
 { "lhu",	type_I , I,	__,	5,	0X03,	__,	HI,	RS1,	IMM,	RE,	__,	__,	RD,	MDR,	__,	 }, // 4
 { "fence",	type_I , I,	__,	0,	0X0F,	__,	SI,	__,	__,	__,	__,	__,	__,	__,	__,	 }, // 5
 { "fence.i",	type_I , I,	__,	1,	0X0F,	__,	SI,	__,	__,	__,	__,	__,	__,	__,	__,	 }, // 6
 { "addi",	type_I , I,	__,	0,	0X13,	ADD,	SI,	__,	__,	__,	RS1,	IMM,	RD,	ALU,	__,	 }, // 7
 { "slti",	type_I , I,	__,	2,	0X13,	SLT,	SI,	__,	__,	__,	RS1,	IMM,	RD,	ALU,	__,	 }, // 8
 { "sltiu",	type_I , I,	__,	3,	0X13,	SLTU,	SI,	__,	__,	__,	RS1,	IMM,	RD,	ALU,	__,	 }, // 9
 { "xori",	type_I , I,	__,	4,	0X13,	XOR,	SI,	__,	__,	__,	RS1,	IMM,	RD,	ALU,	__,	 }, // 10
 { "ori",	type_I , I,	__,	6,	0X13,	OR,	SI,	__,	__,	__,	RS1,	IMM,	RD,	ALU,	__,	 }, // 11
 { "andi",	type_I , I,	__,	7,	0X13,	AND,	SI,	__,	__,	__,	RS1,	IMM,	RD,	ALU,	__,	 }, // 12
 { "slli",	type_R , I,	0X00,	1,	0X13,	SLL,	SI,	__,	__,	__,	RS1,	SHAMT,	RD,	ALU,	__,	 }, // 13
 { "srli",	type_R , I,	0X00,	5,	0X13,	SRL,	SI,	__,	__,	__,	RS1,	SHAMT,	RD,	ALU,	__,	 }, // 14
 { "srai",	type_R , I,	0X20,	5,	0X13,	SRA,	SI,	__,	__,	__,	RS1,	SHAMT,	RD,	ALU,	__,	 }, // 15
 { "auipc",	type_U , I,	__,	__,	0X17,	ADD,	SI,	__,	IMM,	__,	PC,	IMM,	RD,	ALU,	__,	 }, // 16
 { "sb",	type_S , I,	__,	0,	0X23,	__,	QI,	RS1,	IMM,	WE,	__,	__,	__,	__,	__,	 }, // 17
 { "sh",	type_S , I,	__,	1,	0X23,	__,	HI,	RS1,	IMM,	WE,	__,	__,	__,	__,	__,	 }, // 18
 { "sw",	type_S , I,	__,	2,	0X23,	__,	SI,	RS1,	IMM,	WE,	__,	__,	__,	__,	__,	 }, // 19
 { "add",	type_R , I,	0X00,	0,	0X33,	ADD,	SI,	__,	__,	__,	RS1,	RS2,	RD,	ALU,	__,	 }, // 20
 { "sub",	type_R , I,	0X20,	0,	0X33,	SUB,	SI,	__,	__,	__,	RS1,	RS2,	RD,	ALU,	__,	 }, // 21
 { "sll",	type_R , I,	0X00,	1,	0X33,	SLL,	SI,	__,	__,	__,	RS1,	RS2,	RD,	ALU,	__,	 }, // 22
 { "slt",	type_R , I,	0X00,	2,	0X33,	SLT,	SI,	__,	__,	__,	RS1,	RS2,	RD,	ALU,	__,	 }, // 23
 { "sltu",	type_R , I,	0X00,	3,	0X33,	SLTU,	SI,	__,	__,	__,	RS1,	RS2,	RD,	ALU,	__,	 }, // 24
 { "xor",	type_R , I,	0X00,	4,	0X33,	XOR,	SI,	__,	__,	__,	RS1,	RS2,	RD,	ALU,	__,	 }, // 25
 { "srl",	type_R , I,	0X00,	5,	0X33,	SRL,	SI,	__,	__,	__,	RS1,	RS2,	RD,	ALU,	__,	 }, // 26
 { "sra",	type_R , I,	0X20,	5,	0X33,	SRA,	SI,	__,	__,	__,	RS1,	RS2,	RD,	ALU,	__,	 }, // 27
 { "or",	type_R , I,	0X00,	6,	0X33,	OR,	SI,	__,	__,	__,	RS1,	RS2,	RD,	ALU,	__,	 }, // 28
 { "and",	type_R , I,	0X00,	7,	0X33,	AND,	SI,	__,	__,	__,	RS1,	RS2,	RD,	ALU,	__,	 }, // 29
 { "mul",	type_R , M,	0X01,	0,	0X33,	MUL,	SI,	__,	__,	__,	RS1,	RS2,	RD,	ALU,	__,	 }, // 30
 { "mulh",	type_R , M,	0X01,	1,	0X33,	MULH,	SI,	__,	__,	__,	RS1,	RS2,	RD,	ALU,	__,	 }, // 31
 { "mulhsu",	type_R , M,	0X01,	2,	0X33,	MULHSU,	SI,	__,	__,	__,	RS1,	RS2,	RD,	ALU,	__,	 }, // 32
 { "mulhu",	type_R , M,	0X01,	3,	0X33,	MULHU,	SI,	__,	__,	__,	RS1,	RS2,	RD,	ALU,	__,	 }, // 33
 { "div",	type_R , M,	0X01,	4,	0X33,	DIV,	SI,	__,	__,	__,	RS1,	RS2,	RD,	ALU,	__,	 }, // 34
 { "divu",	type_R , M,	0X01,	5,	0X33,	DIVU,	SI,	__,	__,	__,	RS1,	RS2,	RD,	ALU,	__,	 }, // 35
 { "rem",	type_R , M,	0X01,	6,	0X33,	REM,	SI,	__,	__,	__,	RS1,	RS2,	RD,	ALU,	__,	 }, // 36
 { "remu",	type_R , M,	0X01,	7,	0X33,	REMU,	SI,	__,	__,	__,	RS1,	RS2,	RD,	ALU,	__,	 }, // 37
 { "lui",	type_U , I,	__,	__,	0X37,	ADD,	SI,	__,	IMM,	__,	X0,	IMM,	RD,	ALU,	__,	 }, // 38
 { "beq",	type_SB, I,	__,	0,	0X63,	CMP,	SI,	__,	IMM,	__,	RS1,	RS2,	__,	__,	BRA,	 }, // 39
 { "bne",	type_SB, I,	__,	1,	0X63,	CMP,	SI,	__,	IMM,	__,	RS1,	RS2,	__,	__,	BRA,	 }, // 40
 { "blt",	type_SB, I,	__,	4,	0X63,	CMP,	SI,	__,	IMM,	__,	RS1,	RS2,	__,	__,	BRA,	 }, // 41
 { "bge",	type_SB, I,	__,	5,	0X63,	CMP,	SI,	__,	IMM,	__,	RS1,	RS2,	__,	__,	BRA,	 }, // 42
 { "bltu",	type_SB, I,	__,	6,	0X63,	CMP,	SI,	__,	IMM,	__,	RS1,	RS2,	__,	__,	BRA,	 }, // 43
 { "bgeu",	type_SB, I,	__,	7,	0X63,	CMP,	SI,	__,	IMM,	__,	RS1,	RS2,	__,	__,	BRA,	 }, // 44
 { "jalr",	type_I , I,	__,	0,	0X67,	S2,	SI,	__,	IMM,	__,	RS1,	INC,	RD,	ALU,	JMP,	 }, // 45
 { "jal",	type_UJ, I,	__,	__,	0X6F,	S2,	SI,	__,	IMM,	__,	__,	INC,	RD,	ALU,	JMP,	 }, // 46
 { "ecall",	type_I , E,	0X00,	0,	0X73,	__,	SI,	__,	__,	__,	__,	__,	__,	__,	__,	 }, // 47
 { "ebreak",	type_I , E,	0X00,	0,	0X73,	__,	SI,	__,	__,	__,	__,	__,	__,	__,	__,	 }, // 48
 { "csrrw",	type_I , C,	__,	1,	0X73,	__,	SI,	__,	__,	__,	__,	__,	__,	__,	__,	 }, // 49
 { "csrrs",	type_I , C,	__,	2,	0X73,	__,	SI,	__,	__,	__,	__,	__,	__,	__,	__,	 }, // 50
 { "csrrc",	type_I , C,	__,	3,	0X73,	__,	SI,	__,	__,	__,	__,	__,	__,	__,	__,	 }, // 51
 { "csrrwi",	type_I , C,	__,	5,	0X73,	__,	SI,	__,	__,	__,	__,	__,	__,	__,	__,	 }, // 52
 { "csrrsi",	type_I , C,	__,	6,	0X73,	__,	SI,	__,	__,	__,	__,	__,	__,	__,	__,	 }, // 53
 { "csrrci",	type_I , C,	__,	7,	0X73,	__,	SI,	__,	__,	__,	__,	__,	__,	__,	__,	 }, // 54
 { "uret",	type_R , E,	0X00,	0,	0X73,	__,	SI,	__,	__,	__,	__,	__,	__,	__,	__,	 }, // 55
 { "sret",	type_R , E,	0X08,	0,	0X73,	__,	SI,	__,	__,	__,	__,	__,	__,	__,	__,	 }, // 56
 { "mret",	type_R , E,	0X18,	0,	0X73,	__,	SI,	__,	__,	__,	__,	__,	__,	__,	__,	 }, // 57

};

#define Nopc    (sizeof(optab)/sizeof(optab_t))


