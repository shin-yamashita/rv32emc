//======================================================
// ulib.h
//  library for mm6 hardware control
//

#ifndef _ULIB_H
#define _ULIB_H

#include <_ansi.h>
#include <stdint.h>

_BEGIN_STD_C

#include "types.h"
#include "uartdrv.h"

//---- CSR access ----

#define csrw(csr, val) ({asm volatile ("csrw "#csr" , %0" :: "rK"(val));})
#define csrr(csr)      ({uint32_t __tmp;asm volatile ("csrr %0, "#csr : "=r"(__tmp)); __tmp;})
#define csrs(csr, val) ({asm volatile ("csrs "#csr" , %0" :: "rK"(val));})  // set
#define csrc(csr, val) ({asm volatile ("csrc "#csr" , %0" :: "rK"(val));})  // clear

#define csrwi(csr, imm) ({asm volatile ("csrw "#csr" , %0" :: "i"(imm));})
#define csrsi(csr, imm) ({asm volatile ("csrs "#csr" , %0" :: "i"(imm));})
#define csrci(csr, imm) ({asm volatile ("csrc "#csr" , %0" :: "i"(imm));})

#define MSIE    0x8
#define MTIE    0x80
#define MEIE    0x800
//----

#define mtime	 ((volatile u64*)0xffff8000)
#define mtimecmp ((volatile u64*)0xffff8008)

//	para port
#define POUT    ((volatile u8*)0xffff0000)

//      rv_sio interface
#define SIOTRX  ((volatile char *)0xffff0020)
#define SIOFLG  ((volatile char *)0xffff0021)
#define SIOBR   ((volatile short *)0xffff0022)

//      rv_xadcif  XADC interface
#define XADC    ((volatile u16 *)0xffff0040)  // 
// 0 0x0  temperature  
// 1 0x11 aux1  vsns5v0
// 2 0x12 aux2  vsnsvu
// 3 0x14 aux4 
// 4 0x15 aux5 
// 5 0x19 aux9  isns5v0
// 6 0x1a aux10 isns0v95

//      rv_pwm  12 channel LED pwm interface
#define PWM     ((volatile u8 *)0xffff0060)  // 
#define PWMPS   ((volatile u16 *)0xffff0070)    // prescaler

//      ulib.c function prototypes

int get_pout();                 // POUT port shadow register read
void set_pout(int d);           // direct set 8bit
void set_port(int bit);         // bit set
void reset_port(int bit);       // bit reset
int get_port();                 // direct read POUT port


void init_timer(int br);
void disable_timer();
void enable_timer();

void wait(void);	// wait 1 ms
void n_wait(int n);	// wait n ms
void set_timer(int t);	// set 1ms counter val
int get_timer();	// 
void enable_irq();   //

void irq_handler(void);
void add_timer_irqh_sys(void (*irqh)(void));
void add_timer_irqh(void (*irqh)(void));
void add_user_irqh(void (*irqh)(void));
void add_user_irqh_1(void (*irqh)(void));
void add_user_irqh_2(void (*irqh)(void));
void remove_timer_irqh_sys(void);
void remove_timer_irqh(void);
void remove_user_irqh(void);
void remove_user_irqh_1(void);
void remove_user_irqh_2(void);

// memcpy32		len : # of bytes
void memcpy32(u32 *dst, u32 *src, size_t len);	// dst, src : u32 aligned
void memcpydma(u8 *dst, u8 *src, size_t len);	// sr_dmac

// memclr.c
void *memclr(void *s, size_t n);

//#include "uartdrv.h"

// clear bss section
extern u32 _bss_start, _end;
#define	zero_bss()	{u32 *p;for(p=&_bss_start;p<&_end;*p++=0);}
// get stack pointer
static inline u32 get_sp(){u32 sp;__asm__("mv %0,sp" : "=r" (sp));return sp;}
static inline u32 get_gp(){u32 gp;__asm__("mv %0,gp" : "=r" (gp));return gp;}

//------------------------------------------------------------------
// CPU clock frequency (Hz)
//#define f_clk	100e6f
#define f_clk	60e6f
//#define f_clk	48e6f
//#define f_clk	42e6f
//#define f_clk	36e6f
//#define f_clk	30e6f

_END_STD_C

#endif  // _ULIB_H
