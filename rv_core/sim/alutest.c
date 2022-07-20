
#include <stdio.h>
#include <stdlib.h>

//typedef enum {
//  A_NA, S2, ADD, SLT, SLTU, XOR, OR, AND, SLL, SRL, SRA, SUB,
//  MUL, MULH, MULHSU, MULHU, DIV, DIVU, REM, REMU,
//  FLD, FST, FMADD, FMSUB, FADD, FSUB, FMUL, FDIV, FSQRT, FSGN, FMIN, FMAX, FCVT
//} alu_t;
#include "../../rvsim/simcore.h"
#include "../../rvsim/optab.h"

void wvec(FILE *fp, alu_t alu, u32 r1, u32 r2, u32 ref)
{
  fprintf(fp, "%d %d %d %d\n", alu, r1, r2, ref);
  printf("%s %d op %d = %d\n", _alu_nam[alu], r1, r2, ref);
}

void wvecf(FILE *fp, alu_t alu, float fr1, float fr2, float fref)
{
  u32 r1 = fu(fr1);
  u32 r2 = fu(fr2);
  u32 ref = fu(fref);
  fprintf(fp, "%d %d %d %d\n", alu, r1, r2, ref);
  printf("%s %g op %g = %g\n", _alu_nam[alu], fr1, fr2, fref);
}

void wvecfi(FILE *fp, alu_t alu, float fr1, float fr2, u32 ref)
{
  u32 r1 = fu(fr1);
  u32 r2 = fu(fr2);
  fprintf(fp, "%d %d %d %d\n", alu, r1, r2, ref);
  printf("%s %g op %g = %d\n", _alu_nam[alu], fr1, fr2, ref);
}

void wvecfloat(FILE *fp, alu_t alu, u32 r1, u32 r2)
{
  u32 ref = r2 ? fu((float)r1) : fu((float)(s32)r1);
  fprintf(fp, "%d %d %d %d\n", alu, r1, r2, ref);
  printf("%s float(%d) %d  = %g\n", _alu_nam[alu], r2, r1, r2 ? (float)r1 : (float)(s32)r1);
}

void wvecfix(FILE *fp, alu_t alu, float fr1, u32 r2)
{
  u32 r1 = fu(fr1);
  fprintf(fp, "%d %d %d %d\n", alu, r1, r2, r2 ? (u32)fr1 : (int)fr1);
  printf("%s fix(%d) %g  = %d\n", _alu_nam[alu], r2, fr1, r2 ? (u32)fr1 : (int)fr1);
}

float sgnj(float fr1, float fr2, int c)
{
  u32 rv;
  u32 r1 = fu(fr1);
  u32 r2 = fu(fr2);
  switch(c){
    case 0: rv = (r1 & 0x7fffffff) | (r2 & 0x80000000);  break;
    case 1: rv = (r1 & 0x7fffffff) | (~r2 & 0x80000000);  break;
    case 2: rv =  r1 ^ (r2 & 0x80000000);  break;
  }
  return uf(rv);
}

int main()
{
  int i;
  alu_t alu = 0;
  FILE *fp = fopen("alutest.vec", "w");
  s32 s1, s2;
  u32 u1, u2;
  int64_t s64;
  float fr1, fr2, fref;

  printf("RAND_MAX: %x\n", RAND_MAX);
  
//add
  wvec(fp, A_NA, 0,0,0);

  fr1 = 0.5;
  fr2 = 1.0;
  for(i = 0; i < 3; i++){
    wvecfi(fp, FLT, fr1, fr2, fr1 < fr2);
    wvecfi(fp, FLE, fr1, fr2, fr1 <= fr2);
    wvecfi(fp, FEQ, fr1, fr2, fr1 == fr2);
    fr1 += 0.5;
  }
  fr1 = -0.5;
  fr2 = 1.0;
  for(i = 0; i < 3; i++){
    wvecfi(fp, FLT, fr1, fr2, fr1 < fr2);
    wvecfi(fp, FLE, fr1, fr2, fr1 <= fr2);
    wvecfi(fp, FEQ, fr1, fr2, fr1 == fr2);
    fr1 += 0.5;
  }
  fr1 = -0.5;
  fr2 = -1.0;
  for(i = 0; i < 3; i++){
    wvecfi(fp, FLT, fr1, fr2, fr1 < fr2);
    wvecfi(fp, FLE, fr1, fr2, fr1 <= fr2);
    wvecfi(fp, FEQ, fr1, fr2, fr1 == fr2);
    fr1 -= 0.5;
  }

  wvecf(fp, FSGNJ, fr1, fr2, sgnj(fr1, fr2, 0));
  wvecf(fp, FSGNJN, fr1, fr2, sgnj(fr1, fr2, 1));
  wvecf(fp, FSGNJX, fr1, fr2, sgnj(fr1, fr2, 2));
  fr1 = 1.0;
  wvecf(fp, FSGNJ, fr1, fr2, sgnj(fr1, fr2, 0));
  wvecf(fp, FSGNJN, fr1, fr2, sgnj(fr1, fr2, 1));
  wvecf(fp, FSGNJX, fr1, fr2, sgnj(fr1, fr2, 2));
  fr2 = 1.0;
  wvecf(fp, FSGNJ, fr1, fr2, sgnj(fr1, fr2, 0));
  wvecf(fp, FSGNJN, fr1, fr2, sgnj(fr1, fr2, 1));
  wvecf(fp, FSGNJX, fr1, fr2, sgnj(fr1, fr2, 2));
  fr1 = -1.0;
  wvecf(fp, FSGNJ, fr1, fr2, sgnj(fr1, fr2, 0));
  wvecf(fp, FSGNJN, fr1, fr2, sgnj(fr1, fr2, 1));
  wvecf(fp, FSGNJX, fr1, fr2, sgnj(fr1, fr2, 2));

  fr1 = 1.0;
  fr2 = 2.0;
  for(i = 0; i < 50; i++){
    wvecf(fp, FADD, fr1, fr2, fr1 + fr2);
    wvecf(fp, FSUB, fr1, fr2, fr1 - fr2);
    wvecf(fp, FMUL, fr1, fr2, fr1 * fr2);
    wvecf(fp, FDIV, fr1, fr2, fr1 / fr2);
    wvecf(fp, FMIN, fr1, fr2, fr1 < fr2 ? fr1 : fr2);
    wvecf(fp, FMAX, fr1, fr2, fr1 > fr2 ? fr1 : fr2);

    wvecfi(fp, FLT, fr1, fr2, fr1 < fr2);
    wvecfi(fp, FLE, fr1, fr2, fr1 <= fr2);
    wvecfi(fp, FEQ, fr1, fr2, fr1 == fr2);

    wvecfloat(fp, FLOAT, (int)(-fr1*1000), 0); // signed
    wvecfloat(fp, FLOAT, (int)(fr1*1000), 0); // signed
    wvecfloat(fp, FLOAT, (int)(fr1*1000), 1); // unsigned
    wvecfloat(fp, FLOAT, (int)(-fr1*1000), 1); // unsigned

    wvecfix(fp, FIX, (fr1*1000), 0); // signed
    wvecfix(fp, FIX, (-fr1*1000), 0); // signed
    wvecfix(fp, FIX, (fr1*1000), 1); // unsigned
    wvecfix(fp, FIX, (-fr1*1000), 1); // unsigned

    fr1 *= 0.99;
    fr2 *= 0.9;
  }
  fr1 = 1.0;
  fr2 = 2.0;
  for(i = 0; i < 50; i++){
    wvecf(fp, FADD, fr1, fr2, fr1 + fr2);
    wvecf(fp, FSUB, fr1, fr2, fr1 - fr2);
    fr1 *= 0.2;
    fr2 *= 0.3;
  }
  for(i = 0; i < 50; i++){
    s1 = random() + random();
    s2 = random() + random();
    u1 = s1;
    u2 = s2;
    wvec(fp, ADD, u1, u2, u1+u2);
  }
  for(i = 0; i < 50; i++){
    s1 = random() + random();
    s2 = random() + random();
    u1 = s1;
    u2 = s2;
    wvec(fp, SUB, u1, u2, u1-u2);
  }

  for(i = 0; i < 10; i++){
    s1 = random() + random();
    s2 = random() + random();
    u1 = s1;
    u2 = s2;
//MULH, MULHSU, MULHU
    wvec(fp, MUL, u1, u2, u1*u2);
    wvec(fp, MUL, s1, u2, s1*u2);
    wvec(fp, MUL, s1, s2, s1*s2);
    wvec(fp, MULH, s1, s2, ((int64_t)s1*s2)>>32);
    wvec(fp, MULHSU, s1, u2, ((int64_t)s1*u2)>>32);
    wvec(fp, MULHU, u1, u2, ((uint64_t)u1*u2)>>32);
  }

  for(i = 0; i < 10; i++){
    s1 = random() + random();
    s2 = random() + random();
    u1 = s1;
    u2 = s2;
    s2 >>= i/10;
    u2 >>= i/10;
    wvec(fp, DIV, s1, s2, s1/s2);
    wvec(fp, ADD, s1, s1/s2, s1+s1/s2);
    wvec(fp, REM, s1, s2, s1%s2);
    wvec(fp, ADD, s1, s1%s2, s1+s1%s2);
    wvec(fp, DIVU, u1, u2, u1/u2);
    wvec(fp, SUB, u1, u1/u2, u1-u1/u2);
    wvec(fp, REMU, u1, u2, u1%u2);
    wvec(fp, SUB, u1, u1%u2, u1-u1%u2);
  }

  fclose(fp);
}

