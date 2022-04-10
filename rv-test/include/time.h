
// time.h
// 
//

#ifndef _TIME_H
#define _TIME_H

#include "ulib.h"

//---------------------------------------------------------
typedef u64 clock_t;

clock_t clock();
#define CLOCKS_PER_SEC ((u64)(f_clk + 0.5))

#endif // _TIME_H
