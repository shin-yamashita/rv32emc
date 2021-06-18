
#include "ulib.h"

int main();


int __attribute__ ((section (".text.startup"))) crt0()
{
    zero_bss();
    return main();
}