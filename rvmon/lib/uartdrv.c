//
// uartdrv.c
//  mm6 uart control
//

#include "stdio.h"
#include "ulib.h"

//	sr_sio interface

#define brate(br)       ((int)(f_clk / (br) + 0.5f))

void uart_set_baudrate(int sel)
{
    //#ifndef DEBUG
    //      *SIOBR = 5;     // for simulation
    if(sel == 0){
        *SIOBR = brate(2000000);	// cp2104 max speed
    }else if(sel == 1){
        *SIOBR = brate(115200);		// ESP WiFi
    }else{
        *SIOBR = brate(sel);
    }
    //	*SIOBR = brate(230400);
    //	*SIOBR = brate(115200);
    //      *SIOBR = brate(57600);
    //      *SIOBR = brate(38400);
    //      *SIOBR = brate(19200);
    //      *SIOBR = brate(9600);

    //	*SIO2BR = brate(921600);	// RN-171 fastest
    //	*SIO2BR = brate(460800);
    //	*SIO2BR = brate(230400);
    //	*SIO2BR = brate(115200);	// TWE-001 default
    //#endif
}

void select_uart(int sel)
{
    if(sel){	// wireless
        uart_set_baudrate(1);
        *GPIO |= 0x1;
    }else{		// wired
        uart_set_baudrate(0);
        *GPIO &= ~0x1;
    }
}

#ifdef TXIRQ
#define RXIRQ

#define TXINTE  0x8 //
#define TXINT   0x4 // not txfull
#define TXFULL  0x2 // tx full
#define RXINTE  0x20
#define RXINT   0x10    // not rx empty
#define RXEMP   0x1 // rx empty

// SIOFLG  rxinte rxirq txinte txirq txf rxe

static char txbuf[256];
static u8 txwpt = 0, txrpt = 0;
static u8 txinte = 0;

static char rxbuf[256];
static u8 rxinte = 0;
static volatile u8 rxwpt = 0, rxrpt = 0;
//static u8 xoff = 0;

void txirq_handl()
{
    u8 flg = *SIOFLG;
    set_port(1);
    if(flg & TXINT){	// txirq
        *SIOTRX = txbuf[++txrpt];	// pop txbuf
        if(txwpt == txrpt){		// tx empty
            txinte = 0;
            *SIOFLG = txinte | rxinte;	// tx int disable
        }
    }
#ifdef RXIRQ
    if(flg & RXINT){
        rxbuf[rxwpt++] = *SIOTRX;
        if(rxwpt == rxrpt) rxwpt--; // rx full
        *SIOFLG = txinte | rxinte | RXEMP;  // increment rxfifo
    }
#endif
    reset_port(1);
}
#else
void txirq_handl(){}
#endif

unsigned char uart_rx(void)
{
    char c;
#ifdef RXIRQ
    rxinte = RXINTE;
    *SIOFLG = txinte | rxinte;
    enable_irq();
    set_port(2);
    while(rxwpt == rxrpt);  // while rxbuf empty
    c = rxbuf[rxrpt++];
    reset_port(2);
#else
    while(*SIOFLG & 1);     // while rx fifo empty
    c = *SIOTRX;
    while(!(*SIOFLG & 1)){	// while not empty
        *SIOFLG = 1;    // inc fifo pointer
    }
#endif
    return c;
}
int uart_rx_ready(void)
{
#ifdef RXIRQ
    return !(rxwpt == rxrpt);
#else
    return !(*SIOFLG & 1);
#endif
}

void uart_tx(unsigned char data)
{
    while(*SIOFLG & TXFULL);     // while tx fifo full
    *SIOTRX = data;
}

//--------- mini stdio --------------------------------------------

void uart_putc(char c)
{
#ifdef TXIRQ
    unsigned char res = txrpt - txwpt;

    if(res > 7){	// txbuf full
        while(*SIOFLG & TXFULL);	// while tx fifo full
    }
    txbuf[++txwpt] = c;	// push txbuf
    txinte = TXINTE;
    *SIOFLG = txinte | rxinte;
    enable_irq();   // set mie[11]

#else
    uart_tx(c);
#endif
}

ssize_t uart_read(int fd, char *buf, size_t count)
{
    int n = count;
    while(n--){
        *buf++ = uart_rx();
    }
    return count;
}

ssize_t uart_write(int fd, const char *buf, size_t count)
{
    int n = count;
    while(n--){
        uart_putc(*buf++);
    }
    return count;
}

