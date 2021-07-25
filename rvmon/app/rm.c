
//
//	ll.c
//	2013/06/03 fatfs version

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

int main(int argc, char *argv[])
{
    int i, c, err;
    FILE *fp;

    for(i = 1; i < argc; i++){
        Tolower(argv[i]);
        fp = fopen(argv[i], "r");
        if(fp){
            fclose(fp);
            printf(" remove '%s' ? (y/N) ", argv[i]);
            putchar('\n');	// flush
            c = getchar();
            if(tolower(c) == 'y'){
                err = f_unlink(argv[i]);
                if(err){
                    printf(" can't remove.\n");
                } else {
                    printf(" done.\n");
                }
            } else {
                printf("\n");
            }
        } else {
            printf(" file '%s' doesn't exists.\n", argv[i]);
        }
    }
    return 0;
}

