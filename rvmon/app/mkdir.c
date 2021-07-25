
//
//	mkdir.c
//	2013/09/11 fatfs version

#include "stdio.h"
#include <string.h>
#include <ctype.h>
#include "ulib.h"
#include "ff.h"

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
    int i, err;

    for(i = 1; i < argc; i++){
        Tolower(argv[i]);
        err = f_mkdir(argv[i]);
        if(err){
            printf(" can't make directory '%s'.\n", argv[i]);
        }
    }
    return 0;
}

