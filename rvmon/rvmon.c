//
// srmon  monitor program for mm6
// 2007/11/27
// 2010/06/27
// 2011/03/10 bluetooth uart
// 2015/10/   mm8
// 2016/01/   sbin
//

#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ulib.h"
#include "time.h"
#include "readline.h"

#define limit(x, min, max)	((x)<(min)?(min):((x)>(max)?(max):(x)))

#define UBASE	0x4800

#define ETB	'\027'	// End of Transfer Block
#define EOT	'\004'	// End of Transfer
#define XON	'\021'	// X-on
#define NUL	'\0'	// NULL
#define ETX	'\003'	// End of Text

static u8 *ram = 0x00000000;

//============
// IRQ Handler
//============
static u32 ms_timer = 0;
int get_ms_timer()
{
    return ms_timer;
}
static void timer_1ms(void)
{
//    ms_timer = 1000 > ms_timer ? ms_timer + 1 : 1;
    ms_timer++;
    if(ms_timer >= 1000){  // 1 sec
        ms_timer = 0;
    }
    switch(ms_timer / 100){
    case 0:
    case 2:
        set_port(0);
        break;
    default:
        reset_port(0);
        break;
    }
}

u8 *dump(u8 *pt, int nbyte)
{
    int i, j;
//	fprintf(fp, "adr : %x\n", adr);
//
//	for(i = 0; i < 512; i+=16){
    for(i = 0; i < nbyte; i+=16){
        printf("%08x: ", (int)pt);

        for(j = 0; j < 16; j++){
            printf("%02x ", pt[j]);
        }
        for(j = 0; j < 16; j++){
            char c = pt[j];
            fputc(c >= 0x20 && c < 0x7f ? c : '.', stdout);
        }
        fputc('\n', stdout);
        pt += 16;
    }
    fputc('\n', stdout);
    return pt;
}

//==============================================
// Get Byte from Rx
//==============================================
unsigned char asc2hex(int c)
{
    if(c >= '0' && c <= '9') return c - '0';
    if(c >= 'A' && c <= 'F') return c - 'A' + 10;
    if(c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}
unsigned int str2u32(char *s)
{
    int c;
    u32 d = 0;
    while((c = *s++)){
        d <<= 4;
        d |= asc2hex(c);
    }
    return d;
}

int isBlank(char *s)
{
    int c;
    while((c = *s++)){
        if(!isspace(c)) return 0;
    }
    return 1;       // blank
}

void memwrite(u32 adr, u8 dat)
{
    ram[adr] = dat;
//    printf("a:%6x %02x\n", adr, dat);
}

#define EFMT	-2

//
// load binarized S-fmt file to memory
//
int get_byte(){
    int c;
    c = asc2hex(getchar()) << 4;
    c |= asc2hex(getchar());
    return c;
}
static int load_S(int wen)	// wen 1: write memory
{
    int i, c, nadr, den;
    int nbytes = 0;
    u8 nb, sum;
    u32 addr;

    while(1){
        //if((c = fgetc(fp)) == EOF) break;	// normal end of file
        if((c = getchar()) == EOT) return EOF;
        //putchar(c);
        if(c == 'S'){
            if((c = getchar()) == EOT) return EOF;	//
            //putchar(c);
            nadr = den = 0;
            switch(c){	// check record type
            case '0': nadr = 2;	  break;
            case '1': nadr = 2; den = 1; break;
            case '2': nadr = 3; den = 1; break;
            case '3': nadr = 4; den = 1; break;
            case '7': nadr = 4;	  break;
            case '8': nadr = 3;	  break;
            case '9': nadr = 2;	  break;
            default: return EFMT;	// s-format err
            }
            c = get_byte();	// n-byte
            //printf(" %2x ", c);
            sum = nb = c;
            addr = 0;
            for(i = 0; i < nb-1; i++){
                c = get_byte();
                //printf("%2x", c);
                sum += c;
                if(nadr){	// addr field
                    addr = (addr << 8) | c;
                    nadr--;
                }else if(den){		// data
                    if(wen) memwrite(addr, c);
                    addr++;
                    nbytes++;
                }
            }
            c = get_byte(); // cs
            //printf(" %2x:%2x\n", c, sum^0xff);
            sum = ~sum;
            if(sum != c){	// cs err
                printf(" cs err: %2x %2x ", sum, c);
                return EFMT;
            }
        }
    }
    return nbytes;
}

//static int argc = 1;
//static char *argv[10];  // maxarg : 10
#define MAXARG  10

int get_args(char *str, char *argv[])
{
    char *tok;
    int argc = 1;
    argv[0] = strtok(str, " \t\n");
    while(argc < MAXARG && (tok = strtok(NULL, " \t\n")) != NULL){
        argv[argc++] = tok;
    }
    return argc;
}

int main(void)
{
    int i, c;
    static char prmpt[30];
    char *argstr;
    int argc;
    static char *argv[MAXARG];
    int (*usrfunc)(int argc, char *argv[]);
    u32 dumpadr = 0;

    uart_set_baudrate(115200);

    init_timer(1000);   // 1000 ms 1kHz
    add_timer_irqh_sys(timer_1ms);

    printf("rvmon loaded.\n");

    while(1){
        add_timer_irqh_sys(timer_1ms);
        //sprintf(prmpt, "%4.1f' %4.2fV rvmon$ ", (get_temp()), (get_vbatt()));	//m8 XADC
        sprintf(prmpt, "rvmon$ ");
        argstr = readline(prmpt);
        putchar('\n');
        argc = get_args(argstr, argv);
        if(!strcmp("l", argv[0])){	// Download (ascii)file and save it to MMC
            if(argc > 1){
                printf("\033<%s\n", argv[1]);
                load_S(1);
            //    sprintf(cfn, "/bin/%s", argv[1]);
            //    save_file(fp, cfn, argv[1], 1); // load Sbin

            }
        }else if(!strcmp("d", argv[0])){
            for(i = 1; i < argc; i++){
                if(!strcmp(argv[i], "-w")){

                }else{
                    dumpadr = str2u32(argv[i]);
                }
            }
            dumpadr = (u32)dump((u8*)dumpadr, 256);
        }else if(!strcmp("help", argv[0])){
            printf(//	"    ll  {fn}\n"
                "    d  {addr}      : dump memory\n"
                "    l  {cmd(.mot)} : load S-record file\n"
                "    go             : exec cmd (@0x%04x)\n", UBASE
            );
        }else if(!strcmp("go", argv[0])){
            usrfunc = (void (*)) UBASE;
            printf("run user func : %4x\n", (int)usrfunc);
        //    i_cache_clean();
            c = (* usrfunc)(argc, argv) ;
            printf("%d\n", c);
        }
        remove_timer_irqh();
        remove_user_irqh();
    }
}


