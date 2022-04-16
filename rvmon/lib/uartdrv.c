//
// uartdrv.c
//  rv_sio uart control
//

#include "stdio.h"
#include "ulib.h"

//	rv_sio interface

#define brate(br)       ((int)((float)f_clk / (br) + 0.5f))

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
}

#ifdef TXIRQ
static char txbuf[256];
static u8 txwpt = 0, txrpt = 0;
static u8 txinte = 0;
//static char rxbuf[256];
//static u8 rxinte = 0;
//static volatile u8 rxwpt = 0, rxrpt = 0;
//static u8 xoff = 0;

void txirq_handl()
{
	if(*SIOFLG & 0x4){	// txirq
		*SIOTRX = txbuf[++txrpt];	// pop txbuf
		if(txwpt == txrpt){		// tx empty
			txinte = 0;
			*SIOFLG = txinte;	// tx int disable
		}
	}
}
#else
void txirq_handl(){}
#endif

unsigned char uart_rx(void)
{
	char c;

	while(*SIOFLG & 1);     // while rx fifo empty
	c = *SIOTRX;
	while(!(*SIOFLG & 1)){	// while not empty
		*SIOFLG = 1;    // inc fifo pointer
	}
	return c;
}
int uart_rx_ready(void)
{
	return !(*SIOFLG & 1);
}

void uart_tx(unsigned char data)
{
	while(*SIOFLG & 2);     // while tx fifo full
	*SIOTRX = data;
}

//--------- mini stdio --------------------------------------------

void uart_putc(char c)
{
#ifdef TXIRQ
	unsigned char res = txrpt - txwpt;

	if(res > 7){	// txbuf full
		while(*SIOFLG & 2);	// while tx fifo full
	}
	txbuf[++txwpt] = c;	// push txbuf
	txinte = 8;
	*SIOFLG = txinte;
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

