
#include <math.h>
#include "mathsf.h"

// 2013/10/08
// Trigonometric function
// Maclaurin expansion + coeff correction
// sin(x) = x - x^3/3! + x^5/5! - x^7/7! ...
//	= x(1 - x^2/2.3 + x^4/2.3.4.5 - x^6/2.3.4.5.6.7)
// cos(x) = 1 - x^2/2! + x^4/4! - x^6/6! ...
//	= (1 - x^2/2   + x^4/2.3.4   - x^6/2.3.4.5.6)

#define Cs1 (float)(1.0/(2*3) - 38e-6)
#define Cs2 (float)(1.0/(2*3*4*5) - 180e-6)

#define Cc1 (float)(1.0/(2) + 4e-6)
#define Cc2 (float)(1.0/(2*3*4) + 15e-6)
#define Cc3 (float)(1.0/(2*3*4*5*6))

void sincossf(float x, float *s, float *c)
{
    int sgn = x < 0.0f;
    x = sgn ? -x : x;
    int ix = ((int)(x * (float)(4/M_PI)) + 1) >> 1;
    x -= ix * (float)(M_PI/2);
    float xx = x*x;
    float ss, cc;
    ss = x * (1.0f - xx * (Cs1 - xx * Cs2));
    cc = (1.0f - xx * (Cc1 - xx * (Cc2 - xx * Cc3)));

    if(sgn){
        switch(ix & 0x3){
        case 0: *s = -ss; *c = cc;  break;
        case 1: *s = -cc; *c = -ss; break;
        case 2: *s = ss;  *c = -cc; break;
        case 3: *s = cc;  *c = ss;  break;
        }
    }else{
        switch(ix & 0x3){
        case 0: *s = ss;  *c = cc;  break;
        case 1: *s = cc;  *c = -ss; break;
        case 2: *s = -ss; *c = -cc; break;
        case 3: *s = -cc; *c = ss;  break;
        }
    }
}

float sinsf(float x)
{
    int sgn = x < 0.0f ? -1 : 1;
    x *= sgn;
    int ix = ((int)(x * (float)(4/M_PI)) + 1) >> 1;
    x -= ix * (float)(M_PI/2);
    float xx = x*x;
    sgn = (ix & 0x2) ? -sgn : sgn;
    switch(ix & 0x1){
        case 0: return (x * (1.0f - xx * (Cs1 - xx * Cs2)))*sgn;
        case 1: return (1.0f - xx * (Cc1 - xx * (Cc2 - xx * Cc3)))*sgn;
    }
    return 0.0f;
}

float cossf(float x)
{
    x = x < 0.0f ? -x : x;
    int ix = ((int)(x * (float)(4/M_PI)) + 1) >> 1;
    x -= ix * (float)(M_PI/2);
    float xx = x*x;
    switch(ix & 0x3){
        case 0: return (1.0f - xx * (Cc1 - xx * (Cc2 - xx * Cc3)));
        case 1: return -(x * (1.0f - xx * (Cs1 - xx * Cs2)));
        case 2: return -(1.0f - xx * (Cc1 - xx * (Cc2 - xx * Cc3)));
        case 3: return (x * (1.0f - xx * (Cs1 - xx * Cs2)));
    }
    return 0.0f;
}

#ifdef MAIN
#include <stdio.h>
#include "Grp.h"
int main()
{
 double a;
 float x, y, ys, yc, y1, er;
 g_page_attach(0,0,FULLSCR);
 g_page_attach(1,1,FULLSCR);
 do{
    g_page(0); g_clear();
    g_page(1); g_clear();
    for(x = -M_PI*2; x < M_PI*2; x += 0.01){
        fsincos(x, &ys, &yc);
        y = sin(x);
    //	y1 = sinp((float)x);
        er = fabs(y - ys);
        g_plot_lt(0, (double)x, (double)y, 1);
        g_plot_lt(0, (double)x, (double)ys, 2);
        ys = fsin(x);
        er = fabs(y - ys);
        g_plot_lt(0, (double)x, (double)ys, 3);
        if(er > 1e-12) g_plot_lt(1, (double)x, (double)er, 5);
        y = cos(x);
    //	y1 = cosp((float)x);
        er = fabs(y - yc);
        g_plot_lt(0, (double)x, (double)y, 4);
        g_plot_lt(0, (double)x, (double)yc, 5);
        yc = fcos(x);
        er = fabs(y - yc);
        g_plot_lt(0, (double)x, (double)yc, 6);
        if(er > 1e-12) g_plot_lt(1, (double)x, (double)er, 6);
    }
    g_view_page(0);
    g_view_page(1);
    fprintf(stderr, "alpha:");
    scanf("%lf", &a);
 }while(1);
}
#endif


