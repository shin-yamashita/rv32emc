
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

typedef uint64_t  u64;
typedef uint32_t  u32;
typedef uint16_t  u16;
typedef uint8_t   u8;

typedef int64_t  s64;
typedef int32_t  s32;
typedef int16_t  s16;
typedef int8_t   s8;

typedef union {float f; u32 u;} fu_t;
#define fu(x)   ((fu_t)(x)).u
#define uf(x)   ((fu_t)(x)).f

#include "../../rvsim/optab.h"

#include "dpi.h"

int alu_check(int alu, int r1, int r2 ,int rwd, int ref)	//
{
  float err;
  switch(alu){
    case FADD:
    case FSUB:
    case FMUL:
    case FDIV:
    case FMIN:
    case FMAX:  // float op float -> float
      err = uf((u32)rwd) - uf((u32)ref);
      printf("%-6s %11g %11g %11g %11g err: %g\n", _alu_nam[alu], uf((u32)r1), uf((u32)r2), uf((u32)rwd), uf((u32)ref), err / uf((u32)ref) );
      break;
    case FLT:
    case FLE:
    case FEQ:
      printf("%-6s %11g %11g %11d %11d err: %d\n", _alu_nam[alu], uf((u32)r1), uf((u32)r2), rwd, ref, rwd - ref);
      break;
    case FSGNJ:
    case FSGNJN:
    case FSGNJX:
      err = uf((u32)rwd) - uf((u32)ref);
      printf("%-6s %11g %11g %11g %11g err: %g\n", _alu_nam[alu], uf((u32)r1), uf((u32)r2), uf((u32)rwd), uf((u32)ref), err );
      break;
    case FLOAT:
      err = uf((u32)rwd) - uf((u32)ref);
      printf("%-6s %11d %11d %11g %11g err: %g\n", _alu_nam[alu], (u32)r1, (u32)r2, uf((u32)rwd), uf((u32)ref), err / uf((u32)ref)); //, (u32)r1, (u32)rwd, fu((float)r1));

      break;
    case FIX:
      printf("%-6s %11g %11d %11d %11d err: %d\n", _alu_nam[alu], uf((u32)r1), (u32)r2, (u32)rwd, (u32)ref, rwd - ref );
      break;
    default:  // int op int -> int   
      printf("%-6s %11d %11d %11d %11d err: %d\n", _alu_nam[alu], (u32)r1, (u32)r2, (u32)rwd, (u32)ref, rwd - ref);
      break;
  }
}



