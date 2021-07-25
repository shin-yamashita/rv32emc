
//
//	ll.c
//	2013/06/03 fatfs version

#include "stdio.h"
#include <string.h>
#include <ctype.h>
#include "ulib.h"
#include "time.h"
#include "ff.h"

void fpath(char *path, char *cwd, char *fn){
    sprintf(path, "%s/%s", cwd, fn);
    return;
}

time_t cnvtime(u16 fdate, u16 ftime)
{
    time_t lt;
    lt.year  = fdate >> 9;
    lt.month = fdate >> 5;
    lt.day   = fdate;
    lt.hour  = ftime >> 11;
    lt.min   = ftime >> 5;
    lt.sec   = ftime;
    return lt;
}

void Tolower(char *str)
{
    char c;

    while((c = *str)){
        *str++ = tolower(c);
    }
    *str = 0;
}

//int __START run_main(int argc, char *argv[])
int main(int argc, char *argv[])
{
    int fd = -1, err;
    FILINFO finfo;
    char str[143], cfn[130];
    time_t ltime;
    DIR dent;

    if(argc > 1)
        strcpy(cfn, argv[1]);
    else
        strcpy(cfn, "");

    fd = f_opendir(&dent, cfn);
    if(fd){
        printf(" fat can't open.\n");
    }else{
        for(;;){
            err = f_readdir(&dent, &finfo);
            if(err || !finfo.fname[0]) break; /* Error or end of dir */
            Tolower(finfo.fname);
            fpath(str, cfn, finfo.fname);
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

