
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
 char *msg = "Hello world!";
 int i;

//  init_timer(100000);	// 10 us
  set_port(0);
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
  return 0; 
}

