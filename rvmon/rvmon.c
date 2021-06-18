//
// rvmon.c
// 2021/06/12 monitor program for rv_core
//
#include "stdio.h"
#include <ctype.h>
#include "ulib.h"

#define ETB     '\027'		// End of Transfer Block
#define EOT     '\004'		// End of Transfer
#define XON     '\021'		// X-on
#define NUL     '\0'		// NULL


void timer_1ms()
{
    static int timer = 0;

    timer++;
    timer %= 1000;
    switch(timer / 100){
    case 0:
    case 2:
        set_port(0);
        break;
    default:
        reset_port(0);
        break;
    }
}

unsigned char *ram = 0;
//unsigned char ram[32768];

unsigned int dump(unsigned int adr)
{
    int i, j;

    for(i = 0; i < 256; i+=16){
        printf("%08x", adr);
        putchar(':');

        for(j = 0; j < 16; j++){
            putchar(' ');
            printf("%02x", ram[adr+j]);
        }
        putchar(' ');
        for(j = 0; j < 16; j++){
            char c = ram[adr+j];
            putchar(c >= 0x20 && c < 0x7f ? c : '.');
        }
        putchar('\n');

        adr+=16;
    }
    putchar('\n');
    return adr;
}

//-- srec()  read S-record

int ctoh(char c){
    return (isxdigit(c) ? (c - 'a' < 0 ? (c - 'A' < 0 ? c - '0' : c - 'A'+10) : c - 'a'+10) : 0);
}

int get_byte()
{
    char c0 = getchar();
    char c1 = getchar();
    return (ctoh(c0) << 4) | ctoh(c1);
}

void memwrite(u32 adr, u8 dat)
{
    ram[adr] = dat;
//    printf("a:%6x %02x\n", adr, dat);
}

int srec()
{
    int i, c, cs, count;
    u8 type, bc, dat;
    u32 adr;    // address
    int ab;     // address field bytes
    enum rec_type {Head, Data, Term, NA} dw;

    count = 0;
    while((c = getchar()) >= 0){
        if(c == 'S'){
            type = getchar();
            dw = NA;
            switch(type){
                case '0': ab = 2; dw = Head;    break;  // header
                case '1': ab = 2; dw = Data;    break;  // data,16bit addr
                case '2': ab = 3; dw = Data;    break;  // data,24bit addr
                case '3': ab = 4; dw = Data;    break;  // data,32bit addr
                case '5': ab = 2;               break;  // 16bit count
                case '6': ab = 3;               break;  // 24bit count
                case '7': ab = 4; dw = Term;    break;  // Term 32bit start addr
                case '8': ab = 3; dw = Term;    break;  // Term 24bit start addr
                case '9': ab = 2; dw = Term;    break;  // Term 16bit start addr
                default:  ab = 0; dw = Term;    break;  // N/A
            }
            adr = 0;
            cs = bc = get_byte();       // byte count : addr+data+cs
            for(i = 0; i < ab; i++){    // address field
                dat = get_byte();
                cs += dat;
                adr = (adr << 8) | dat;
            }
            for(; i < bc-1; i++){       // Data field
                dat = get_byte();
                cs += dat;
                if(dw == Data) {
                    memwrite(adr++, dat);
                    count++;
                } else if(dw == Head) putchar(dat);
            }
            dat = get_byte();   // cs
            // printf("cs: %d %2x %c\n", bc, dat, dat == ((~cs)&0xff) ? 'O':'X');
            if(dat != ((~cs)&0xff)) 
                printf("cs err %2x - %2x\n", dat, ((~cs)&0xff));    // checksum error
            putchar(XON);
            if(dw == Term) {
                printf(" %d bytes start:%x\n", count, adr);
                break;
            }
        }
    }
    return count;
}

unsigned int str2u32(char *s)
{
    int c;
    u32 d = 0;
    while((c = *s++)){
        d <<= 4;
        d |= ctoh(c);
    }
    return d;
}

void getstr(char *str)
{
    char c;
    int p = 0;
    do{
        c = getchar();
        switch(c){
        case '\r':
            break;
        case '\n':
            *str++ = '\0';
            putchar(c);
            break;
        case 0x7f:
        case '\b':
            if(p > 0){
                str--;
                p--;
                printf("\033[1D \033[1D");
            }
            break;
        default:
            *str++ = c;
            p++;
            putchar(c);
            break;
        }
    }while(c != '\n');
}

#define VECT0	(0x6000)

int main(void)
{
    char str[200];
    unsigned int adr = 0;
    void (* userfunc)(void);

    add_timer_irqh(timer_1ms);
    init_timer(1000);   // 1kHz 1ms
    uart_set_baudrate(0);

    while(1){
        printf("%4.1f'%4.2fV rvmon> ", fu(get_temp()), fu(get_vbatt()));       //m8 XADC
    //    printf("rvmon> ");
        getstr(str);
        switch(*str){
        case 'd':	// dump
            if(str[1]) adr = str2u32(&str[1]);
            adr = dump(adr);
            break;
        case 'l':	// load
            printf("\033<");	//esc <
            printf(&str[1]);
            putchar('\n');
            srec();
            break;
        case 'g':	// go
            if(str[1]) adr = str2u32(&str[1]);
            printf("=== Go to ");
            adr &= ~1;
            printf("%08x", adr);
            printf("\n");
            userfunc = (void (*)) adr;
            (* userfunc)();
            break;
        case 'r':	// reset go
            adr = VECT0;
            printf("=== Reset & go to ");
            printf("%08x", adr);
            printf("\n");
            userfunc = (void (*)) adr;
            (* userfunc)();
            break;
        case 'h':	// help
            printf("    d<adr>  : dump\n");
            printf("    l<file> : load S format file\n");
            printf("    g<adr>  : go to adr\n");
            printf("    r       : reset go\n");
            break;
        default:
            printf("??\n");
            break;
        }
    }
}


