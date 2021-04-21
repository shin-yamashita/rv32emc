
#include <stdint.h>
#include <stdio.h>

int main (int argc, char **argv)
{
    union
    {
        uint32_t b4 ;     /* 4byte    */
        uint16_t b2 [2] ; /* 2byte×2 */
        uint8_t b1 [4] ;  /* 1byte×4 */
    } bytes ;

    bytes.b4 = 0x12345678 ;
    printf ("bytes.b4: %08X\n", bytes.b4) ;
    printf ("bytes.b2: %04X, %04X\n", bytes.b2[0], bytes.b2[1]) ;
    printf ("bytes.b1: %02X, %02X, %02X, %02X\n", bytes.b1[0], bytes.b1[1], bytes.b1[2], bytes.b1[3]) ;
    return 0 ;
}

