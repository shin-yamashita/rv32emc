
//#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "bfd.h"
#include "monlib.h"
#include "simcore.h"
#include "consio.h"

 bfd_byte         *memory;
 bfd_byte         *stack;
 bfd_size_type    memsize = 32768;
 bfd_size_type    stacksize = 0;
 bfd_vma          vaddr = 0;
 bfd_vma          statck_top;

 int view_reg[];
 int nview;
 int sys_exit;

 int n_break;
 u32 break_adr[];
 int     break_en[];
 FILE *ofp;

 int debug = 0;

int main (int argc, char **argv)
{
    int i;
    char cmd[256], c, *s, *scr = NULL, *lfn = NULL;
    FILE *ifp = fopen("sr_dbg.out", "r");
    FILE *pfp = fopen("prog.mem", "r");
    char str[131];
    int adr, nxtadr, inc;

    memory = malloc(40000);

    for (i = 1; i < argc; i++) {
	c = *argv[i];
        if (!strcmp(argv[i], "-i")) {
            if (++i >= argc)
                break;
            scr = argv[i];
        } else if (!strcmp(argv[i], "-debug")) {
            debug = 1;
        } else if (isalnum(c) || c == '.' || c == '/') {
	    lfn = argv[i];
	}
    }

    while(fgets(str, 130, pfp) != NULL){
	if(*str == '@'){
		s = strtok(&str[1], " \n");
		i = strtol(s, NULL, 16);
		while((s = strtok(NULL, " \n")) != NULL){
			memory[i++] = strtol(s, NULL, 16);
		}
	}
    }

    inc = 0;
    nxtadr = 0x400;
    while(fgets(str, 130, ifp) != NULL){
	char istr[80], opstr[80], oprstr[80];

	str[18] = 0;
	adr = strtol(&str[10], NULL, 16);
	printf("%s:", str);
	if(adr > 0){
//	  if(nxtadr == adr){
		inc = disasm(adr, istr, opstr, oprstr);	
		nxtadr = adr + inc;
		printf("%02x%02x %-6s %-16s", memory[adr], memory[adr+1], opstr, oprstr);
//		printf(" %x  +%d ", adr, inc);
//	  }else{
//		printf("%02x%02x                 ", memory[adr], memory[adr+1]);
//	  }
	}
	printf("%s", &str[26]);	
    }
    
    return 0;
}



