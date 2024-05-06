
#include "ulib.h"
#include "stdio.h"

int main(int argc, char *argv[]);
extern u32 _bss_start, _end;

int __attribute__ ((section (".init"))) crt0(int argc, char *argv[])
{
    zero_bss();
    printf("_bss:%x _end:%x sp:%x gp:%x f_clk:%4.1fMHz\n", &_bss_start, &_end, get_sp(), get_gp(), fu(f_clk*1e-6f));

    int rv = main(argc, argv);
    
    remove_timer_irqh();
    //int rv = main(argc, argv);
    remove_user_irqh();
    remove_user_irqh_1();
    remove_user_irqh_2();

    return rv;
}
