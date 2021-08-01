
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

int main(void) 
{
 char *msg = "Hello world!\n";
 char *str = "uart test\n";
 int i;

 uint32_t val = 0x180;

  init_timer(100000);	// 10 us
//  uart_set_baudrate(0);
//  set_port(0);
//  uart_write(0, str, strlen(str));
//  set_port(1);

//  char *d;
//  d = (char*)0x3001;
//  set_port(2);
//  memclr(d, 106);
//  reset_port(2);

  printf("mtvec : %x\n", csrr(mtvec));
 int r = csrr(time);
//  printf("mtime : %d\n", r);
//  set_port(2);
//  init_timer(50000);	// 20 us
  set_port(3);

  for(i = 0; i < 5; i++){
//    *mtimecmp = *mtime + 1000;
//    printf("%d ", (uint32_t)(*mtime));
    printf("%d: %s", i, msg);
    if(i%2) set_port(4);
    else    reset_port(4);
  }
  return 0; 
}

