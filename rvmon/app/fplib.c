/*
	ieee754 subset float functions.

*/
#ifdef i386
 #include <endian.h>
#else
 #define __BYTE_ORDER	__BIG_ENDIAN
#endif
typedef struct {
 union{
  unsigned u;
  struct {
#if __BYTE_ORDER == __BIG_ENDIAN
   unsigned sgn:1 __attribute__ ((packed)) ;
   unsigned exp:8 __attribute__ ((packed)) ;
   unsigned sig:23 __attribute__ ((packed)) ;
#else
   unsigned sig:23 __attribute__ ((packed)) ;
   unsigned exp:8 __attribute__ ((packed)) ;
   unsigned sgn:1 __attribute__ ((packed)) ;
#endif
  } x;
  float f;
 };
} sf;

typedef struct {
 union{
  unsigned u;
  struct {
#if __BYTE_ORDER == __BIG_ENDIAN
   unsigned xs:8 __attribute__ ((packed)) ;
   unsigned ms:16 __attribute__ ((packed)) ;
   unsigned ls:8 __attribute__ ((packed)) ;
#else
   unsigned ls:8 __attribute__ ((packed)) ;
   unsigned ms:16 __attribute__ ((packed)) ;
   unsigned xs:8 __attribute__ ((packed)) ;
#endif
  } x;
 };
} si;

//typedef union {float f; unsigned u;} fu_t;

#define fu(x)   ((fu_t)(x)).u
#define uf(x)   ((fu_t)(x)).f

#define us(a)		fu(a)	//(*((unsigned*)&(a)))	// convert float to unsigned
#define fl(a)		uf(a)	//(*((float*)&(a)))	// convert unsigned to float

#define is_zero(a)	((us(a) & 0x7fffffff)==0)	// +0.0f or -0.0f
#define is_nan(a)	((us(a) & 0x7f800000)==0x7f800000 && (us(a) & 0x7fffff))
#define is_inf(a)	((us(a) & 0x7fffffff)==0x7f800000)
#define FINF		((union{unsigned u;float f;}){u:0x7f800000}).f
#define FNAN		((union{unsigned u;float f;}){u:0x7fc00000}).f

float __addsf3(float a, float b)
{
	sf ua, ub, ur;
	int d, sa, sb, exp;
	ua.u = us(a);
	ub.u = us(b);
	if(is_nan(a) || is_nan(b)) return FNAN;
	if(is_inf(a) && is_inf(b)) return ua.x.sgn == ub.x.sgn ? FINF : FNAN;
	if(is_inf(a)) return a;
	if(is_inf(b)) return b;
	sa = (ua.x.sig | 0x800000)<<1;
	sb = (ub.x.sig | 0x800000)<<1;
	exp = ua.x.exp;
	if(ua.x.exp > ub.x.exp){
		d = ua.x.exp - ub.x.exp;		
		if(is_zero(b) || d > 24){
			sb = 0;
		}else{
			sb >>= d;
		}
	}else if(ua.x.exp < ub.x.exp){
		d = ub.x.exp - ua.x.exp;
		exp = ub.x.exp;
		if(is_zero(a) || d > 24){
			sa = 0;
		}else{
			sa >>= d;
		}
	}
	sa = ua.x.sgn ? -sa : sa;
	sb = ub.x.sgn ? -sb : sb;
	sa = (sa + sb + 1)>>1;
	if(sa < 0){
		sa = -sa;
		ur.x.sgn = 1;
	}else{
		ur.x.sgn = 0;
	}
	if(sa == 0){
		exp = 0;
	}else if(sa & 0x1000000){
		sa = (sa >> 1) + (sa & 0x1);
		exp++;
	}else{
		while(!(sa & 0x800000)){
			sa <<= 1;
			exp--;
		}
	}
	if(exp < 0){
		sa = exp = 0;
	}
	ur.x.exp = exp;
	ur.x.sig = sa;

	return ur.f;
}
float __subsf3(float a, float b)
{
	return __addsf3(a, us(b) ^ 0x80000000);
}
float __mulsf3(float a, float b)
{
	sf ua, ub, ur;
	si ma, mb;
	unsigned sa, exp;
	ua.u = us(a);
	ub.u = us(b);

	if(is_nan(a) || is_nan(b)) return FNAN;
	if(is_inf(a)) return is_zero(b) ? FNAN : (ub.x.sgn ? -a : a);
	if(is_inf(b)) return is_zero(a) ? FNAN : (ua.x.sgn ? -b : b);
	if(is_zero(a) || is_zero(b)) return ua.x.sgn == ub.x.sgn ? 0.0f : -0.0f;
	ma.u = (ua.x.sig | 0x800000);	// unpack
	mb.u = (ub.x.sig | 0x800000);
	exp = ua.x.exp + ub.x.exp;
	sa = (ma.x.ms * mb.x.ms 
		+ (((ma.x.ms * mb.x.ls + ma.x.ls * mb.x.ms) + ((ma.x.ls * mb.x.ls) >> 8)) >> 8)
		) >> 6;			// (24bit * 24bit => 48bit) >> 6
	if(sa & 0x2000000){		// 4.0 > mul >= 2.0
		sa = (sa >> 2) + ((sa>>1) & 0x1);
		exp++;
	}else{				// 2.0 > mul >= 1.0
		sa = (sa >> 1) + (sa & 0x1);
	}
	if(exp < 128){
		exp = 0;	// underflow
		sa = 0;
	}else if(exp > 254+127){
		exp = 255;	// overfow
		sa = 0;
	}else{
		exp -= 127;
	}
	ur.x.sgn = ua.x.sgn ^ ub.x.sgn;
	ur.x.exp = exp;
	ur.x.sig = sa;
	return ur.f;
}
float __divsf3(float a, float b)
{
	sf ua, ub, ur;
	unsigned sa, sb, q, m, exp;
	ua.u = us(a);
	ub.u = us(b);

	if(is_nan(a) || is_nan(b)) return FNAN;
//	if(is_inf(a)) return is_zero(b) ? FNAN : (ub.x.sgn ? -a : a);
	if(is_inf(b)) return is_inf(a) ? FNAN : (ua.x.sgn == ub.x.sgn ? 0.0f : -0.0f);
	if(is_zero(b)) return (ua.x.sgn == ub.x.sgn ? FINF : -FINF);
	if(is_zero(a)) return ua.x.sgn == ub.x.sgn ? 0.0f : -0.0f;
	sa = (ua.x.sig | 0x800000);	// unpack
	sb = (ub.x.sig | 0x800000);
	exp = 254 + ua.x.exp - ub.x.exp;
	q = (sa<<8) / sb;	// 24bit*s^25 / 24bit => 25bit
	m = (sa<<8) % sb;
	q = (q<<8)+((m<<8)/sb);
	m = (m<<8) % sb;
	q = (q<<8)+((m<<8)/sb);
	m = (m<<8) % sb;
	q = (q<<1)+((m<<1)/sb);
//q = (((unsigned long long)sa)<<25)/sb;
//printf("|%8x|",q);
	if(q & 0x2000000){		// 2.0 > mul >= 1.0
		sa = (q >> 2) + ((q >> 1) & 0x1);
	}else{				// 1.0 > mul >= 0.5
		sa = (q >> 1) + (q & 0x1);
		exp--;
	}
	if(exp <= 127){
		exp = 0;	// underflow
		sa = 0;
	}else if(exp > 254+127){
		exp = 255;	// overfow
		sa = 0;
	}else{
		exp -= 127;
	}
	ur.x.sgn = ua.x.sgn ^ ub.x.sgn;
	ur.x.exp = exp;
	ur.x.sig = sa;
	return ur.f;
}
int __cmpsf2(float a, float b)
{
	sf ua, ub;
	int sa, sb;
	ua.u = us(a);
	ub.u = us(b);
	sa = is_zero(a) ? 0 : (ua.x.sgn ? (ua.u ^ 0x7fffffff) : ua.u);
	sb = is_zero(b) ? 0 : (ub.x.sgn ? (ub.u ^ 0x7fffffff) : ub.u);
	if(ua.x.sgn == ub.x.sgn) sa = sa - sb;
	else sa = (sa>>1) - (sb>>1);
	return sa ? (sa < 0 ? -1 : 1) : 0;
}
#if 0
int __eqsf2(float a, float b){}
int __gtsf2(float a, float b){}
int __gesf2(float a, float b){}
int __ltsf2(float a, float b){}
int __lesf2(float a, float b){}
float __floatunsisf(unsigned a){}
float __floatsisf(int a){}
int __fixsfsi(float a){}
unsigned __fixunsfsi(float a){}
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define FADD	1
#define FMUL	2
#define FDIV	3
#define FSUB	4
#define FCMP	5

#define FUNC	FADD

#if FUNC == FADD
#define op	+
#define ops	"+"
#define fnc(a,b)	__addsf3(a,b)
#elif FUNC == FMUL
#define op	*
#define ops	"*"
#define fnc(a,b)	__mulsf3(a,b)
#elif FUNC == FDIV
#define op	/
#define ops	"/"
#define fnc(a,b)	__divsf3(a,b)
#elif FUNC == FSUB
#define op	-
#define ops	"-"
#define fnc(a,b)	__subsf3(a,b)
#elif FUNC == FCMP
#define op	-
#define ops	"-"
#define fnc(a,b)	__cmpsf2(a,b)
#endif

void check(float a, float b, float x)
{
	float r = a op b;
	float err = x-r;
	printf("%12g %s %12g = %12g (%12g)  %12g %8x %8x %8x (%8x)\n", 
		 a, ops, b,     x,    r,    err, us(a),us(b),us(x),us(r));
}
void label()
{
	printf(
  "          a  op          b              x          ref              err       a        b        x       ref\n");
//"-3.36737e+26 + -1.46183e-08 =  3.36737e+26 ( 3.36737e+26)             0 6b8b4567 327b23c6 6b8b4567 (6b8b4567)
}

int main()
{
	int i;
	unsigned ia, ib;
	float x, a, b;

	label();
	x = fnc(a=1.0e-4f, b=-10.0e-6f); check(a,b,x);
	x = fnc(a=2.0f, b=10.0f); check(a,b,x);
	x = fnc(a=0.0f, b=10.0f); check(a,b,x);
	x = fnc(a=1.0f, b=1.0f); check(a,b,x);
	x = fnc(a=-1.001f, b=-1.001f); check(a,b,x);
	x = fnc(a=1.0f, b=0.0f); check(a,b,x);
	x = fnc(a=-0.0f, b=10.0f); check(a,b,x);
	x = fnc(a=1.0f, b=-0.0f); check(a,b,x);
	x = fnc(a=1.0f, b=10.0f); check(a,b,x);
	x = fnc(a=10.0f, b=1.0f); check(a,b,x);
	x = fnc(a=10.0f, b=.0f); check(a,b,x);
	x = fnc(a=10.0f, b=-1.0f); check(a,b,x);
	x = fnc(a=-10.0f, b=-1.0f); check(a,b,x);
	x = fnc(a=10.0f, b=-10.0f); check(a,b,x);
	x = fnc(a=1.0f, b=-10.0f); check(a,b,x);
	x = fnc(a=1.0e-4f, b=-10.0e-6f); check(a,b,x);
	x = fnc(a=1.0001f, b=-1.0002f); check(a,b,x);
	x = fnc(a=FINF, b=-10.0e-6f); check(a,b,x);
	x = fnc(a=FINF, b=-FINF); check(a,b,x);
	x = fnc(a=1.0f, b=-FINF); check(a,b,x);
	x = fnc(a=-1.0f, b=-FINF); check(a,b,x);
	x = fnc(a=0.0f, b=FINF); check(a,b,x);
	x = fnc(a=0.0f, b=-FINF); check(a,b,x);
	x = fnc(a=-0.0f, b=-FINF); check(a,b,x);
	x = fnc(a=FNAN, b=-10.0e-6f); check(a,b,x);

	label();
	for(i = 0; i < 100; i++){
	ia = rand()*3;
	ib = rand()*3;
	x = fnc(a=fl(ia), b=fl(ib)); check(a,b,x);
	}
	label();
	return 0;
}

