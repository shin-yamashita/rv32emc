//
// ecc.c
// Reed Solomon Coding 
// primitive polynomial : X^8 + X^4 + X^3 + X^2 + 1
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//------------------------------------------------------
extern char _end[];                /* _end is set in the linker command file */
char *heap_ptr;
/*
 * sbrk -- changes heap size size. Get nbytes more
 *         RAM. We just increment a pointer in what's
 *         left of memory on the board.
 */
#ifdef __cplusplus
extern "C" char *_sbrk (int nbytes);
#endif

char *_sbrk (int nbytes)
{
  char        *base;

  if (!heap_ptr)
    heap_ptr = (char *)&_end;
  base = heap_ptr;
  heap_ptr += nbytes;

  return base;
}
//------------------------------------------------------

#define MAXN	256
#define MAXV	255

#define MAXCORR	8

typedef uint8_t GFN;	// GF(2^8) Galois extension

typedef struct {	// Galois extension Polynomial
  int n;
  GFN *x;
} Poly_t;

typedef struct {	// error location / pattern data
  int n;
  uint8_t ea[MAXCORR];
  GFN     ep[MAXCORR];
} errdata_t;


int glog[MAXN];
GFN gexp[MAXN];

void root_table()
{
  int i, x;
  x = 1;
  for(i = 0; i < MAXN-1; i++){
    glog[x] = i;
    gexp[i] = x;
    x = x << 1;
    if(x & 0x100){	// X^8 + X^4 + X^3 + X^2 + 1
      x = (x ^ 0x1d) & 0xff;
    }
  }
  glog[0] = 255;
  gexp[255] = 0;
}

GFN alpha(GFN x)	// alpha x
{
  if(x & 0x80){
    x = x << 1;
    x = (x ^ 0x1d) & 0xff;
  }else{
    x = x << 1;
  }
  return x;
}

GFN mul(GFN a, GFN b)	// a * b
{
  int i;
  GFN x = 0;
  for(i = 0; i < 8; i++){
    if(b & 0x1){
      x = x ^ a;
    }
    b = b >> 1;
    a = alpha(a);
  }
  return x;
}

GFN vmul(GFN a, GFN b)	// a * b
{
  if(!a || !b) return 0;
  return gexp[(glog[a] + glog[b]) % 255];
}

GFN vinv(GFN a)
{
  return gexp[(255-glog[a])%255];
}

GFN vdiv(GFN a, GFN b)	// a / b
{
  return vmul(a, vinv(b));
}

GFN vsqrt(GFN a)
{
  int e = glog[a];
  return gexp[((e >> 1)|((e&1) << 7))];
}

GFN alpham1(GFN x)	// alpha^-1 x
{
  return vmul(x, gexp[254]);
}

GFN alphan(GFN x, int n)	// alpha^n x
{
  return vmul(x, gexp[n%255]);
}

GFN sq(GFN a)
{
  return mul(a, a);
}

void p_print(const char *msg, Poly_t *p, int mode)
{
  int i;
  printf("%s", msg);
  for(i = p->n; i >= 0; i--){
    switch(mode){
    case 0: printf("%02xx^%d %c ", p->x[i], i, i > 0 ? '+':' '); break;
    case 1: printf("%02x ", p->x[i]);	break;
    case 2: printf("%2d ", glog[p->x[i]]);	break;
    }
  }
  printf("\n");
}

Poly_t *p_new(int n)	// n deg polynom
{
  Poly_t *a;
  a = (Poly_t *)malloc(sizeof(Poly_t));
  a->n = n < 0 ? 0 : n;
  a->x = (GFN *)calloc(a->n+1, sizeof(GFN));
  return a;
}

void p_free(Poly_t *a)
{
  if(a && a->x){
    free(a->x);
    free(a);
  }
}

Poly_t *p_resize(Poly_t *y, int n)	// resize
{
  int i;
  if(!y){
    y = p_new(n);
  }else{
    n = n < 0 ? 0 : n;
    if(n+1 > y->n)
      y->x = (GFN*)realloc(y->x, n+1);
    for(i = 0; i <= n; i++) y->x[i] = 0;
    y->n = n;
  }
  return y;
}

GFN elem(Poly_t *a, int i)
{
  return a->n < i ? 0 : a->x[i];
}

Poly_t *p_add(Poly_t *a, Poly_t *b, Poly_t *y)	// a + b => y
{
  int i;
  int n = a->n > b->n ? a->n : b->n;
  y = p_resize(y, n);
  for(i = 0; i <= n; i++){
    y->x[i] = elem(a, i) ^ elem(b, i);
  }
  return y;
}

Poly_t *p_mul(Poly_t *a, Poly_t *b, Poly_t *y)  // a * b => y
{
  int i, j;
  int n = a->n + b->n;
  y = p_resize(y, n); 
  for(i = 0; i <= a->n; i++){
    for(j = 0; j <= b->n; j++){
      y->x[i+j] ^= vmul(elem(a,i), elem(b,j));
    }
  }
  return y;
}

Poly_t *p_mul_xn(Poly_t *a, GFN x, int n, Poly_t *y)  // a * s x^n => y
{
  int i;
  int m = a->n + n;
  y = p_resize(y, m);
  for(i = 0; i <= a->n; i++)
    y->x[i+n] = vmul(a->x[i], x);
  return y;
}

Poly_t *p_cpy(Poly_t *a, Poly_t *y)
{
  y = p_resize(y, a->n);
  memcpy(y->x, a->x, a->n+1);
  return y;
}

Poly_t *p_reduc(Poly_t *a)
{
  int i;
  for(i = a->n; i > 0; i--){
    if(a->x[i]) break;
  }
  a->n = i;
  return a;
}

Poly_t *p_mod(Poly_t *a, Poly_t *b, Poly_t *q, Poly_t *r)	// a / b => q ... r
{
  int n = a->n;
  int m = b->n;
  GFN qe;
  r = p_cpy(a, r);

  Poly_t *xr = p_new(r->n);
  while(n >= m){
    qe = vdiv(r->x[n], b->x[m]);
    xr = p_mul_xn(b, qe, n-m, xr);
    if(q) q->x[n-m] = qe;
    r = p_add(r, xr, NULL);
    n--;
  }
  return r;
}

Poly_t *encode(Poly_t *info, int npar)
{
  Poly_t *code = p_new(info->n + npar);
  Poly_t *G = p_new(0);
  Poly_t *r = p_new(0);
  Poly_t *g = p_new(1);
  int i;
  G->x[0] = 1;	// G(x) Generator polynomial (x-a^0)(x-a^1)(x-a^3)...(x-a^(npar-1))
  g->x[1] = 1;	// x-a^0
  for(i = 0; i < npar; i++){
    g->x[0] = gexp[i];	// x-a^i
    G = p_mul(G, g, NULL);
  }
  for(i = 0; i <= info->n; i++){
    code->x[i+npar] = info->x[i];
  }
  r = p_mod(code, G, NULL, NULL);
  for(i = 0; i < npar; i++){
    code->x[i] = r->x[i];
  }
  return code;
}

Poly_t *syndrome(Poly_t *code, int npar)
{
  int i, j;
  Poly_t *sy = p_new(npar-1);

  for(i = 0; i <= code->n; i++){
    for(j = 0; j <= sy->n; j++){
      sy->x[j] ^= alphan(code->x[i], i*j);
    }
  }
  return sy;
}

GFN p_eval(Poly_t *a, GFN x)
{
  int i;
  GFN ev = 0;
  GFN xn = 1;
  for(i = 0; i <= a->n; i++){
    ev ^= vmul(a->x[i], xn);
    xn = vmul(xn, x);
  }
  return ev;
}

Poly_t *findroot(Poly_t *a)
{
  int i, n = 0;
  Poly_t *ea = p_new(a->n);
  GFN ev;
  for(i = 0; i <= 255; i++){
    ev = p_eval(a, gexp[i]);
    if(!ev){
      ea->x[n++] = gexp[i];	// error locater a^-i
      ea->n = n;
    }
  }
  return ea;
}

int decode(Poly_t *s, errdata_t *edat)
{
  int i;
  Poly_t *A   = p_new(s->n+1);
  Poly_t *B   = p_cpy(s, NULL);
  Poly_t *r   = NULL;
  Poly_t *ea  = NULL;
  Poly_t *q   = p_new(s->n+1);
  Poly_t *sgm = p_new(0);
  Poly_t *sgm0 = p_new(0);
  Poly_t *tmp = p_new(0);

  A->x[A->n] = 1;	// x^(n+1)
  sgm->x[0] = 1;

  while(B->n >= 4){	// Euclid's Algorithm
    r = p_mod(A, B, q, NULL);

    tmp = sgm;
    sgm = p_add(sgm0, p_mul(sgm, q, NULL), NULL);
    sgm = p_reduc(sgm);
    sgm0 = tmp;

    ea = findroot(sgm);
    A = B;
    B = p_reduc(r);
  }
//  p_print(" omega: ", B, 1);
//  p_print(" sigma: ", sgm, 1);
  // ei = w(a^-ji)/dsgm(a^-ji)
  for(i = 1; i <= sgm->n; i+=2){	// diff sigma / x
    sgm->x[i] = 0;
  }

  GFN ep, aj;
  edat->n = ea->n;
  for(i = 0; i < ea->n; i++){
    aj = ea->x[i];
    ep = vdiv(p_eval(B, aj), p_eval(sgm, aj));
    edat->ea[i] = 255 - glog[aj];
    edat->ep[i] = ep;
  }

  return ea->n;
}

int is_zero(Poly_t *a)
{
  int i;
  for(i = 0; i <= a->n; i++){
    if(a->x[i]) return 0;
  }
  return 1;
}

int seed = 100;
GFN prsg()
{
  seed = (seed>>1) ^ ((seed & 0x1) ? 0xb400 : 0);
  return seed ^ (seed >> 8);
}

#define Ni	32
#define Np	8

int main()
{
  int i, j;
  Poly_t *info, *code, *Synd;

  printf("*** GF(2^8) Reed Solomon Coding. %d parity, %d error correction.\n", Np, Np/2);

  root_table();

  info = p_new(Ni-1);

  for(i = 0; i < Ni; i++){
    info->x[info->n-i] = i;
  }
  p_print("info :", info, 1);

  code = encode(info, Np);
  p_print("code :", code, 1);

  Synd = syndrome(code, Np);
  p_print("Synd :", Synd, 1);

  int ea;
  GFN ep;
  errdata_t edat;

  for(i = 0; i < 4; i++){
    ea = prsg() % (Ni+Np);
    ep = prsg();

    code->x[ea] ^= ep;
    printf("+++++ %d err  %2x @ %d\n", i+1, ep, ea);
    p_print("code :", code, 1);
    Synd = syndrome(code, Np);
    p_print("Synd :", Synd, 1);

    decode(Synd, &edat);

    printf(" found %d errors. : ", edat.n);
    for(j = 0; j < edat.n; j++){	// correct
      printf("  %2x@%d ", edat.ep[j], edat.ea[j]);
    }
    printf("\n");
  }

  printf("++++ correct.\n");
  for(i = 0; i < edat.n; i++){	// correct
    code->x[edat.ea[i]] ^= edat.ep[i];
  }
  Synd = syndrome(code, Np);
  p_print("code :", code, 1);
  p_print("Synd :", Synd, 1);

  printf("**** %s\n", is_zero(Synd) ? "No error." : "Error");
  printf("heap used : %d B\n", heap_ptr - _end);

}



