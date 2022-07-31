//
// 2022/07  mathsf test
//

#include <stdio.h>
#include <math.h>

#define FIRM

#ifndef FIRM
#include "Grp.h"
#else
#define g_plotm(x,y,n)
#define g_set_title(p,t,...)
#define g_set_xlabel(p,t)
#define g_set_ylabel(p,t)
#define g_set_legend(p,l,t)
#define g_disp(m)
#endif

#include "mathsf.h"

#include "time.h"

int  main()
{
    float x, y, y1, y2, er1, er2;
    double yr;
        double em1, em2;
    double XST	= 0.1;
    double XEND	= 10.0;
    volatile unsigned t0,t1,t2,t3;
    unsigned e0,e1,e2;
    int n;
    em1 = em2 = 0.0;
    n = 0;
    e0 = e1 = e2 = 0;
    for(x = XST; x < XEND; x *= 1.1f){
        t0 = clock();
        yr = log((double)x);	// ref
        t1 = clock();
        y1 = logsf(x);	// dut
        t2 = clock();
        y2 = logf(x);	// 
        t3 = clock();
        er1 = fabs((yr - y1));
        er2 = fabs((yr - y2));
        em1 = em1 < er1 ? er1 : em1;
        em2 = em2 < er2 ? er2 : em2;
        g_plotm((double)x, (double)fabs(yr), 1);
        g_plotm((double)x, (double)fabs(y1), 2);
        if(er1 > 1e-8) g_plotm((double)x, (double)er1, 4);
        if(er2 > 1e-8) g_plotm((double)x, (double)er2, 5);
        e0 += t1 - t0;
        e1 += t2 - t1;
        e2 += t3 - t2;
        n++;
    //	printf("x:%6.4f\n", fu(x));
    }
    printf("logsf  err:%6.2fe-6 logf  err:%6.2fe-6 ",fu((float)em1*1e6f), fu((float)em2*1e6f));
    printf(" cycle log: %7.1f logsf: %5.1f logf: %5.1f\n", fu((float)e0/n), fu((float)e1/n), fu((float)e2/n));

    g_set_title(0, "arithmetic error of logsf() %g < x < %g", XST, XEND);
    g_set_xlabel(0, "x");
    g_set_ylabel(0, "y / |error|");
    g_set_legend(0, 1, "log()");
    g_set_legend(0, 2, "logsf()");
    g_set_legend(0, 4, "logsf err");
    g_set_legend(0, 5, "logf err");
    g_disp("log");

    XST = -1.0;
    XEND = 1.0;
    em1 = em2 = 0.0;
    n = 0;
    e0 = e1 = e2 = 0;
    for(x = XST; x < XEND; x += 0.03f){
        t0 = clock();
        yr = exp((double)x);	// ref
        t1 = clock();
        y1 = expsf(x);	// dut
        t2 = clock();
        y2 = expf(x);
        t3 = clock();
        er1 = fabs((yr - y1));
        er2 = fabs((yr - y2));
        em1 = em1 < er1 ? er1 : em1;
        em2 = em2 < er2 ? er2 : em2;
        g_plotm((double)x, (double)fabs(yr), 1);
        g_plotm((double)x, (double)fabs(y1), 2);
        if(er1 > 1e-9) g_plotm((double)x, (double)er1, 4);
        if(er2 > 1e-9) g_plotm((double)x, (double)er2, 5);
        e0 += t1 - t0;
        e1 += t2 - t1;
        e2 += t3 - t2;
        n++;
    }
    printf("expsf  err:%6.2fe-6 expf  err:%6.2fe-6 ",fu((float)em1*1e6f), fu((float)em2*1e6f));
    printf(" cycle exp: %7.1f expsf: %5.1f expf: %5.1f\n", fu((float)e0/n), fu((float)e1/n), fu((float)e2/n));

    g_set_title(0, "arithmetic error of expsf() %g < x < %g", XST, XEND);
    g_set_xlabel(0, "x");
    g_set_ylabel(0, "y / |error|");
    g_set_legend(0, 1, "exp()");
    g_set_legend(0, 2, "expsf()");
    g_set_legend(0, 4, "expsf err");
    g_set_legend(0, 5, "expf err");
    g_disp("logy");

    XST = 1.0;
    XEND = 1000.0;
    em1 = em2 = 0.0;
    n = 0;
    e0 = e1 = e2 = 0;	
    for(y = -0.9; y < 0.0; y += 0.2){
       for(x = XST; x < XEND; x *= 1.2f){
        t0 = clock();
        yr = pow((double)x, (double)y);	// ref
        t1 = clock();
        y1 = powsf(x, y);	// dut
        t2 = clock();
        y2 = powf(x, y);
        t3 = clock();
        er1 = fabs((yr - y1));
        er2 = fabs((yr - y2));
        em1 = em1 < er1 ? er1 : em1;
        em2 = em2 < er2 ? er2 : em2;
        g_plotm((double)x, (double)fabs(yr), 1);
        g_plotm((double)x, (double)fabs(y1), 2);
        if(er1 > 1e-9) g_plotm((double)x, (double)er1, 4);
        if(er2 > 1e-9) g_plotm((double)x, (double)er2, 5);
        e0 += t1 - t0;
        e1 += t2 - t1;
        e2 += t3 - t2;
        n++;
      }
    }
    printf("powsf  err:%6.2fe-6 powf  err:%6.2fe-6 ",fu((float)em1*1e6f), fu((float)em2*1e6f));
    printf(" cycle pow: %7.1f powsf: %5.1f powf: %5.1f\n", fu((float)e0/n), fu((float)e1/n), fu((float)e2/n));

    g_set_title(0, "arithmetic error of expsf() %g < x < %g, -0.9 < y < 0.0", XST, XEND);
    g_set_xlabel(0, "x");
    g_set_ylabel(0, "y / |error|");
    g_set_legend(0, 1, "pow()");
    g_set_legend(0, 2, "powsf()");
    g_set_legend(0, 4, "powsf err");
    g_set_legend(0, 5, "powf err");
    g_disp("log");

    em1 = em2 = 0.0;
    n = 0;
    e0 = e1 = e2 = 0;	
    for(x = -M_PI*2; x < M_PI*2; x += 0.03){
    //    sincossf(x, &ys, &yc);
        t0 = clock();
        yr = sin((double)x);
        t1 = clock();
        y1 = sinsf(x);
        t2 = clock();
        y2 = sinf(x);
        t3 = clock();
        er1 = fabs(yr - y1);
        er2 = fabs(yr - y2);
        e0 += t1 - t0;
        e1 += t2 - t1;
        e2 += t3 - t2;
        em1 = em1 < er1 ? er1 : em1;
        em2 = em2 < er2 ? er2 : em2;
        n++;
    }
    printf("sinsf  err:%6.2fe-6 sinf  err:%6.2fe-6 ",fu((float)em1*1e6f), fu((float)em2*1e6f));
    printf(" cycle sin: %7.1f sinsf: %5.1f sinf: %5.1f\n", fu((float)e0/n), fu((float)e1/n), fu((float)e2/n));

    em1 = em2 = 0.0;
    n = 0;
    e0 = e1 = e2 = 0;
    for(x = -M_PI*2; x < M_PI*2; x += 0.03){
        t0 = clock();
        yr = cos((double)x);
        t1 = clock();
        y1 = cossf(x);
        t2 = clock();
        y2 = cosf(x);
        t3 = clock();
        er1 = fabs(yr - y1);
        er2 = fabs(yr - y2);
        e0 += t1 - t0;
        e1 += t2 - t1;
        e2 += t3 - t2;
        em1 = em1 < er1 ? er1 : em1;
        em2 = em2 < er2 ? er2 : em2;
        n++;
    }	
    printf("cossf  err:%6.2fe-6 cosf  err:%6.2fe-6 ",fu((float)em1*1e6f), fu((float)em2*1e6f));
    printf(" cycle cos: %7.1f cossf: %5.1f cosf: %5.1f\n", fu((float)e0/n), fu((float)e1/n), fu((float)e2/n));

    em1 = em2 = 0.0;
    n = 0;
    e0 = e1 = e2 = 0;
    for(x = 0.0001; x < 2.0; x += 0.03){
        t0 = clock();
        yr = sqrt((double)x);
        t1 = clock();
        y1 = sqrtsf(x);
        t2 = clock();
        y2 = sqrtf(x);
        t3 = clock();
        er1 = fabs(yr - y1);
        er2 = fabs(yr - y2);
        e0 += t1 - t0;
        e1 += t2 - t1;
        e2 += t3 - t2;
        em1 = em1 < er1 ? er1 : em1;
        em2 = em2 < er2 ? er2 : em2;
        n++;
    }	
    printf("sqrtsf err:%6.2fe-6 sqrtf err:%6.2fe-6 ",fu((float)em1*1e6f), fu((float)em2*1e6f));
    printf(" cycle sqrt:%7.1f sqrtsf:%5.1f sqrtf:%5.1f\n", fu((float)e0/n), fu((float)e1/n), fu((float)e2/n));

    return 0;
}


