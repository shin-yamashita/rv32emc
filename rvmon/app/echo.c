
//
// echo.c
//

#include "stdio.h"
#include <string.h>
#include "ulib.h"


int main(int argc, char *argv[])
{
    int i, j, c, nc;
    char str[1024];
    FILE *fpo = NULL;

    strcpy(str, "");
    for(i = 1; i < argc; i++){
        if(!strcmp(argv[i], ">")){
            if(++i >= argc) break;
            fpo = fopen(argv[i], "w");
            if(!fpo)
                printf(" '%s' can't open for write.\n", argv[i]);
        }else if(!strcmp(argv[i], ">>")){
            if(++i >= argc) break;
            fpo = fopen(argv[i], "a");
            if(!fpo)
                printf(" '%s' can't open for append.\n", argv[i]);
        }else{
            strcat(str, argv[i]);
        }
    }

    if(fpo){
        fprintf(fpo, "%s\n", str);
        fclose(fpo);
    }else{
        printf("%s\n", str);
    }

    return 0;
}

