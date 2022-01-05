
#include "stdio.h"
#include "ulib.h"

void timer_1ms()
{
  static int timer = 0;

  timer++;
  timer %= 1000;
  switch(timer / 100){
  case 0:
  case 2:
    set_port(0);
    break;
  default:
    reset_port(0);
    break;
  }
}

void init()
{
  add_timer_irqh(timer_1ms);
  init_timer(1000);   // 1kHz 1ms
  uart_set_baudrate(0);

  while(1){
    int c;
    c = getchar();

    if(c == 'g') main();
  }
}

