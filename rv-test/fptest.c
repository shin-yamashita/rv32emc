//
// 2011/4/20 fpu test
// FPGA impliment bug check
//#define STD

#ifndef STD
#include "stdio.h"
 #include "ulib.h"
#else
 #include <stdio.h>
#endif
#include <stdlib.h>
#include <math.h>

void print_dbl(float x)
{
	int f, e, s;
	s = x < 0.0;
	x = s ? -x : x;
	f = (int)x;
	e = (x - f)*100000;
	printf("%s%d.%05d", s ? "-":"", f, e);
}

void compare_test(float x, float y)
{
	print_dbl(x);
	printf(" %c ", x>y?'>':(x<y?'<':'='));
	print_dbl(y);
#ifdef STD
	printf("\t%08x %08x", *(unsigned*)&x, *(unsigned*)&y);
#endif
	printf("\n");
}

int main()
{
	float x;
	int j;

#if 1
	compare_test(1.0f, 2.0f);
	compare_test(0.01f, 0.0001f);
	compare_test(3.14f, 3.14f);
	compare_test(-1.0f, -2.0f);
	compare_test(-0.01f, -0.0001f);
	compare_test(-3.14f, -3.14f);
	compare_test(1.0f, -2.0f);
	compare_test(0.01f, -0.0001f);
	compare_test(3.14f, -3.14f);
	compare_test(-1.0f, 2.0f);
	compare_test(-0.01f, 0.0001f);
	compare_test(-3.14f, 3.14f);
	compare_test(0.0f, 0.0f);
	compare_test(-0.0f, 0.0f);
	compare_test(-0.0f, -0.0f);
#endif
	for(x = 0.003f; x < 2.0f; x *= 1.41f){
		print_dbl(x);
	//	printf("%8.5f ", fu(x));
		for(j = 0; j < 100; j++){
			if((100*x) >= j) putchar('*');
			else		break;
		}
		putchar('\n');
//		printf("\n");
	}
}


