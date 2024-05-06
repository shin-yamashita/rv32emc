
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/*---
#define csrw(csr, val) ({asm volatile ("csrw "#csr" , %0" :: "rK"(val));})
#define csrr(csr)      ({uint32_t __tmp;asm volatile ("csrr %0, "#csr : "=r"(__tmp)); __tmp;})
#define csrs(csr, val) ({asm volatile ("csrs "#csr" , %0" :: "rK"(val));})

#define csrwi(csr, imm) ({asm volatile ("csrw "#csr" , %0" :: "i"(imm));})
#define csrsi(csr, imm) ({asm volatile ("csrs "#csr" , %0" :: "i"(imm));})
---*/

#include "ulib.h"

void spi_config(int br, int rxph){
  *SPITXR = br-1;
  *SPIRPH = rxph;
}

void spi_trx(u8 *txbuf, u8 *rxbuf, int n){
  int i;
  *SPISCS = 0x0;  // 
  for(i = 0; i < n; i++){
    *SPITXD = txbuf[i];
  }
  while((*SPISCS & 0x2))  // txen == 1
    ;
  *SPISCS = 0x3;  // reset fifo pointer
  *SPISCS = 0x1;
  for(i = 0; i < n; i++){
    rxbuf[i] = SPIRXD[i];
  }
}


int main(void) 
{
 char *msg = "Hello world!";
 int i;

//  init_timer(100000);	// 10 us
/*
  printf("mtvec : %x\n", csrr(mtvec));
  printf("mtime : %d\n", csrr(time));
  set_port(1);
//  init_timer(50000);	// 20 us
//  set_port(3);

  for(i = 0; i < 5; i++){
//    *mtimecmp = *mtime + 1000;
//    printf("%d ", (uint32_t)(*mtime));
    printf("%d: %s  %d\n", i, msg, csrr(time));
    if(i%2) set_port(4);
    else    reset_port(4);
  }
*/
// spi test
  u8 txbuf[16], rxbuf[16];

  for(i = 0; i < 16; i++) txbuf[i] = i + 1;
  int Ntx = 5;

  int p, r;
  for(r = 4; r < 7; r++){
    for(p = 0; p < r; p++){
      spi_config(r, p);

      for(i = 0; i < Ntx; i++) txbuf[i] = i + r;

      set_port(0);
      spi_trx(txbuf, rxbuf, Ntx);

      //printf("spi tx done. twp:%d\n", *SPITXD);
      set_pout(0xff);
      int chk;
      chk = 0;
      for(i = 0; i < Ntx; i++){
        //printf("rx%d : %2x\n", i, SPIRXD[i]);
        //set_pout(rxbuf[i]);
        if(rxbuf[i] != i+r) {
          chk++;
          set_port(1);
        }else{
          reset_port(1);
        }
      }
      set_pout(0xff);
      set_pout(chk);
    }
  }
  set_pout(0);
  set_port(3);
  

  return 0; 
}

