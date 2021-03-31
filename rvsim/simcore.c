//
// simcore.c
// rvsim simulation core
//
//#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include <bfd.h>
#include "simcore.h"
#include "optab.h"
#include "c-optab.h"

#include "consio.h"

extern bfd_byte 	*memory;
extern bfd_byte 	*stack;
extern bfd_size_type 	memsize;
extern bfd_size_type	stacksize;
extern bfd_vma 		vaddr;
extern bfd_vma		stack_top;

extern int view_reg[];
extern int nview;
extern int sys_exit;
extern int arch;
extern int Nregs;

extern int n_break;
extern u32 break_adr[];
extern int break_en[];


//struct _reg32 { u32 d; u32 q; };
typedef struct _reg32 { u32 d; u32 q; }	reg32;
typedef struct _reg16 { u16 d; u16 q; }	reg16;
typedef struct _regalu { alu_t d; alu_t q; }	regalu;
typedef struct _regint { int d; int q; }    regint;

static reg32 R[NREG];
static reg32 pc, pc1, bdst;	// 18
static reg32 ir, irh;
static reg32 mar, mdr, _mdr, mdw;
static reg16 mmd, mwe;	// mem-mode, mem-we
static reg32 rrd1, rrd2;
static regint rwa[3], rwd[3];	// reg-wadr, reg-wr-data
static reg32 rwdat[3];	// alu out data
static regalu alu;	// alu mode

static reg16 bra_stall = {0,0};	// branch stall
static reg16 ex_stall = {0,0};	// exec stall
static reg16 d_stall = {0,0};	// data stall

#define reset(r)	{r.d = r.q = 0;}
#define latch(r)	{r.q = r.d;}

//static int drw_chk = 0;
u32 insncount = 0;

FILE *debfp_reg = NULL;
FILE *debfp_mem = NULL;

void clock()
{
    int i;

    //drw_chk = 0;

    latch(ir);
    latch(irh);

    latch(mar);
    latch(mdr);
    latch(_mdr);
    latch(mdw);
    latch(mmd);
    latch(mwe);
    latch(rrd1);
    latch(rrd2);
    latch(alu);
    latch(rwa[0]);
    latch(rwa[1]);
    latch(rwa[2]);
    latch(rwd[0]);
    latch(rwd[1]);
    latch(rwd[2]);
    rwa[2].d = rwa[1].q;	rwa[1].d = rwa[0].q;
    rwd[2].d = rwd[1].q;	rwd[1].d = rwd[0].q;
    //	latch(rwdat[0]);
    latch(rwdat[1]);
    latch(rwdat[2]);
    rwdat[2].d = rwdat[1].q;

    for(i = 0; i < NREG; i++) latch(R[i]);

    latch(pc);
    latch(pc1);
    latch(bdst);
    pc1.d = pc.q;

    latch(bra_stall);
    if(!d_stall.d){
        latch(ex_stall);
        ex_stall.d = ex_stall.q > 0 ? ex_stall.q - 1 : 0;
    }
    latch(d_stall);
    d_stall.d = 0;

}

void fetch()
{
    int a;

    a = pc.q - (int)vaddr;
    if(a < 0 || a >= memsize){
        printf("%-8d: ill memory address : pc=%x  vaddr:%x a:%d\n", insncount, pc.q, (int)vaddr, a);
        return;
    }
    ir.d = (u32)((memory[a+3]<<24)|(memory[a+2]<<16)|(memory[a+1]<<8)|memory[a]);

    if(pc.d == (u32)-1){
        pc.d = pc.q + (isCinsn(ir.d) ? 2 : 4);
    }
}

void illinsn()
{
}

#define iRd(ir)		(((ir)>>7)&0x1f)
#define iRs1(ir)	(((ir)>>15)&0x1f)
#define iRs2(ir)	(((ir)>>20)&0x1f)

u32 Reg(int ix)
{
    if(ix <= 0) return 0;
    if(ix < NREG) return R[ix].q;
    return 0;
}
u32 Reg_fwd(int ix) // Register read with fowarding
{
    u32 rd = Reg(ix);
    if(ix == 0) return 0;
    if(rwa[0].q == ix){
        if(rwd[0].q == ALU) rd = rwdat[1].d;
        else if(rwd[0].q == MDR) d_stall.d = 1;
    }else if(rwa[1].q == ix){
        if(rwd[1].q == ALU) rd = rwdat[1].q;
        else if(rwd[1].q == MDR) d_stall.d = 1;
    }else if(rwa[2].q == ix){
        if(rwd[2].q == ALU) rd = rwdat[2].q;
        else if(rwd[2].q == MDR) rd = mdr.q;
    }
    return rd;
}

const char *regnam[] = 
// 0    1    2    3    4    5    6    7    8    9   10    11    12   13   14   15
{"x0","ra","sp","gp","tp","t0","t1","t2","s0","s1","a0", "a1", "a2","a3","a4","a5",
//16   17   18   19   20   21   22   23   24   25   26    27    28   29   30   31
 "a6","a7","s2","s3","s4","s5","s6","s7","s8","s9","s10","s11","t3","t4","t5","t6"};

const char *fregnam[] =
{"ft0","ft1","ft2","ft3","ft4","ft5","ft6","ft7","fs0","fs1", "fa0", "fa1","fa2","fa3","fa4","fa5",
        "fa6","fa7","fs2","fs3","fs4","fs5","fs6","fs7","fs8","fs9","fs10","fs11","ft3","ft4","ft5","ft6"};

void Reg_wr(int ix, u32 data)
{
    if(ix <= 0) return ;
    if(ix < NREG) R[ix].d = data;
    if(debfp_reg){
        if(ix < NREG){
            char *sym = search_symbol(data);
            fprintf(debfp_reg, "%5d %2d %s %x %s\n", insncount, ix, regnam[ix], data, sym ? sym : "");
        }
    }
    return ;
}

const char *Reg_nam(int ix, ex_t ex)
{
    if(ix == NA) return "--";
    else if(ix < NREG) return ex != F ? regnam[ix] : fregnam[ix];
    else return "??";
}

int immdata(u32 ir, itype_t type)
{
    int imm = 0x0;
    u32 sxt = (ir&0x80000000) ? 0xffffffff : 0x0;
    switch(type){
    case type_I:  imm = ((s32)ir >> 20);	break;
    case type_S:  imm = ((ir>>7)&0x1f) | ((ir>>20)&0xfe0) | (sxt&0xfffff000);	break;
    case type_SB: imm = ((ir>>7)&0x1e) | ((ir>>20)&0x7e0) | ((ir<<4)&0x800) | (sxt&0xfffff000);	break;
    case type_U:  imm = (ir&0xfffff000);	break;
    case type_UJ: imm = ((ir>>20)&0x7fe) | ((ir>>9)&0x800) | (ir&0xff000) | (sxt&0xfff00000);	break;
    case type_R:  imm = 0;	break;
    case type_RF: imm = 0;	break;
    }
    return imm;
}


int print_optab()
{
    int i;
    for(i = 0; i < Nopc; i++){
        printf("%s, %x\n", optab[i].mnemonic, optab[i].opc);
    }
    return 0;
}

int optab_search(u32 ir)
{
    int i;
    for(i = 0; i < Nopc; i++){
        //printf("%8x/%x,",ir,optab[i].opc);
        if((ir&0x7f) == optab[i].opc){
            if(optab[i].func3 != __){
                if(((ir>>12)&0x7) == optab[i].func3){
                    if(optab[i].func7 != __){
                        if(((ir>>25)&0x7f) == optab[i].func7){
                            break;
                        }
                    }else{
                        break;
                    }
                }
            }else{
                break;
            }
        }
    }
    if(i == Nopc) return -1;
    return i;
}

struct {
    char *mnemonic;
    u32 exir;
    u32 imm;
} cop;

u32 expand_c_insn(u32 ir)
{
    int i;
    u32 exir = 0x0;
    u32 imm = 0x0;
    s32 simm = 0;
#define sext(m) (simm = ((simm << (31-m)) >> (31-m)))
    for(i = 0; i < Ncopc; i++){
        if((ir & c_optab[i].mask) == c_optab[i].opcode){
            cop.mnemonic = c_optab[i].mnemonic;
            switch(c_optab[i].imm){
            case CIMM1: imm = ((ir >> 7) & 0x3c) | ((ir >> 1) & 0xc0);  break;
            case CIMM2: imm = ((ir >> 2) & 0x1c) | ((ir << 4) & 0xc0) | ((ir >> 7) & 0x20); break;
            case CIMM3: simm = ((ir << 1) & 0xc0) | ((ir >> 2) & 0x6) | ((ir << 3) & 0x20)
                    | ((ir >> 7) & 0x18) | ((ir >> 4) & 0x100);
                    sext(8); break;
            case CIMM4: imm = ((ir >> 2) & 0x1f) | ((ir >> 7) & 0x20);
                    exir |= imm << 20; break;  // shmat
            case CIMM5: simm = ((ir >> 1) & 0xb40) | ((ir >> 2) & 0xe) | ((ir << 3) & 0x20)
                    | ((ir << 1) & 0x80) | ((ir << 2) & 0x400) |((ir >> 7) & 0x10);
                    sext(11);   break;
            case CIMM6: simm = ((ir << 10) & 0x1f000) | ((ir << 5) & 0x20000);
                    sext(17);   break;
            case CIMM7: simm = ((ir >> 3) & 0x200) | ((ir << 4) & 0x180) | ((ir << 1) & 0x40)
                    | ((ir >> 2) & 0x10) | ((ir << 3) & 0x20);
                    sext(9);    break;
            case CIMM8: imm = ((ir >> 7) & 0x38) | ((ir >> 4) & 0x4) | ((ir << 1) & 0x40);  break;
            case CIMM9: imm = ((ir >> 7) & 0x30) | ((ir >> 1) & 0x3c0) | ((ir >> 4) & 0x4)
                    | ((ir >> 2) & 0x8); break;
            case CIMM10: simm = ((ir >> 2) & 0x1f) | ((ir >> 7) & 0x20);
                    sext(5);    break;
            default: break;
            }
            switch(c_optab[i].rs1){
            case CX0: break;
            case CX2: exir |= 0x2 << 15;    break;
            case CRS1D: exir |= (((ir << 8) & 0x38000) | 0x40000);    break;
            case CRS1:  exir |= ((ir << 8) & 0xf8000); break;
            default: break;
            }
            switch(c_optab[i].rs2){
            case CX0: break;
            case CRS2D: exir |= (((ir << 18) & 0x700000) | 0x800000);   break;
            case CRS2: exir |= ((ir << 18) & 0x1f00000); break;
            default: break;
            }
            switch(c_optab[i].rd){
            case CX0: break;
            case CX1:   exir |= 0x80; break;
            case CX2:   exir |= 0x100; break;
            case CRS1:  exir |= (ir & 0xf80); break;
            case CRS1D: exir |= ((ir & 0x380) | 0x400);  break;
            case CRS2D: exir |= (((ir << 5) & 0x380) | 0x400); break;
            case CRS2:  exir |= (ir & 0xf80);  break;
            default: break;
            }
            if(c_optab[i].func7 != __) exir |= c_optab[i].func7 << 25;
            if(c_optab[i].func3 != __) exir |= c_optab[i].func3 << 12;
            if(c_optab[i].opc != __) exir |= c_optab[i].opc;
            break;
        }
    }
    cop.imm = imm | simm;
    cop.exir = exir;
//    if(!strcmp(cop.mnemonic, "c.slli")) printf("%s %d %x\n",cop.mnemonic,imm,exir );
    return exir;
}

int disasm(int adr, char *dat, char *opc, char *opr, int *dsp)
{
    int i, imm, a, isC;
    ex_t ex;
    const char *Rd, *Rs1, *Rs2;
    u32 ir;
    a = adr - vaddr;
    ir = (u32)((memory[a+3]<<24)|(memory[a+2]<<16)|(memory[a+1]<<8)|memory[a]);
    isC = isCinsn(ir);
    if(isC){
        sprintf(dat, "    %04x ", ir&0xffff);
        ir = expand_c_insn(ir);
        //       sprintf(&dat[5], "%08x ", ir);
    }else{
        sprintf(dat, "%08x ", ir);
    }
    i = optab_search(ir);
    if(i < 0){
        sprintf(opc, "***");
        sprintf(opr, " ");
        return isC ? 2 : 4;
    }else{
        if(isC){
            imm = cop.imm;
            sprintf(opc, "%-7s", cop.mnemonic);
        }else{
            imm = immdata(ir, optab[i].type);
            sprintf(opc, "%-7s", optab[i].mnemonic);
        }
        ex = optab[i].ex;
        //	ofs = regofs(optab[i].mode, optab[i].ofs, optab[i].len>1 ? ir[1] : imm);

        Rd  = Reg_nam(iRd(ir), ex);
        Rs1 = Reg_nam(iRs1(ir), ex);
        Rs2 = Reg_nam(iRs2(ir), ex);
        //        sprintf(opc, "%-6s", optab[i].mnemonic);
        *dsp = -1;
        switch(optab[i].type){
        case type_R:  if(optab[i].rrd2 == SHAMT) sprintf(opr, "%s,%s,%d", Rd, Rs1, iRs2(ir));
        else sprintf(opr, "%s,%s,%s", Rd, Rs1, Rs2);
        break;
        case type_RF: sprintf(opr, "%s,%s,%s", Rd, Rs1, Rs2);	break;
        case type_I:  sprintf(opr, "%s,%s,%d", Rd, Rs1, imm);	break;
        case type_S:  sprintf(opr, "%s,%s,%d", Rs1, Rs2, imm);	break;
        case type_SB: sprintf(opr, "%s,%s,(%x)", Rs1, Rs2, adr+imm); *dsp = adr + imm;	break;
        case type_U:  sprintf(opr, "%s,%d", Rd, imm);	break;
        case type_UJ: sprintf(opr, "%s,(%x)", Rd, adr+imm);	*dsp = adr + imm; break;
        }
    }
    return isC ? 2 : 4;
}

u32 bra_dest(optab_t *op, u32 rs1, u32 rs2, u32 imm, u32 bdst, int pcinc)
{
    int bra = 0;
    u32 pc_nxt = -1;
    if(op->pc == BRA){
        switch(op->func3){
        case 0: if(rs1 == rs2) bra = 1; break;  // beq
        case 1: if(rs1 != rs2) bra = 1; break;  // bne
        case 4: if((s32)rs1 < (s32)rs2) bra = 1; break; // blt
        case 5: if((s32)rs1 >= (s32)rs2) bra = 1; break;// bge
        case 6: if(rs1 < rs2) bra = 1; break;   // bltu
        case 7: if(rs1 >= rs2) bra = 1; break;  // bgeu
        }
        if(bra){
            //            pc_nxt = pc1.q + imm;
            pc_nxt = bdst;
            bra_stall.d = 1;
        }else{
            //    pc_nxt = pc.q + pcinc;
        }
    }else if(op->pc == JMP){
        if(op->rrd1 == RS1)
            pc_nxt = (rs1 + imm) & ~0x1;
        else
            //            pc_nxt = pc1.q + imm;
            pc_nxt = bdst;
        bra_stall.d = 1;
    }else{
        //    pc_nxt = pc.q + pcinc;
    }
    return pc_nxt;
}

int sys_func = 17;	// a7

void reset_pc(u32 adr)
{
    pc.d = adr;
    pc1.d = adr;
    ir.d = 0xffffffff;
    rwa[0].d = rwa[1].d = rwa[2].d = NA;
    rwd[0].d = rwd[1].d = rwd[2].d = NA;
    mwe.d   = NA;
    bra_stall.d = 0;
    R[2].d = stack_top; // x2 == stack pointer
    sys_func = arch == 'e' ? 5 : 17;	// t0 : a7
    Nregs = arch == 'e' ? 16 : 32;
    printf("reset_pc()\n");
}

char mem_rd(int adr);

#include "syscall.h"
extern u32 heap_ptr;

u32 syscall(u32 func, u32 a0, u32 a1, u32 a2){
    u32 rv = 0;
    int i;
    extern u32 _end_adr;
    //printf(" ecall %x %x %x %x\n", func, a0, a1, a2);
    switch(func){
//    case SYS_fstat:
//        break;
    case SYS_exit:
        sys_exit = 1;
        printf("exit(): %d\n", a0);
        break;
    case SYS_write:
        if (a2 > 256) a2 = 256;
        for(i = 0; i < a2; i++){
            fputc(mem_rd(a1 + i), stderr);
        }
        rv = i;
//        printf("write(%x %x %x) : %x\n", a0,a1,a2,rv);
        break;
    case SYS_brk:
        if(!a0) heap_ptr = _end_adr;
        else    heap_ptr = a0;
        rv = heap_ptr;
//        printf("sbrk %x %x %x (%x %x)\n", rv, heap_ptr, _end_adr, a0, a1);
        break;
//    case SYS_close:
//        break;
    default:
        printf(" ecall %d %x %x %x\n", func, a0, a1, a2);
        break;
    }
    return rv;
}

void decode()
{
    int i, pcinc;
    u32 IR, imm;

    //    pc.d = pc.q + 4;
    rwa[0].d = NA;
    rwd[0].d = NA;
    mwe.d   = NA;
    IR = ir.q;

    if(ex_stall.q || d_stall.q) IR = irh.q;
    else	irh.d = ir.q;

    int isC = isCinsn(IR);
    pcinc = isC ? 2 : 4;
    //    pc.d = pc.q + pcinc;
    pc.d = -1;

    if(isC){
        IR = expand_c_insn(IR);
    }
    // optab search
    i = optab_search(IR);
    if(isC){
        imm = cop.imm;
    }else{
        imm = immdata(IR, optab[i].type);
    }
    if(i < 0){
        illinsn();
    }else{
        if(bra_stall.q){
            bra_stall.d = 0;
            return ;
        }
#if 1
        if(ex_stall.q == 0){
            if(optab[i].excyc > 0){ // multi cycle op
                ex_stall.d = optab[i].excyc;
                pc.d = pc.q;	// pc hold
                return;
            }
        }else if(ex_stall.q){
            pc.d = pc.q;	// pc hold
            if(ex_stall.q > 1) return;
            else pc.d = pc.q + pcinc;
        }
#endif
        // rrd1
        rrd1.d = (optab[i].rrd1 == PC) ? pc1.q :
                (optab[i].rrd1 == X0) ? 0 :
                        Reg_fwd(iRs1(IR));
        // rrd2
        rrd2.d = (optab[i].rrd2 == IMM) ? imm :
                (optab[i].rrd2 == INC) ? pc1.q + pcinc :
                        (optab[i].rrd2 == SHAMT) ? iRs2(IR) :
                                Reg_fwd(iRs2(IR));
        // forward logic here!
        // mar
        mar.d = d_stall.d ? NA : (optab[i].mar == RS1 ? rrd1.d + imm : 0x0);
        mdw.d = d_stall.d ? NA : (optab[i].mwe == WE ?  rrd2.d : 0);
        // rwa
        rwa[0].d = d_stall.d ? NA : optab[i].rwa == RD ? iRd(IR) : 0;
        // pc
        bdst.d = d_stall.q ? bdst.q : pc1.q + imm;
        pc.d =  d_stall.d ? pc.q : bra_dest(&optab[i], rrd1.d, rrd2.d, imm, bdst.d, pcinc);

        mmd.d 	 = d_stall.d ? NA : optab[i].mode;
        mwe.d 	 = d_stall.d ? NA : optab[i].mwe;
        rwd[0].d = d_stall.d ? NA : optab[i].rwd;
        alu.d	 = d_stall.d ? __ : optab[i].alu;
//        if(optab[i].opc == 0x73 && optab[i].func3 == 0){
        if(optab[i].ex == E){
            u32 rv = syscall(Reg_fwd(sys_func),Reg_fwd(10),Reg_fwd(11),Reg_fwd(12));
            Reg_wr(10, rv);
        }
    }

}

// RAM emulation

char mem_rd(int adr)
{
    //    int stk = mar.q;
    bfd_byte *mem;

    if((adr >= (u32)vaddr && adr < ((u32)vaddr + memsize))		// data/bss area
            || (adr > stack_top-stacksize && adr <= stack_top)){	// stack area

        if(!(adr < ((u32)vaddr + memsize))){
            adr -= stack_top-stacksize;
            mem = stack;
        }else{
            adr -= (u32)vaddr;
            mem = memory;
        }
        return mem[adr];
    }
    return 0;
}

void RAM_access(int wr)
{
    int adr = mar.q - (u32)vaddr;
    int stk = mar.q;
    bfd_byte *mem;

    if((adr >= 0 && adr < memsize)		// data/bss area
            || (stk > stack_top-stacksize && stk <= stack_top)){	// stack area

        if(!(adr < memsize)){
            adr = stk - (stack_top-stacksize);
            mem = stack;
        }else{
            mem = memory;
        }
        if(wr == WE){   // lsb first
            u32 mask = 0;
            char *wmd = "";
            switch(mmd.q){
            case SI: adr &= ~3;
              mask = 0xffffffff;
              wmd = "SI";
              mem[adr++] = mdw.q;
              mem[adr++] = mdw.q>>8;
              mem[adr++] = mdw.q>>16;
              mem[adr] = mdw.q>>24;	break;
            case HI: adr &= ~1;
              mask = 0xffff;
              wmd = "HI";
              mem[adr++] = mdw.q;
              mem[adr] = mdw.q>>8;	break;
            case QI:
              mask = 0xff;
              wmd = "QI";
              mem[adr] = mdw.q;	break;
            }
            if(debfp_mem){
                char *sym = search_symbol(mar.q);
                char *symd = mmd.q == SI ? search_symbol(mdw.q) : NULL;
                fprintf(debfp_mem, "%6d %8x %x %s %s %s\n",
                        insncount, mar.q, mdw.q & mask, wmd, sym ? sym : "-", symd ? symd : "-");
            }
        }else if(wr == RE){	// memory read
            switch(mmd.q){
            case SI: adr &= ~3;
            _mdr.d =  mem[adr++];
            _mdr.d |= mem[adr++]<<8;
            _mdr.d |= mem[adr++]<<16;
            _mdr.d |= mem[adr]<<24;	break;
            case HI: adr &= ~1;
            _mdr.d =  mem[adr++];
            _mdr.d |= mem[adr]<<8;	break;
            case SHI: adr &= ~1;
            _mdr.d =  mem[adr++];
            _mdr.d |= mem[adr]<<8;
            _mdr.d = (s32)((s16)_mdr.d);
            break;
            case QI:
                _mdr.d =  mem[adr];	break;
            case SQI:
                _mdr.d =  (s32)((s8)mem[adr]);
                break;            }
        }
    }else if((wr == WE)||(wr == RE)){
        printf("%-8d: ill memory address : mar=%x  %s\n", insncount, mar.q, wr == WE ? "WE" : "RE");
    }
    return ;
}

void exec()
{
    RAM_access(mwe.q);
    // ALU operation
    switch(alu.q){	// rrd1 op rrd2
    case CMP:   break;
    case ADD:   rwdat[1].d = rrd1.q + rrd2.q;  break;
    case S2:    rwdat[1].d = rrd2.q;	break;
    case SLT:   rwdat[1].d = (s32)rrd1.q < (s32)rrd2.q;	break;
    case SLTU:  rwdat[1].d = rrd1.q < rrd2.q;	break;
    case XOR:   rwdat[1].d = rrd1.q ^ rrd2.q;	break;
    case OR:    rwdat[1].d = rrd1.q | rrd2.q;	break;
    case AND:   rwdat[1].d = rrd1.q & rrd2.q;	break;
    case SLL:   rwdat[1].d = rrd1.q << (rrd2.q & 0x3f);	break;
    case SRL:   rwdat[1].d = rrd1.q >> (rrd2.q & 0x3f);	break;
    case SRA:   rwdat[1].d = (s32)rrd1.q >> (rrd2.q & 0x3f);	break;
    case SUB:   rwdat[1].d = rrd1.q - rrd2.q;	break;
    case MUL:   rwdat[1].d = rrd1.q * rrd2.q;	break;
    case MULH:  rwdat[1].d = ((s64)rrd1.q * rrd2.q) >> 32;	break;
    case MULHSU: rwdat[1].d = ((s64)rrd1.q * (s32)rrd2.q) >> 32;	break;
    case MULHU:  rwdat[1].d = ((u64)rrd1.q * rrd2.q) >> 32;	break;
    case DIV:   rwdat[1].d = (s32)rrd1.q / (s32)rrd2.q;	break;
    case DIVU:  rwdat[1].d = rrd1.q / rrd2.q; 	break;
    case REM:   rwdat[1].d = (s32)rrd1.q % (s32)rrd2.q;	break;
    case REMU:  rwdat[1].d = rrd1.q % rrd2.q;	break;
    case FLD:
    case FST:
    case FMADD:
    case FMSUB:
    case FADD:
    case FSUB:
    case FMUL:
    case FDIV:
    case FSQRT:
    case FSGN:
    case FMIN:
    case FMAX:
    case FCVT:
        //    case __: break;
    default :	rwdat[1].d = 0;
    //    printf("ill ALU operation %d.\n", alu.q);	break;
    }
    if(rwd[1].d < 0) rwdat[1].d = 0;
}

void memac()
{
    mdr.d = _mdr.q;
}

void wback()
{
    if(rwa[2].q >= 0){
        if(rwd[2].q == ALU){
            Reg_wr(rwa[2].q, rwdat[2].q);
        }else if(rwd[2].q == MDR){
            Reg_wr(rwa[2].q, mdr.q);
        }
    }
}

char *wrstr()
{
    static char str[20];
    switch(mwe.q){
    case RE:	return "read";
    case WE:	sprintf(str, "%8x", mdw.q);	return str;
    default: return "--";
    }
}

u32 b_st_cnt = 0;
u32 d_st_cnt = 0;
u32 x_st_cnt = 0;

extern FILE *ofp;

void print_regs_label()
{
    int n;
    fprintf(ofp, "   cnt     pc:       ir                               mar      mdr      mdw "
            "    rrd1     rrd2  alu rwa rwd    rwdat ");
    for(n = 0; n < nview; n++){
        fprintf(ofp, "    %5s", Reg_nam(view_reg[n], I));
    }
    fprintf(ofp, "\n");
}

const char *alu_nam(u32 alu)
{
    if(alu < sizeof(_alu_nam)/sizeof(char*)) return _alu_nam[alu];
    else return "xx";
}
const char *regs_nam(u32 reg)
{
    if(reg+1 < sizeof(_regs_nam)/sizeof(char*)) return _regs_nam[reg+1];
    else return "xx";
}

void print_regs()
{
    int n, dsp;
    u32 Ir;
    char opc[80], dat[80], opr[80];

    sprintf(opr, " ");

    if(bra_stall.q)     sprintf(opc, "-b-");
    else if(ex_stall.q||d_stall.q)
        sprintf(opc, "-%c%c-", ex_stall.q?'x':' ', d_stall.q?'d':' ');
    else 		    disasm(pc1.q, dat, opc, opr, &dsp);

    Ir = (ex_stall.q || d_stall.q) ? irh.q : ir.q;

    if(isCinsn(Ir)){
        fprintf(ofp, "%6d %6x:     %04x", insncount, pc1.q, Ir & 0xffff);
    }else{
        fprintf(ofp, "%6d %6x: %08x", insncount, pc1.q, Ir);
    }
    fprintf(ofp,
            " %-8s %-15s %8x %8x %8s"
            " %8x %8x %4s %3s %3s %8x "
            , opc, opr, mar.q, mdr.q, wrstr()
            , rrd1.q, rrd2.q, alu_nam(alu.q), Reg_nam(rwa[0].q, I), regs_nam(rwd[0].q)
            , rwdat[1].q);

    for(n = 0; n < nview; n++) fprintf(ofp, " %8x", Reg(view_reg[n]));
    fprintf(ofp, "  \n");
}

void reg_dump()
{
    int i;
    for(i = 1; i < Nregs; i++){
        fprintf(ofp, "%3s(%8x) ", Reg_nam(i, I), Reg(i));
        if((i-1) % 11 == 10) fprintf(ofp, "%s\n", ELEOL);
    }
    fprintf(ofp, "%s\n", ELEOL);
}

void stat_clear()
{
    insncount = b_st_cnt = x_st_cnt = d_st_cnt = 0;
}

void stat_count()	// statistics count
{
    insncount++;
    if(bra_stall.q)	    b_st_cnt++;
    else if(ex_stall.q) x_st_cnt++;
    else if(d_stall.q)  d_st_cnt++;
}

void stat_print()
{
    fprintf(ofp, "\nTotal clock cycles: %d   stall - bra: %d [%4.1f%%]  data: %d [%4.1f%%]  exec: %d [%4.1f%%]\n"
            , insncount
            , b_st_cnt, b_st_cnt*100.0/insncount
            , d_st_cnt, d_st_cnt*100.0/insncount
            , x_st_cnt, x_st_cnt*100.0/insncount);
}

void simcyc()
{
    wback();
    memac();
    exec();
    decode();
    fetch();

    clock();

    stat_count();
}

int break_chk()
{
    int i;
    for(i = 0; i < n_break; i++){
        if(!bra_stall.q && break_en[i] && break_adr[i] == pc1.q){
            printf("***** break @ %8x\n", pc1.q);
            return 1;
        }
    }
    return 0;
}

u32 simrun (u32 addr, int steps, int reset)
{
    int i, c;

    if(reset){
        stat_clear();
        reset_pc(addr);
        clock();
    }
    initerm();
    for(i = 0; steps < 0 || i < steps; i++){
        simcyc();
        if(break_chk() || sys_exit) break;
        if(!chkrdy()){
            c = inchr();
            if(c == 'q') break;
            if(c == 'r') reg_dump();
        }
    }
    stat_print();
    deinitrm();

    return insncount;
}

//extern int LPP;
//extern int CPL;

u32 simtrace (u32 addr, int steps, int reset)
{
    int i, c;
//    debfp = fopen("regtrace.log","w");

    if(reset){
        stat_clear();
        reset_pc(addr);
        clock();
    }
    initerm();
    get_wsize();

    if(!steps) steps = LPP-5;

    print_regs_label();
    for(i = 0; steps < 0 || i < steps; i++){
        print_regs();
        simcyc();
        if(break_chk() || sys_exit){
            deinitrm();
            return insncount;
        }
    }
    print_regs_label();
    point(0, 0);
    reg_dump();
    point(0, LPP - 1);

    do{
        int ncyc = 0;

        fflush(ofp);
        c = inchr();
        if(isdigit(c)){
            do{
                putchar(c);
                ncyc = ncyc*10 + c - '0';
                c = inchr();
            }while(isdigit(c));
            for(i = 0; i < ncyc-LPP; i++){
                simcyc();
                if(break_chk() || sys_exit){
                    stat_print();
                    deinitrm();
                    return insncount;
                }
            }
            c = ' ';
        }
        switch(c){
        case '\n':
            point(0, LPP - 2);
            print_regs();
            simcyc();
            if(break_chk() || sys_exit) break;
            print_regs_label();
            point(0, 0);
            reg_dump();
            point(0, LPP - 1);
            break;
        case 'c':
        case ' ':
            point(0, LPP - 2);
            for(i = 0; i < LPP-6; i++){
                print_regs();
                simcyc();
                if(break_chk() || sys_exit) break;
            }
            print_regs_label();
            point(0, 0);
            reg_dump();
            point(0, LPP - 1);
            break;
        }
        if(break_chk() || sys_exit) break;
    }while(c != 'q');

    stat_print();
    deinitrm();
    return insncount;
}


