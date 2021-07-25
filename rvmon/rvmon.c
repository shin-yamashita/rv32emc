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
#include "ff.h"
#include "save_data.h"
#include "readline.h"

#define limit(x, min, max)	((x)<(min)?(min):((x)>(max)?(max):(x)))

#define UBASE	0x8000

#define ETB	'\027'	// End of Transfer Block
#define EOT	'\004'	// End of Transfer
#define XON	'\021'	// X-on
#define NUL	'\0'	// NULL
#define ETX	'\003'	// End of Text

static u8 *ram = 0x00000000;

static save_data_t save_data;
save_data_t *get_save_data()
{
        return &save_data;
}

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
        update_rtc_1s();    // update calender time
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
#if 0
void fpath(char *path, char *cwd, char *fn){
    sprintf(path, "%s/%s", cwd, fn);
    return;
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

time_t cnvtime(u16 fdate, u16 ftime)
{
    union{
        time_t lt;
        u32 ft;
    } ltime;
    ltime.ft = (((u32)fdate<<16)|ftime)<<1;
    return ltime.lt;
}

void Tolower(char *str)
{
    char c;

    while((c = *str)){
        *str++ = tolower(c);
    }
    *str = 0;
}

int ls(char *dir)
{
    int fd = -1, err;
    FILINFO finfo;
    char str[130];
    time_t ltime;
    DIR dent;

    fd = f_opendir(&dent, dir);
    if(fd){
        printf(" fat can't open.\n");
    }else{
        for(;;){
            err = f_readdir(&dent, &finfo);
            if(err || !finfo.fname[0]) break; /* Error or end of dir */
            Tolower(finfo.fname);
            fpath(str, dir, finfo.fname);
            ltime = cnvtime(finfo.fdate, finfo.ftime);
            if(finfo.fattrib & AM_DIR)
                printf("   <dir>");
            else
                printf(" %7d", finfo.fsize);

            printf(" %4d/%02d/%02d %2d:%02d  %s%c\n",
                ltime.year+1980, ltime.month, ltime.day, ltime.hour, ltime.min,
                str, (finfo.fattrib & AM_DIR)?'/':' ');
        }
    }
    return 0;
}

#endif

static int date_valid = 0;

int date(int argc, char *argv[])
{
    int i, c, repeat = 0;
    time_t ltime;

    for(i = 1; i < argc; i++){
        if(!strcmp(argv[i], "-h")){
            printf(
                    "date <-h> <-r> <date str>\n"
                    "  -r      : pepeat read RTC.('q' for quit)\n"
                    "  datestr : set RTC. yyyy/mm/dd/hh:mm:ss\n"
            );
        }else if(!strcmp(argv[i], "-r")){
            repeat = 1;
        }else if(isdigit((u8)*argv[i])){
            ltime = set_strtime(argv[i]);
            date_valid = 1;
        }
    }
    while(1){
        ltime = get_ltime();
        printf(" %d/%d/%d %d:%d:%d\n",
                ltime.year+1980, ltime.month, ltime.day,
                ltime.hour, ltime.min, ltime.sec);
        if(!repeat) break;
        for(i = 0; i < 100; i++){   // wait 1000 ms
            if(uart_rx_ready()){
                c = getchar();
                if(c == 'q') return 0;
            }
            n_wait(10);
        }
    }
    return 0;
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
#if 0
void getstr(char *str)
{
    char c;
    int p = 0;
    do{
        c = getchar();
        switch(c){
        case '\r':      // ignore cr
            break;
        case '\n':      // lf
            *str++ = '\0';
            putchar(c);
            break;
        case 0x7f:
        case '\b':
            if(p > 0){
                str--;
                p--;
                printf("\b \b");
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
#endif

void memwrite(u32 adr, u8 dat)
{
    ram[adr] = dat;
//    printf("a:%6x %02x\n", adr, dat);
}

#define EFMT	-2

//
// load binarized S-fmt file to memory
//
static int load_sbin(FILE *fp, int wen)	// wen 1: write memory
{
    int i, c, nadr, den;
    int nbytes = 0;
    u8 nb, sum;
    u32 addr;

    while(1){
        if((c = fgetc(fp)) == EOF) break;	// normal end of file
        //putchar(c);
        if(c == 'S'){
            if((c = fgetc(fp)) == EOF) return EOF;	//
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
            if((c = fgetc(fp)) == EOF) return EOF;	// n-byte
            //printf(" %2x ", c);
            sum = nb = c;
            addr = 0;
            for(i = 0; i < nb-1; i++){
                if((c = fgetc(fp)) == EOF) return EOF;
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
            if((c = fgetc(fp)) == EOF) return EOF;	// cs
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

//
// input charcter stream c
// write binarized S-fmt file to fp
//
static int S_to_bin(int c, FILE *fp)
{
    static int count = 0;
    static u8 dat = 0;
    int rv = 0;

    if(c == 'S'){
        count = 0;
        rv = fputc(c, fp);  // 'S'
    }else if(isxdigit(c)){
        if(count == 0){
            rv = fputc(c, fp);  // record type char
        }else{
            if(count & 0x1){
                dat = asc2hex(c) << 4;
            }else{
                dat |= asc2hex(c);
                rv = fputc(dat, fp);
            }
        }
        count++;
    }
    return rv;
}

// cfn : save file name
// tok : file name request to term
// sbin: 1: binarize S-fmt file

static void save_file(FILE *fp, char *cfn, char *tok, int sbin)
{
    int i, c, bytes;

    fp = fopen(cfn, "w");
    if(!fp){
        printf(" '%s' can't open.\n", cfn);
    }else{
        int j, csum;
        u8 sbuf[1024];
        printf("load: %s\n", cfn);

        printf("\033<%s\n", tok);
        bytes = 0;
        i = 0;
        csum = 0;
        while((c = uart_rx()) != EOT){
            if(c == ETB){
                for(j = 0; j < i; j++){
                    if(sbin) S_to_bin(sbuf[j], fp);	// S-fmt binarize
                    else     fputc(sbuf[j], fp);	//
                }
                i = 0;
                putchar(XON);
            }else{
                sbuf[i++] = c;
                csum += c;
                bytes++;
            }
        }
        fclose(fp);
        printf("\n %d chars rcv'd. cs:%d\n", bytes, csum);
    }
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
    int i, c, bytes;
    char *tok;
    static char prmpt[30], cfn[80];
    char *argstr;
    int argc;
    static char *argv[MAXARG];
    int (*usrfunc)(int argc, char *argv[]);
    FILE *fp;

    uart_set_baudrate(0);

    init_timer(1000);   // 1000 ms 1kHz
    add_timer_irqh_sys(timer_1ms);

    set_port(1);
    printf("rvmon loaded.\n");
//    printf(" stack:%x\n", get_sp());
    mount();	// mount SD card
//    strcpy(cwd, "");
    reset_port(1);

    while(1){
        add_timer_irqh_sys(timer_1ms);
        sprintf(prmpt, "%4.1f' %4.2fV rvmon$ ", (get_temp()), (get_vbatt()));	//m8 XADC
        if(!date_valid){
            puts("\033D\n");
        }
//        getstr(argstr);	// fgets(str, 199, stdin);
        argstr = readline(prmpt);
        putchar('\n');
        argc = get_args(argstr, argv);
        tok = argv[0];
        if(!strcmp("load", argv[0])){	// Download (ascii)file and save it to MMC
            if(argc > 2){
                sprintf(cfn, "/%s/%s", argv[2], argv[1]);
                save_file(fp, cfn, argv[1], 0);
            }else if(argc > 1){
                sprintf(cfn, "/bin/%s", argv[1]);
                save_file(fp, cfn, argv[1], 1); // load Sbin
            }
        }else if(!strcmp("mld", argv[0])){	// multi command Download
            for(i = 1; i < argc; i++){
                tok = argv[i];
                sprintf(cfn, "/bin/%s", tok);
                printf(" load '%s' ", cfn);
                save_file(fp, cfn, tok, 1);
            }
        }else if(!strcmp("help", argv[0])){
            puts(//	"    ll  {fn}\n"
            //	"    mkdir {dir}\n"
            //	"    cat {fn}\n"
            //	"    uart {1/0}\n"
            //	"    cd  {fn}\n"
            //	"    pwd\n"
            //	"    cp  {src} {dst}\n"
            //    "    fd  {0/1} : dbg dump\n"
            //    "    d  {addr} : dump\n"
                "    load  {cmd(.mot)} ({dir})\n"
                "    mld   {cmd1(.mot)} {cmd2 ...}\n"
                "    {cmd}\n"
                "    go\n"
            );
#if 0
        }else if(!strcmp("cat", argv[0])){
            for(i = 1; i < argc; i++){
                tok = argv[i];
                fp = fopen(tok, "r");
                if(!fp){
                    printf(" '%s' can't open.\n", tok);
                }else{
                    while((c = fgetc(fp)) != EOF){
                        putchar(c);
                    }
                    fclose(fp);
                }
            }
#endif
        }else if(!strcmp("date", argv[0])){
            date(argc, argv);
        }else if(!strcmp("go", argv[0])){
            usrfunc = (void (*)) UBASE;
            printf("run user func : %4x\n", (int)usrfunc);
            i_cache_clean();
            c = (* usrfunc)(argc, argv) ;
            printf("%d\n", c);
        }else{
            sprintf(cfn, "/bin/%s", argv[0]);
            fp = fopen(cfn, "r");
            if(!fp){
                puts("???\n");
            }else{
                set_port(1);
                bytes = load_sbin(fp, 1);
                fclose(fp);
                reset_port(1);
                d_cache_flush();
                if(bytes > 0){
                    usrfunc = (void (*)) UBASE;
                    i_cache_clean();
                    c = (* usrfunc)(argc, argv);
                }else{
                    printf(" NG %d\n", bytes);
                }
            }
        }
        remove_timer_irqh();
        remove_user_irqh();
    }
}


