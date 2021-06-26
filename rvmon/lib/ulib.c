//======================================================
//  2010/04  ulib.c   mm6 hardware driver library
//

#include "stdio.h"
#include "ulib.h"
#include "time.h"

// POUT  parallel output port

static unsigned char _pout = 0;

int get_pout()
{
    return _pout;
}
void set_pout(int d)		// direct set 8bit
{
    *POUT = _pout = d;
}
void set_port(int bit)		// bit set
{
    _pout |= (1<<bit);
    *POUT = _pout;
}
void reset_port(int bit)	// bit reset
{
    _pout &= ~(1<<bit);
    *POUT = _pout;
}
int get_port()
{
    return *POUT;
}

float get_temp()        // mm8 Artix-7 FPGA temperature sens
{
        return (*TEMP * (503.975f / 4096) - 273.15f);
        // temp(degree)
}

float get_vbatt()
{
        return *VBATT * (5.7f / 4096);
        // V battery (V)
}


//	mtime interface

static volatile int timer;
static volatile int tick;
static volatile int tmexp, expose;
static volatile int _br;

void init_timer(int br)
{
    _br = (int)(f_clk / br + 0.5f);
    *mtime = 0l;
    *mtimecmp = (u64)_br;
    timer = 0;
    csrs(mie, MTIE);
    //	csrc(mip, MTIE);
}
void at_exit()
{
    printf("** exit() timer: %d\n", timer);
}
void enable_irq()
{
    csrs(mie, MEIE);
}
void set_expose(int exp)
{
    expose = exp;
}
/*
void timer_ctrl(void)
{
	if(csrr(mip) & MTIE){
 *mtimecmp += _br;
	}
	tmexp++;
}
 */

void wait(void)
{
    while(!tick) ;  // wait for 1 ms interrupt
    tick = 0;
}
void n_wait(int n)
{
    int i;
    for(i = 0; i < n; i++) wait();
}
void set_timer(int t)
{
    timer = t;
}
int get_timer()
{
    return timer;
}
//----- interrupt handler -----------------------------
//  irq (sr_core irq pin - vector #1)

static void (*timer_irqh_s)(void) = 0;
static void (*timer_irqh)(void) = 0;
static void (*user_irqh)(void) = 0;
static void (*user_irqh1)(void) = 0;
static void (*user_irqh2)(void) = 0;

static void timer_handl(void)
{
    if(csrr(mip) & MTIE){
        set_port(7);
        *mtimecmp += _br;
        if(timer_irqh_s)
            (*timer_irqh_s)(); // 1ms system handler call
        if(timer_irqh)
            (*timer_irqh)(); // 1ms user handler call
        timer++;
        tick = 1;
        csrs(mie, MTIE);
        //	csrc(mip, MTIE);
        reset_port(7);
    }
}

void irq_handler(void)
{
    timer_handl();
    if(csrr(mip) & MEIE){
        txirq_handl();
        if(user_irqh)  (*user_irqh)();
        if(user_irqh1) (*user_irqh1)();
        if(user_irqh2) (*user_irqh2)();
        csrs(mie, MEIE);
    }
}

void add_timer_irqh_sys(void (*irqh)(void))
{
    timer_irqh_s = irqh;
}
void add_timer_irqh(void (*irqh)(void))
{
    timer_irqh = irqh;
}
void add_user_irqh(void (*irqh)(void))
{
    user_irqh = irqh;
}
void add_user_irqh_1(void (*irqh)(void))
{
    user_irqh1 = irqh;
}
void add_user_irqh_2(void (*irqh)(void))
{
    user_irqh2 = irqh;
}
void remove_timer_irqh(void)
{
    timer_irqh = 0;
}
void remove_timer_irqh_sys(void)
{
    timer_irqh_s = 0;
}
void remove_user_irqh(void)
{
    user_irqh = 0;
}
void remove_user_irqh_1(void)
{
    user_irqh1 = 0;
}
void remove_user_irqh_2(void)
{
    user_irqh2 = 0;
}

//===== 2012/08/11 memif/sr_cache control

void d_cache_flush()
{
        *CACHECTRL = 2; // flush d-cache
        while(*CACHECTRL);
}

void d_cache_clean()
{
        *CACHECTRL = 1; // clean d-cache
        while(*CACHECTRL);
}

void i_cache_clean()
{
        *CACHECTRL = 4; // clean i-cache
        while(*CACHECTRL);
}

void cache_flush()
{
        *CACHECTRL = 7; // flush and clean all
        while(*CACHECTRL);
}

static time_t rtc2time(u16 rtc[])
{
        time_t ltime;
        ltime.year = (2000+((rtc[3]>>4)&0x3)*10+(rtc[3]&0xf)) - 1980;
        ltime.month = ((rtc[3]>>12)&0x1)*10+((rtc[3]>>8)&0xf);
        ltime.day = ((rtc[2]>>4)&0x3)*10+(rtc[2]&0xf);
        ltime.hour = ((rtc[1]>>4)&0x3)*10+(rtc[1]&0xf);
        ltime.min = ((rtc[1]>>12)&0x7)*10+((rtc[1]>>8)&0xf);
        ltime.sec = (((rtc[0]>>4)&0x7)*10+(rtc[0]&0xf));
        return ltime;
}

//#define HAS_RTC

time_t get_time()
{
        time_t ltime;
#ifdef HAS_RTC
        u16 rtc[4];
//        i2c_read(RTC, 0, rtc, 4);
        i2c_read(RTC, 0, rtc, 4);
        ltime = rtc2time(rtc);
#else
        ltime.year = 2018-1980;
        ltime.month = 9;
        ltime.day = 12;
        ltime.hour = 0;
        ltime.min = 0;
        ltime.sec = 0;
#endif
        return ltime;
}
