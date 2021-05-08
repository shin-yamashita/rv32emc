
#include <stdio.h>
#include <stdint.h>

#define csrw(csr, val) ({asm volatile ("csrw "#csr" , %0" :: "rK"(val));})
#define csrr(csr)      ({uint32_t __tmp;asm volatile ("csrr %0, "#csr : "=r"(__tmp)); __tmp;})
#define csrs(csr, val) ({asm volatile ("csrs "#csr" , %0" :: "rK"(val));})

#define csrwi(csr, imm) ({asm volatile ("csrw "#csr" , %0" :: "i"(imm));})
#define csrsi(csr, imm) ({asm volatile ("csrs "#csr" , %0" :: "i"(imm));})

#include "ulib.h"

int main(void) 
{
 char *msg = "Hello world!\n";
 int i;

 uint32_t val = 0x180;
 
 printf("mtvec : %x\n", csrr(mtvec));
 int r = csrr(time);
 printf("mtime : %d\n", r);

 asm volatile ("ebreak");
 asm volatile ("uret");
 asm volatile ("mret");

 csrsi(mip, 0x8);

 for(i = 0; i < 5; i++){
   printf("%d ", (uint32_t)(*mtime));
   printf("%d: %s", i, msg);
 }
 return 0; 
}

