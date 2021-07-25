
// dump.c
//

#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ulib.h"
#include "save_data.h"

unsigned int dump(unsigned int adr, unsigned char *pt, FILE *fp, int bwl)
{
    int i, j;

    for(i = 0; i < 256; i+=16){
        fprintf(fp, "%08x: ", (int)pt);

        for(j = 0; j < 16; j+=bwl){
            switch(bwl){
            case 1: fprintf(fp, "%02x ", pt[j]);    break;
            case 2: fprintf(fp, "%04x ", *(u16*)(&pt[j]));    break;
            case 4: fprintf(fp, "%08x ", *(u32*)(&pt[j]));    break;
            }
        }
        for(j = 0; j < 16; j++){
            char c = pt[j];
            fputc(c >= 0x20 && c < 0x7f ? c : '.', fp);
            //                      fputc(isgraph(c) ? c : '.', fp);
        }
        fputc('\n', fp);
        pt += 16;
        adr+=16;
    }
    fputc('\n', fp);
    return adr;
}

int main(int argc, char *argv[])
{
    int i, nblk = 1, adrset = 0;
    u32 adr;
    int bwl = 1;
    //	save_data = (save_data_t*)0x5700;
    FILE *fp = NULL;
    bwl = SAVE_DATA->bwl;

    for(i = 1; i < argc; i++){
        if(!strcmp(argv[i], "-o")){
            if(++i >= argc) break;
            fp = fopen(argv[i], "w");
            if(!fp) printf(" '%s' can't open for write.\n", argv[i]);
        }else if(!strcmp(argv[i], "-h")){
            printf("*** d <hex addr> <-b/-w/-l> <-o out_file_name>\n");
            return 0;
        }else if(!strcmp(argv[i], "-b")){
            bwl = 1;
        }else if(!strcmp(argv[i], "-w")){
            bwl = 2;
        }else if(!strcmp(argv[i], "-l")){
            bwl = 4;
        }else if(!adrset){
            SAVE_DATA->dadr = str2u32(argv[i]);
            adrset = 1;
        }else if(isdigit((u8)*argv[i])){
            nblk = atoi(argv[i]);
        }
    }
    bwl = bwl < 1 ? 1 : (bwl > 4 ? 4 : bwl);
    adr = SAVE_DATA->dadr;
    if(fp){
        for(i = 0; i < nblk; i++)
            adr = dump(adr, (unsigned char *)adr, fp, bwl);
        fclose(fp);
    }else{
        for(i = 0; i < nblk; i++)
            adr = dump(adr, (unsigned char *)adr, stdout, bwl);
    }
    SAVE_DATA->dadr = adr;
    SAVE_DATA->bwl = bwl;

    return 0;
}

