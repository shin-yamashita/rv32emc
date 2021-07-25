
// cat.c
//

#include "stdio.h"
#include <string.h>
#include "ulib.h"

//int __START run_main(int argc, char *argv[])
int main(int argc, char *argv[])
{
    int i, c;
    FILE *fpo = NULL;
    FILE *fp = NULL;

    for(i = 1; i < argc; i++){
        if(!strcmp(argv[i], "-o")){
            if(++i >= argc) break;
            fpo = fopen(argv[i], "w");
            if(!fpo) printf(" '%s' can't open for write.\n", argv[i]);
        }
    }
    for(i = 1; i < argc; i++){
        if(!strcmp(argv[i], "-o")){
            if(++i >= argc) break;
        }else{
            fp = fopen(argv[i], "r");
            if(!fp){
                printf(" '%s' can't open.\n", argv[i]);
            }else{
                while((c = fgetc(fp)) != EOF){
                    if(fpo) fputc(c, fpo);
                    else putchar(c);
                }
                fclose(fp);
            }
        }
    }
    if(fpo) fclose(fpo);

    return 0;
}

