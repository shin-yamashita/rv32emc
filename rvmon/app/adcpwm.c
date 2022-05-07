
#include <stdio.h>
#include <math.h>

#include "ulib.h"

void led_scan()
{
    static float t = 0.0f;
    static int cnt = 0;
    cnt = (cnt + 1) % 10;
    if(cnt == 0){
        for(int i = 0; i < 12; i++){
            float x = (12 + i) / 12.0f; 
            PWM[i] = (sinf(t * x) + 1.0f) * 127;
        }
        t = t + 0.0628f;
    }
}

void led_clear()
{
    for(int i = 0; i < 12; i++)
        PWM[i] = 0;
}

s16 sext16(u16 x){
    return (s16)(x << 4) >> 4;
}

void thermo(float x, float sc)
{
    int i, xi;
    xi = x * sc * 10 + 0.5f;
    for(i = 0; i <= xi; i++){
        if(i % 10 == 0) putchar('|');
        else putchar('*');
    }
    printf("\033[K\n");

}

int main()
{
    // Arty-A7 board RGB LED drive 
    float Tpwm = 16e-3f;    // LED drive pulse period (s)  , 512 count / period
    int prescale = Tpwm * f_clk / 512;  // prescaler count
    *PWMPS = prescale;

    add_timer_irqh(led_scan);

    printf("\n'q' for quit.\n");
    while(1){
        set_timer(0);
        // Arty-A7 board Voltage/Currect sensor 
        float temp   = XADC[0] * (503.975f / 4096) - 273.15f; // device temperature (degree)
        float vs5v0  = XADC[1] * (5.99f / 4096); // 5V supply (V)
        float vsvu   = XADC[2] * (16.0f / 4096); // VU supply (V)
        float vaux4  = XADC[3] * (3.32f / 4096); // CK_A4
        float vaux5  = XADC[4] * (3.32f / 4096); // CK_A5   
        float is5vp  = XADC[5] * (4.0f / 4096); // 5V supply curr(A)
        float is0p95 = XADC[6] * (2.0f / 4096); // 0.95 supply curr(A)

        printf("temp: %4.1f deg", fu(temp)); thermo(temp,   0.1f);
        printf("V5v : %5.2f V ", fu(vs5v0)); thermo(vs5v0,  1.0f);
        printf("VU  : %5.2f V ", fu(vsvu));  thermo(vsvu,   0.5f);
        printf("Va4 : %5.3f V ", fu(vaux4)); thermo(vaux4,  1.0f);
        printf("Va5 : %5.3f V ", fu(vaux5)); thermo(vaux5,  1.0f);
        printf("IU  : %5.3f A ", fu(is5vp)); thermo(is5vp,  10.0f);
        printf("Iint: %5.3f A ", fu(is0p95));thermo(is0p95, 10.0f);

        if(uart_rx_ready()){
            char c = getchar();
            if(c == 'q') break;
        }
        printf("\033[7A");  // cursor up 7 lines
        while(get_timer() < 750) ;    // wait 750 ms 
    }
    remove_timer_irqh();
    led_clear();
}

//
