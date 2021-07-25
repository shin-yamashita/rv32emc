//
// cp.c
// 2013/11/08

#include "stdio.h"
#include <string.h>
#include "ulib.h"

//int __START run_main(int argc, char *argv[])
int main(int argc, char *argv[])
{
	int c;
	FILE *fpo = NULL;
	FILE *fp = NULL;

	if(argc < 3){
		printf(" cp no file operand.\n");
		return 0;
	}
	fp = fopen(argv[1], "r");
	if(!fp){
		printf(" '%s' can't open for read.\n", argv[1]);
		return 0;
	}
	fpo = fopen(argv[2], "w");
	if(!fpo){
		printf(" '%s' can't open for write.\n", argv[2]);
		return 0;
	}
	while((c = fgetc(fp)) != EOF){
		fputc(c, fpo);
	}	
	fclose(fp);
	fclose(fpo);

	return 0;
}

