
#include "ulib.h"
#include "stdio.h"

int main(int argc, char *argv[]);
extern u32 _bss_start, _end;

int __attribute__ ((section (".init"))) crt0(int argc, char *argv[])
{
    zero_bss();
//    remove_timer_irqh_sys();
//    int i;
//    for(i = 1; i < argc; i++)
//        printf(" %s", argv[i]);
    printf("_bss:%x _end:%x sp:%x gp:%x\n", &_bss_start, &_end, get_sp(), get_gp());

    main(argc, argv);

    return 0;
}
