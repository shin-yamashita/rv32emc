//
// mul.c test mul/mulh/mulhsu/mulhu
//

#include <stdio.h>
#include <stdint.h>

//uint32_t lfsr;
uint32_t seed = 0;

uint32_t prsg()
{
//  if(lfsr == 0) lfsr = 1;
//  lfsr = (lfsr >> 1) ^ (-(lfsr & 1u) & 0xd0000001u); /* taps 32 31 29 1 */
//  return lfsr;
  seed = (seed * 1103515245 + 12345);
  return seed;
}


int main()
{
  uint32_t ua, ub;
  int32_t  sa, sb;
  uint64_t ux;
  int64_t  sx, sux;
  int32_t  ud, sd;
  int32_t  um, sm;
  int i;

  for(i = 0; i < 50; i++){
    sa = ua = prsg();
    sb = ub = prsg();

    ux = (uint64_t)ua * ub;
    sx = (int64_t)sa * sb;
    sux = (int64_t)sa * ub;

    printf("u  %10u * %10u >> 32 = %d  %08x %08x\n", ua, ub, (uint32_t)(ux>>32), (uint32_t)(ux>>32), ux&0xffffffff);
    printf("s  %10d * %10d >> 32 = %d  %08x %08x\n", sa, sb, (uint32_t)(sx>>32), (uint32_t)(sx>>32), sx&0xffffffff);
    printf("su %10d * %10u >> 32 = %d  %08x %08x\n", sa, ub, (uint32_t)(sux>>32), (uint32_t)(sux>>32), sux&0xffffffff);
  }
  for(i = 0; i < 50; i++){
    sa = ua = prsg();
    sb = (int32_t)prsg() / 100;
    ub = prsg() / 100;

    ud = ua / ub;
    um = ua % ub;
    sd = sa / sb;
    sm = sa % sb;

    printf("u  %10u / %10u = %d ... %d\n", ua, ub, ud, um);
    printf("s  %10d / %10d = %d ... %d\n", sa, sb, sd, sm);
  }
}


