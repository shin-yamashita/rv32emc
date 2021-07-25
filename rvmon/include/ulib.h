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

#define __START	__attribute__ ((__section__ (".start"))) 
#define __SRAM	__attribute__ ((__section__ (".sram"))) 
#define __DRAM	__attribute__ ((__section__ (".dram"))) 

#define DRAMTOP	((volatile u8*)0x20000000)
#define SUBTOP	((volatile u8*)0x10000000)

#define mtime	 ((volatile u64*)0xffff8000)
#define mtimecmp ((volatile u64*)0xffff8008)

//	para port
#define POUT    ((volatile u8*)0xffff0000)
#define GPIO    ((volatile u8*)0xffff0004)
#define PMPORT  ((volatile u8*)0xffff0005)	// b0:pwren/pwrsw
#define TEMP    ((volatile u16*)0xffff0002)	// Artix-7 XADC ch0 temp data
#define VBATT   ((volatile u16*)0xffff0006)	//         XADC ch3 VP/VN (Vbatt sens)

//      sr_adc interface
#define LEDEN	((volatile unsigned char *)0xffff00c0)	// b0:leaden
#define LEDTGL	((volatile unsigned short *)0xffff00ca)	// b0:led toggle(500Hz)

//      sr_timer interface
#define TIMERBR   (volatile unsigned *)0xffff0040
#define TIMERBRC  (volatile unsigned *)0xffff0044
#define TIMERCTL  (volatile char *)0xffff0048

//      sr_sio interface
#define SIOTRX  (volatile char *)0xffff0020
#define SIOFLG  (volatile char *)0xffff0021
#define SIOBR   (volatile short *)0xffff0022

//      sr_sio2 interface
#define SIO2TRX  (volatile char *)0xffff0120
#define SIO2FLG  (volatile char *)0xffff0121
#define SIO2BR   (volatile short *)0xffff0122

#if 0
//      sr_sio3 interface
#define SIO3TRX  (volatile char *)0xffff0160
#define SIO3FLG  (volatile char *)0xffff0161
#define SIO3BR   (volatile short *)0xffff0162

//	RTC (i2c)
#define I2C_CMD	 ((volatile u8 *)0xffff0141)
#define I2C_RADR ((volatile u8 *)0xffff0142)
#define I2C_CTRL ((volatile u8 *)0xffff0143)
#define I2C_SADR ((volatile u8 *)0xffff0144)
#define I2C_DATA ((volatile u16 *)0xffff0148)

//	dist1 VL53L0X, SI1153 FR/RR (i2c_1)
#define I2C1_CMD  ((volatile u8 *)0xffff00e1)
#define I2C1_RADR ((volatile u8 *)0xffff00e2)
#define I2C1_CTRL ((volatile u8 *)0xffff00e3)
#define I2C1_SADR ((volatile u8 *)0xffff00e4)
#define I2C1_DATA ((volatile u16 *)0xffff00e8)

//	dist2 Si1153 FL/RL (i2c_2)
#define I2C2_CMD  ((volatile u8 *)0xffff0101)
#define I2C2_RADR ((volatile u8 *)0xffff0102)
#define I2C2_CTRL ((volatile u8 *)0xffff0103)
#define I2C2_SADR ((volatile u8 *)0xffff0104)
#define I2C2_DATA ((volatile u16 *)0xffff0108)

//	dist3 Si1153 TOP (i2c_3)
#define I2C3_CMD  ((volatile u8 *)0xffff0121)
#define I2C3_RADR ((volatile u8 *)0xffff0122)
#define I2C3_CTRL ((volatile u8 *)0xffff0123)
#define I2C3_SADR ((volatile u8 *)0xffff0124)
#define I2C3_DATA ((volatile u16 *)0xffff0128)

//	OV5647 sensor SCCB  
#define I2C4_CMD  ((volatile u8 *)0xffff0241)
#define I2C4_CTRL ((volatile u8 *)0xffff0243)
#define I2C4_SADR ((volatile u8 *)0xffff0244)
#define I2C4_DATA ((volatile u8 *)0xffff0248)
#define I2C4_RADR ((volatile u16 *)0xffff025a)
#endif

//	memif/sr_chache
#define CACHECTRL       ((volatile u8 *)0xffff0180)	// i-cl(2) d-fl(1) d-cl(0)
#define MEMFRM		((volatile u8 *)0xffff0184)	// wr_inh(7) frame(4:0)
#define MEMCTRL		((volatile u8 *)0xffff0185)	// intflg(3:2) inten(1:0)  vd int

#if 0
//	snsif
#define SNS_HSIZE	((volatile u16 *)0xffff0220)	// 2560/1280/640/320
#define SNS_VSIZE	((volatile u16 *)0xffff0222)	// 1920/960/480/240
#define SNS_IRQPOS	((volatile u16 *)0xffff0224)	// 0:VSIZE
#define SNS_FRMSIZE	((volatile u16 *)0xffff0226)	// 4800/1200/300/75 k
#define SNS_FRAME	((volatile u16 *)0xffff0228)	// 0:27/108/433/1728
#define SNS_CTRL	((volatile u8 *)0xffff022b)	// 0:capten 1:s_srst 2:s_pwdn
#define SNS_CAVE	((volatile s32 *)0xffff022c)	// r-y average
#define SNS_YAVE	((volatile u32 *)0xffff0230)	// y average
#define SNS_YTH		((volatile u8 *)0xffff0237)	// y average thresh
#define SNS_YGAIN	((volatile u8 *)0xffff0238)	// ygain 64
#define SNS_CGAIN	((volatile u8 *)0xffff0239)	// cgain 255
#define SNS_YPEAK	((volatile u8 *)0xffff023a)	// y peak
#define SNS_CPEAK	((volatile u8 *)0xffff023b)	// c peak
#endif

//	spi
#define SPITRX		((volatile u8 *)0xffff01a0)	// tx, rx byte
#define SPIFLG  	((volatile u8 *)0xffff01a1)	// 1:txfull 0:rxempty, w:inc readpt
#define SPISCS  	((volatile u8 *)0xffff01a2)	// 0:scs
#define SPIBR	  	((volatile u8 *)0xffff01a3)	// br: 
#define SPITRXBLK	((volatile u8 *)0xffff01a4)	// tx, rx byte , blocking
#define SPIRXBLK	((volatile u8 *)0xffff01a8)	// rx byte , blocking then tx

//	mspi imu
#define MSPI1TRX	((volatile u8 *)0xffff0160)	// tx, rx byte (16 byte)
#define MSPI1NB  	((volatile u8 *)0xffff0170)	// nbytes
#define MSPI1SCS  	((volatile u8 *)0xffff0172)	// 0:scs
#define MSPI1BR	  	((volatile u8 *)0xffff0173)	// br: 

//	mspi fl
#define MSPI2TRX	((volatile u8 *)0xffff01e0)	// tx, rx byte (16 byte)
#define MSPI2NB  	((volatile u8 *)0xffff01f0)	// nbytes
#define MSPI2SCS  	((volatile u8 *)0xffff01f2)	// 0:scs
#define MSPI2BR	  	((volatile u8 *)0xffff01f3)	// br: 

#if 0
//	upwm	ffff0060
#define UPWMCTRL	((volatile u8 *)0xffff0060)	// men(1:0)
#define UPWMBR		((volatile u16 *)0xffff0062)	// br(10:0)
#define UPWMDUTY1	((volatile u16 *)0xffff0064)	// duty1(10:0)
#define UPWMDUTY2	((volatile u16 *)0xffff0066)	// duty2(10:0)
#define UPWMDUTY3	((volatile u16 *)0xffff0068)	// duty3(10:0)
#define UPWMDUTY4	((volatile u16 *)0xffff006a)	// duty4(10:0)
#endif

//	dmac
#define DMASRCPT        ((volatile u32*)0xffff01c0)	// u8* source pointer
#define DMADSTPT        ((volatile u32*)0xffff01c4)	// u8* destination pointer
#define DMALEN          ((volatile u16*)0xffff01ca)	// u16 length (byte)
#define DMACMD          ((volatile u8*) 0xffff01cc)	// bit0: start dma


//	tpmf	triple phase motor driver front
#define TPMF_BR        	((volatile u16*)0xffff0062)	// PWM rate (480=100kHz) 
#define TPMF_MEN        ((volatile u8*)0xffff0064)	// 1:Motor Enable 
#define TPMF_VLIM       ((volatile u8*)0xffff0066)	// Vlim 120:100%  PWM max
#define TPMF_AMP1       ((volatile u8*)0xffff0065)	// motor1 amplitude 240:100%
#define TPMF_AMP2       ((volatile u8*)0xffff0067)	// motor2 amplitude 240:100%
#define TPMF_PHI1       ((volatile s16*)0xffff0068)	// motor1 phase u10
#define TPMF_FREQ1      ((volatile s16*)0xffff006a)	// motor1 freq 0.089Hz/LSB s13 -4096:4095
#define TPMF_PHI2       ((volatile s16*)0xffff006c)	// motor2 phase u10
#define TPMF_FREQ2      ((volatile s16*)0xffff006e)	// motor2 freq 0.089Hz/LSB s13 -4096:4095
#define TPMF_MPOS1      ((volatile s32*)0xffff0070)	// motor1 position
#define TPMF_MPOS2      ((volatile s32*)0xffff0074)	// motor2 position

//	tpmr	triple phase motor driver rear
#define TPMR_BR        	((volatile u16*)0xffff0082)	// PWM rate (480=100kHz) 
#define TPMR_MEN        ((volatile u8*)0xffff0084)	// 1:Motor Enable 
#define TPMR_VLIM       ((volatile u8*)0xffff0086)	// Vlim 120:100%  PWM max
#define TPMR_AMP1       ((volatile u8*)0xffff0085)	// motor1 amplitude 240:100%
#define TPMR_AMP2       ((volatile u8*)0xffff0087)	// motor2 amplitude 240:100%
#define TPMR_PHI1       ((volatile s16*)0xffff0088)	// motor1 phase u10
#define TPMR_FREQ1      ((volatile s16*)0xffff008a)	// motor1 freq 0.089Hz/LSB s13 -4096:4095
#define TPMR_PHI2       ((volatile s16*)0xffff008c)	// motor2 phase u10
#define TPMR_FREQ2      ((volatile s16*)0xffff008e)	// motor2 freq 0.089Hz/LSB s13 -4096:4095
#define TPMR_MPOS1      ((volatile s32*)0xffff0090)	// motor1 position
#define TPMR_MPOS2      ((volatile s32*)0xffff0094)	// motor2 position

// sr_adctrl
#define ADCTRL        	((volatile u16*)0xffff00a0)	// sr_adctrl base
#define TOF_LEDENAC     ((volatile u8*)0xffff00a0)	// ledena 1:0 D1:D2
#define TOF_LEDENBC     ((volatile u8*)0xffff00a1)	// ledenb 1:0 D1:D2
#define TOF_DTENC       ((volatile u8*)0xffff00a2)	// dten   1:0 D1:D2
#define TOF_PSCTL       ((volatile u8*)0xffff00a3)	// psctl  2:0 psincdec,psen,psclk
#define ADC_STEER       ((volatile u16*)0xffff00a4)	// steer adc u14
#define ADC_AD0         ((volatile u16*)0xffff00a8)	// tof adc u12
#define ADC_AD1         ((volatile u16*)0xffff00aa)	// tof adc u12
#define ADC_AD2         ((volatile u16*)0xffff00ac)	// tof adc u12
#define ADC_AD3         ((volatile u16*)0xffff00ae)	// tof adc u12


//      ulib.c function prototypes

void set_leden(int en);
int get_pout();
void set_pout(int d);           // direct set 8bit
void set_port(int bit);         // bit set
void reset_port(int bit);       // bit reset
int get_port();

void init_encoder();
void set_tpmth(int th0, int th1, int th2, int th3);

float get_temp();	// get FPGA temperature (deg)
float get_vbatt();	// get battery voltage (V)

void init_timer(int br);
void disable_timer();
void enable_timer();

void set_expose(int exp);
//void timer_ctrl(void);
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

// rcp.c
float rcp(float x);	// reciprocal  return 1/x

// sincos.c
float fsin(float x);
float fcos(float x);
void fsincos(float x, float *s, float *c);

// fsqrt.c
float invsqrt(float a);
float fsqrt(float x);

// from srmon.c
void getstr(char *str);
unsigned char asc2hex(int c);
unsigned int str2u32(char *s);

#include "uartdrv.h"
//#include "mmcdrv.h"

// clear bss section
extern u32 _bss_start, _end;
#define	zero_bss()	{u32 *p;for(p=&_bss_start;p<&_end;*p++=0);}
// get stack pointer
static inline u32 get_sp(){u32 sp;__asm__("mv %0,sp" : "=r" (sp));return sp;}

//====== 2012/08/11 memif/sr_cache
void d_cache_flush();
void d_cache_clean();
void i_cache_clean();
void cache_flush();

//------------------------------------------------------------------
// CPU clock frequency (Hz)
//#define f_clk	((float)100e6)
#define f_clk	((float)60e6)
//#define f_clk	((float)48e6)
//#define f_clk	42e6
//#define f_clk	36e6
//#define f_clk	30e6

//====== 2013/09/07 i2cdrv.c
//#include "i2cdrv.h"

////====== 2013/10/16 gyro.c  --> mmlib 161123
//#include "gyro.h"

_END_STD_C

#endif  // _ULIB_H
