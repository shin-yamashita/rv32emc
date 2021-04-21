//
// pi.c
//


#include <stdio.h>
#include <stdint.h>

#define DGTS	1000000000

void exadd(int n, uint32_t *a, uint32_t *b, uint32_t *y)
{
  int i;
  uint32_t yy, cy = 0;
  a += n-1;
  b += n-1;
  y += n-1;
  for(i = n - 1; i >= 0; i--){
    yy = *a-- + *b-- + cy;
    if(yy > DGTS){
      cy = 1;
      *y = yy - DGTS;
    }else{
      cy = 0;
      *y = yy;
    }
    y--;
  }
}

void exsub(int n, uint32_t *a, uint32_t *b, uint32_t *y)
{
  int i;
  uint32_t bb, cy = 0;
  a += n-1;
  b += n-1;
  y += n-1;
  for(i = n - 1; i >= 0; i--){
    bb = *b-- + cy;
    if(*a < bb){
      *y = DGTS + *a - bb;
      cy = 1;
    }else{
      *y = *a - bb;
      cy = 0;
    }
    a--;
    y--;
  }
}

int exdiv(int n, uint32_t *a, uint32_t b, uint32_t *y)
{
  int i, z = 1;
  uint64_t r, d;
  r = 0;
  for(i = 0; i < n; i++){
    d = *a++ + r;
    *y = d / b;
    r = (d - (uint64_t)b * *y) * DGTS;
    if(*y) z = 0;
    y++;
  }
  return z;	// return y == 0
}

// 
// - Matin's formula
// PI/4 =  4*atan(1/5) -   atan(1/239)
// PI   = 16*atan(1/5) - 4*atan(1/239)
// atan(1/p) = 1/p - 1/(3*p^3) + 1/(5*p^5) - 1/(7*p^7) + ...
//            at1             at2        
// pi =  (16*5/(5*5)^1 - 4*239/(239*239)^1) / 1
//     - (16*5/(5*5)^2 - 4*239/(239*239)^2) / 3
//     + (16*5/(5*5)^3 - 4*239/(239*239)^3) / 5
//     - (16*5/(5*5)^4 - 4*239/(239*239)^4) / 7
//     +...

int calc_pi(int n, uint32_t *at1, uint32_t *at2, uint32_t *pi1, uint32_t *pi)
{
  int i, z;

  for(i = 0; i < n; i++){
    pi[i] = at1[i] = at2[i] = pi1[i] = 0;
  }
  at1[0] = 16 * 5;
  at2[0] = 4 * 239;

  z = 0;
  for(i = 1; !z; i+=2){
    exdiv(n, at1, 5*5, at1);
    exdiv(n, at2, 239*239, at2);
    exsub(n, at1, at2, pi1);
    z = exdiv(n, pi1,  i, pi1);
    if(i & 0x2)
      exsub(n, pi, pi1, pi);
    else
      exadd(n, pi, pi1, pi);
//    if(i < 16) printf("%3d %09d%09d%09d %09d%09d%09d %09d%09d%09d %09d%09d%09d\n", i, at1[0],at1[1],at1[2], at2[0],at2[1],at2[2], pi1[0],pi1[1],pi1[2], pi[0],pi[1],pi[2]);
  }
  return i/2;
}

// Gauss's formula
// PI/4 =  12*atan(1/18)   + 8*atan(1/57)   - 5*atan(1/239)
// PI   =  4*12*atan(1/18) + 4*8*atan(1/57) - 4*5*atan(1/239)
//            at1              at2           at3
// pi =  (48*18/(18*18)^1 + 32*57/(57*57)^1 - 20*239/(239*239)^1) / 1
//     - (48*18/(18*18)^2 + 32*57/(57*57)^2 - 20*239/(239*239)^2) / 3
//     + (48*18/(18*18)^3 + 32*57/(57*57)^3 - 20*239/(239*239)^3) / 5
//     - (48*18/(18*18)^4 + 32*57/(57*57)^4 - 20*239/(239*239)^4) / 7
//     +...

int calc_pi2(int n, uint32_t *at1, uint32_t *at2, uint32_t *at3, uint32_t *pi1, uint32_t *pi)
{
  int i, z, z1;

  for(i = 0; i < n; i++){
    pi[i] = at1[i] = at2[i] = at3[i] = pi1[i] = 0;
  }
  at1[0] = 48 * 18;
  at2[0] = 32 * 57;
  at3[0] = 20 * 239;

  z = z1 = 0;
  for(i = 1; !z; i+=2){
    exdiv(n, at1, 18*18,   at1);
    exdiv(n, at2, 57*57,   at2);
    if(!z1) z1 = exdiv(n, at3, 239*239, at3);

    exadd(n, at1, at2, pi1);
    if(!z1) exsub(n, pi1, at3, pi1);
    z = exdiv(n, pi1, i, pi1);
    if(i & 0x2)
      exsub(n, pi, pi1, pi);
    else
      exadd(n, pi, pi1, pi);
  }
  return i/2;
}

// 高野喜久雄's formula
// PI/4 =  12*atan(1/49)   + 32*atan(1/57)   - 5*atan(1/239)   + 12*atan(1/110443)
// PI   =  4*12*atan(1/49) + 4*32*atan(1/57) - 4*5*atan(1/239) + 4*12*atan(1/110443)
//            at1              at2                at3                      at4
// pi =  (48*49/(49*49)^1 + 128*57/(57*57)^1 - 20*239/(239*239)^1) + 48*110443/(110443*110443)^1) / 1
//     - (48*49/(49*49)^2 + 128*57/(57*57)^2 - 20*239/(239*239)^2) + 48*110443/(110443*110443)^2) / 3
//     + (48*49/(49*49)^3 + 128*57/(57*57)^3 - 20*239/(239*239)^3) + 48*110443/(110443*110443)^3) / 5
//     - (48*49/(49*49)^4 + 128*57/(57*57)^4 - 20*239/(239*239)^4) + 48*110443/(110443*110443)^4) / 7
//     +...

int calc_pi3(int n, uint32_t *at1, uint32_t *at2, uint32_t *at3, uint32_t *at4, uint32_t *pi1, uint32_t *pi)
{
  int i, z, z1, z2;

  for(i = 0; i < n; i++){
    pi[i] = at1[i] = at2[i] = at3[i] = at4[i] = pi1[i] = 0;
  }
  at1[0] = 48 * 49;
  at2[0] = 128 * 57;
  at3[0] = 20 * 239;
  at4[0] = 48 * 110443;

  z = z1 = z2 = 0;
  for(i = 1; !z; i+=2){
    exdiv(n, at1, 49*49,   at1);
    exdiv(n, at2, 57*57,   at2);
    if(!z1) z1 = exdiv(n, at3, 239*239, at3);
    if(!z2) z2 = exdiv(n, at4, 110443, at4);
    if(!z2) z2 = exdiv(n, at4, 110443, at4);

    exadd(n, at1, at2, pi1);
    if(!z1) exsub(n, pi1, at3, pi1);
    if(!z2) exadd(n, pi1, at4, pi1);
    z = exdiv(n, pi1, i, pi1);
    if(i & 0x2)
      exsub(n, pi, pi1, pi);
    else
      exadd(n, pi, pi1, pi);
  }
  return i/2;
}

#define N 113

int main()
{
  int i, itr;
  uint32_t at1[N];
  uint32_t at2[N];
  uint32_t at3[N];
  uint32_t at4[N];
  uint32_t pi1[N];
  uint32_t pi[N];

//  itr = calc_pi(N, at1, at2, pi1, pi);
//  itr = calc_pi2(N, at1, at2, at3, pi1, pi);
  itr = calc_pi3(N, at1, at2, at3, at4, pi1, pi);

  printf("iter:%d\n3.", itr);
  for(i = 1; i < N; i++){
    printf("%09d", (int)pi[i]);
  }
  printf("\n");

}

