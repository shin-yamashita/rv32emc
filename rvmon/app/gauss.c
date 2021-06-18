
//void *_dso_handle;

#include <stdio.h>
#include <math.h>
//#include "ulib.h"
//#include "grp.h"

float gaussrand();

#define FLG_LEFT        1
#define FLG_CAPS        2
#define FLG_PAD0        4

#if 0
#define MDGT    12
char *print_float(char *str, float data, int digit, int dp, int flg)
{
        int i, c, l = 0, sgn = 0, cnt = 0;
        char buf[MDGT+1], *s = str;
        int idata, idp = 1;

        for(i = 0; i < dp; i++) idp *= 10;

        if(data < 0.0f){
                sgn = 1;
                data = -data;
        }
        idata = (int)(data * idp + 0.5f);
        i = MDGT;
        if(dp){
            for(; i > 0 ; i--){
                buf[i] = (idata % 10) + '0';
                idata /= 10;
                l++;
                if(l >= dp){
                        break;
                }
            }
            buf[--i] = '.';
            i--;
        }
        for(; i > 0 ; i--){
                buf[i] = (idata % 10) + '0';
                idata /= 10;
                l++;
                if(!idata){
                        if(sgn){
                                buf[--i] = '-';
                                l++;
                        }
                        break;
                }
        }
        if(digit && !(flg & FLG_LEFT)){
                c = ' ';
                while(digit > l){
                        *s++ = c;	//if(fputc(c, fp) == EOF) return EOF;
                        digit--;
                        cnt++;
                }
        }
        for(; i <= MDGT ; i++){
                c = buf[i];
                *s++ = c;	//if(fputc(c, fp) == EOF) return EOF;
                cnt++;
        }
        while(digit > l){
                *s++ = c;	//if(fputc(' ', fp) == EOF) return EOF;
                digit--;
                cnt++;
        }
	*s = '\0';
        return str;
}
#endif

int main()
{
	int i, ig;
	float g;
	float rms;
	int N = 10000;
//	int N = 800;
//	int N = 10;
	int hist[120], *hp = &hist[60];
//    zero_bss();

	for(i = 0; i < 120; i++) hist[i] = 0;

	rms = 0.0f;
	for(i = 0; i < N; i++){
		g = gaussrand();
		rms += g*g;
		ig = (int)(g * 10 + 60.5f);
		hist[ig]++;
	}
	rms = sqrtf(rms / N);

	for(i = -40; i < 40; i++){
		int j;
	//	char fstr[20];
	//	print_float(fstr, 100*(float)hp[i]/N, 3, 2, FLG_LEFT);
	//	printf("%3d: %s :", i, fstr);
		printf("%3d: %5.3f :", i, fu(100*(float)hp[i]/N));
		for(j = 0; j < 100; j++){
			if(2000*hp[i]/N > j){
				printf(j%20==19 ? "|":"*");
			}else{
				break;
			}
		}
		printf("\n");
	}
	return 0;
}

#include <math.h>

#define RAND_MAX	2147483647
unsigned seed = 1;

unsigned rand()
{
	seed = (seed * 1103515245 + 12345) & RAND_MAX;
	return seed;
}


float gaussrand()
{
        static float V1, V2, S;
        static int phase = 0;
        float X;

        if(phase == 0) {
 	       do {
 	 	      float U1 = (float)rand() / RAND_MAX;
 	 	      float U2 = (float)rand() / RAND_MAX;
 	 	      V1 = 2 * U1 - 1;
 	 	      V2 = 2 * U2 - 1;
 	 	      S = V1 * V1 + V2 * V2;
 	 	      } while(S >= 1 || S == 0);

 	       X = V1 * sqrtf(-2 * logf(S) / S);
        } else
 	       X = V2 * sqrtf(-2 * logf(S) / S);

        phase = 1 - phase;

        return X;
}

