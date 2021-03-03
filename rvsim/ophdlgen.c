
#include <stdio.h>
#include <string.h>

#define HDLGEN
#include "optab.h"


char *immcd(int i)
{
	if(optab[i].immsz == 4) return "I_U4";
	if(optab[i].immsz == 8 && optab[i].sft == 4){
		if((optab[i].ofs | optab[i].rrd1) == IMM) return "I_S8";
		else return "I_U8";
	}
	if(optab[i].immsz == 5){
		if((optab[i].ofs | optab[i].rrd1) == IMM) return "I_S5";
		else return "I_U5";
	}
	if(optab[i].immsz == 8 && optab[i].sft == 0){
		if((optab[i].spfunc == ADDSP)) return "I_D10";			// addsp
		else if((optab[i].ofs | optab[i].rrd1) == IMM) return "I_D9";	// bcc
		else return "I_V8";	// int #v8
	}
	return "I_NA";
}

char *S_(int i)
{
	switch(i){
	case POPM:	return "S_POPM";
	case PUSHM:	return "S_PUSHM";
	case BCC:	return "S_BCC";
	case ADDSP:	return "S_ADDSP";
	case INT:	return "S_INT";
	case JMP:	return "S_JMP";
	case CALL:	return "S_CALL";
	case RET:	return "S_RET";
	case RTI:	return "S_RTI";
	default:	return "S_NA";
	}
}

char *O_(int c)
{
	switch(c){
	case RJ:	return "O_RJ";   
	case RI:	return "O_RI";   
	case RS1:	return "O_RS1";  
	case RS2:	return "O_RS2";  
	case R15:	return "O_R15";  
	case RP: 	return "O_RP";  
	case RD:  	return "O_RD"; 
	case IMM:	return "O_IMM";  
	case UIMM:	return "O_UIMM";
	case PINC:	return "O_PINC"; 
	case PDEC:	return "O_PDEC"; 
	case MDR: 	return "O_MDR"; 
	case RWD: 	return "O_RWD";
	case RRD1:	return "O_RRD1"; 
	default: return "O_NA";
	}
}

char *A_(int a)
{
	switch(a){
	case AR1:	return "A_AR1";
	case AR2:	return "A_AR2";
	case ADD:	return "A_ADD";
	case LSL:	return "A_LSL"; 
	case ASR:	return "A_ASR"; 
	case LSR:	return "A_LSR"; 
	case SUB:	return "A_SUB"; 
	case CMP:	return "A_CMP";
	case LAND:	return "A_LAND";
	case OR:  	return "A_OR";  
	case EOR: 	return "A_EOR"; 
	case MUL: 	return "A_MUL"; 
	case MULU:	return "A_MULU"; 
	case MULH:	return "A_MULH"; 
	case MULUH:	return "A_MULUH";
	case ADDC:	return "A_ADDC";
	case NEG: 	return "A_NEG";
	case NOT: 	return "A_NOT"; 
	case EXTB:	return "A_EXTB";
	case EXTH:	return "A_EXTH";
	case SXTB:	return "A_SXTB";
	case SXTH:	return "A_SXTH";
	case DIV: 	return "A_DIV";
	case DIVU:	return "A_DIVU";
	case MOD: 	return "A_MOD";
	case MODU:	return "A_MODU";
	case ADDF:	return "A_ADDF";
	case SUBF:	return "A_SUBF";
	case MULF:	return "A_MULF";
	case FLT:	return "A_FLT";
	case FLTU:	return "A_FLTU";
	case FIX:	return "A_FIX";
	case FIXU:	return "A_FIXU";
	case SQRT:	return "A_SQRT";
	case DIVF:	return "A_DIVF";
	case CMPF:	return "A_CMPF";
	default:	return "A_NA";
	}
}

char *bin(int x, int len)
{
	int i;
	static char str[10];

	for(i = 0; i < len; i++){
		int b = x & 0x1;
		x >>= 1;
		str[len-i-1] = '0' + b;
	}
	str[len] = 0;
	return str;
}

int indnt = 0;

char *ind()
{
	int i;
	static char s[20];
	for(i = 0; i < indnt*2 && i < 19; i++){
		s[i] = ' ';
	}
	s[i] = '\0';
	return s;
}

char *table(int mask, int sft, char *tab)
{
	int i;
	char s[20], cst[100], *trm;

	sprintf(cst, "%s  case(conv_integer(ir(15 downto %d))) is\n", ind(), sft);
	for(i = 0; i < Nopc; i++){
	  if(optab[i].opmask == mask){

	    if(!strcmp(tab,"immcd")) 	{trm="I_NA";	sprintf(s, "%s", 	immcd(i));}
	    if(!strcmp(tab,"len")) 	{trm="\"01\"";	sprintf(s, "\"%s\"", 	bin(optab[i].len, 2));}
	    if(!strcmp(tab,"ex")) 	{trm="\"00000\"";sprintf(s, "\"%s\"", 	bin(optab[i].ex, 5));}
	    if(!strcmp(tab,"spfunc")) 	{trm="S_NA";	sprintf(s, "%s", 	S_(optab[i].spfunc));}
	    if(!strcmp(tab,"mode")) 	{trm="\"000\"";	sprintf(s, "\"%s\"", 	bin(optab[i].mode, 3));}
	    if(!strcmp(tab,"mar")) 	{trm="O_NA";	sprintf(s, "%s", 	O_(optab[i].mar));}
	    if(!strcmp(tab,"ofs")) 	{trm="O_NA";	sprintf(s, "%s", 	O_(optab[i].ofs));}
	    if(!strcmp(tab,"mwe")) 	{trm="O_NA";	sprintf(s, "%s", 	O_(optab[i].mwe));}
	    if(!strcmp(tab,"rrd1")) 	{trm="O_NA";	sprintf(s, "%s", 	O_(optab[i].rrd1));}
	    if(!strcmp(tab,"rrd2")) 	{trm="O_NA";	sprintf(s, "%s", 	O_(optab[i].rrd2));}
	    if(!strcmp(tab,"rwa")) 	{trm="O_NA";	sprintf(s, "%s", 	O_(optab[i].rwa));}
	    if(!strcmp(tab,"rwd")) 	{trm="O_NA";	sprintf(s, "%s", 	O_(optab[i].rwd));}
	    if(!strcmp(tab,"alu")) 	{trm="A_NA";	sprintf(s, "%s", 	A_(optab[i].alu));}
	    if(!strcmp(tab,"dl")) 	{trm="false";	sprintf(s, "%s", 	optab[i].dl == EN ? "true" : "false");}

	    if(strcmp(s, trm)) 	{
		printf("%s%s  when 16#%x# => op.%s := %s;\n", cst, ind(), optab[i].code >> sft, tab, s); 
		cst[0] = 0;
	    }
	  }
	}
	if(!cst[0]) {
		printf("%s  when others =>\n", ind());
		indnt++;
	}
	return trm;
}

void case_gen(char *tab)
{
	char *s;

	indnt = 0;
//0xf000,0xfe00,0xff00,0xfff0,0xffff,
	table(0xf000, 12, tab);
	table(0xfe00, 9, tab);
	table(0xff00, 8, tab);
	table(0xfff0, 4, tab);
	s = table(0xffff, 0, tab);
	printf("		op.%s := %s;\n", tab, s);
//I_NA, \"01\", \"0000\",   S_NA, \"000\",   O1_NA,   O2_NA,   O1_NA,   O2_NA,   O1_NA,   O1_NA,   O3_NA,   A_NA, false);\n");
	while(indnt){
		indnt--;
		printf("%s  end case;\n", ind());
	}
	printf("\n");
}

int main()
{

printf(
"-- sr-processor opcode table\n"
"--       (genarated by srsim/ophdlgen)\n"
"\n"
"library ieee;\n"
"use ieee.std_logic_1164.all;\n"
"use ieee.std_logic_arith.all;\n"
"use ieee.std_logic_unsigned.all;\n"
"\n"
"package pkg_optab is\n"
"\n"
"type imm_code is (I_NA\n"
"        ,I_U4   ,I_S8   ,I_U8   ,I_U5   ,I_S5   ,I_D9   ,I_D10  ,I_V8);\n"
"\n"
"type opr_code is (O_NA\n"
"        ,O_RJ   ,O_RI   ,O_RS1  ,O_RS2  ,O_R15  ,O_RP   ,O_RD\n"
"        ,O_IMM  ,O_UIMM ,O_PINC ,O_PDEC\n"
"        ,O_MDR  ,O_RWD  ,O_RRD1 ,O_WR);\n"
"\n"
"type spf_code is (S_NA\n"
"        ,S_POPM ,S_PUSHM,S_INT  ,S_JMP  ,S_CALL ,S_BCC  ,S_ADDSP,S_RET\n"
"        ,S_RTI);\n"
"\n"
"type alu_code is (A_NA\n"
"        ,A_AR1 ,A_AR2   ,A_ADD  ,A_LSL  ,A_ASR  ,A_LSR  ,A_SUB  ,A_CMP\n"
"        ,A_LAND ,A_OR   ,A_EOR  ,A_MUL  ,A_MULU ,A_MULH ,A_MULUH\n"
"        ,A_ADDC ,A_NEG  ,A_NOT  ,A_EXTB ,A_EXTH ,A_SXTB ,A_SXTH ,A_DIV\n"
"        ,A_DIVU ,A_MOD  ,A_MODU \n"
"        ,A_ADDF ,A_SUBF ,A_MULF ,A_FLT  ,A_FLTU ,A_FIX  ,A_FIXU, A_SQRT, A_DIVF, A_CMPF);\n"
"\n"
"type optab is record\n"
"        immcd   : imm_code;\n"
"        len     : unsigned(1 downto 0);\n"
"        ex      : unsigned(4 downto 0);\n"
"        spfunc  : spf_code;\n"
"        mode    : unsigned(2 downto 0);\n"
"        mar     : opr_code;\n"
"        ofs     : opr_code;\n"
"        mwe     : opr_code;\n"
"        rrd1    : opr_code;\n"
"        rrd2    : opr_code;\n"
"        rwa     : opr_code;\n"
"        rwd     : opr_code;\n"
"        alu     : alu_code;\n"
"        dl      : boolean;\n"
"end record;\n"
"\n"
"function opcdec(ir : unsigned) return optab;\n"
"\n"
"end pkg_optab;\n"
"\n"
"\n"
"\n"
"library ieee;\n"
"use ieee.std_logic_1164.all;\n"
"use ieee.std_logic_arith.all;\n"
"use ieee.std_logic_unsigned.all;\n"
"use work.pkg_optab.all;\n"
"\n"
"package body pkg_optab is\n"
"\n"
"function opcdec(ir : unsigned) return optab is\n"
"variable op : optab;\n"
"begin\n"
);
	case_gen("immcd");
	case_gen("len");
	case_gen("ex");
	case_gen("spfunc");
	case_gen("mode");
	case_gen("mar");
	case_gen("ofs");
	case_gen("mwe");
	case_gen("rrd1");
	case_gen("rrd2");
	case_gen("rwa");
	case_gen("rwd");
	case_gen("alu");
	case_gen("dl");

printf(
"  return op;\n"
"end;\n"
"\n"
"end pkg_optab;\n"
"\n"
);
return 0;
}



