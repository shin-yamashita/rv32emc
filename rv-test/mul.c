//
// mul.c test mul/mulh/mulhsu/mulhu
//

#include <stdio.h>
#include <stdint.h>

uint32_t lfsr;

uint32_t prsg()
{
  if(lfsr == 0) lfsr = 1;
  lfsr = (lfsr >> 1) ^ (-(lfsr & 1u) & 0xd0000001u); /* taps 32 31 29 1 */
  return lfsr;
}


int main()
{
  uint32_t ua, ub;
  int32_t  sa, sb;
  uint64_t ux;
  int64_t  sx;
  int i;

  for(i = 0; i < 100; i++){
    sa = ua = prsg();
    sb = ub = prsg();

    ux = (uint64_t)ua * ub;
    sx = (int64_t)sa * sb;

    printf("u %d * %d >> 32 = %d\n", ua, ub, ux>>32);
    printf("s %d * %d >> 32 = %d\n", sa, sb, sx>>32);

  }
}


