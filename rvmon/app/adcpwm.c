//
// Arty-A7 board RGB LED drive and XADC test
// 

#include <stdio.h>
#include <math.h>

#include "time.h"   // clock()
#include "ulib.h"   // PWM XADC ...

// Color LED control

static float ampl = 1.0f;   // LED brightness  0.0 ~ 1.0
static float rate = 1.0f;   // Color change rate  0.0 ~ 1.0

#define limit(x,min,max)    ((x)<(min)?(min):((x)>(max)?(max):(x)))

static void ampl_cb(int up)
{
    ampl += up ? 0.1f : -0.1f;
    ampl = limit(ampl, 0.0f, 1.0f);
}
static void rate_cb(int up)
{
    rate += up ? 0.1f : -0.1f;
    rate = limit(rate, 0.0f, 1.0f);
}

// PWM[3*i] PWM[3*i+1] PWM[3*i+2]  i = 0..3
// led[i]_b led[i]_g   led[i]_r

static void led_scan()
{
    static float t = 0.0f;
    static int cnt = 0;
    cnt = (cnt + 1) % 10;
    if(cnt == 0){   // 10 ms update 
        for(int i = 0; i < 12; i++){
            float x = (12 + i) / 12.0f; 
            PWM[i] = (sinf(t * x) + 1.0f) * 127 * ampl;
            // PWM[i] : 0 ~ 255 => duty 0% ~ 50% 
            // 
        }
        t = t + 0.0628f * rate;
    }
}

static void led_clear()
{
    for(int i = 0; i < 12; i++)
        PWM[i] = 0;
}

// Button switch handler 
// btn0: ampl down 
// btn1: ampl up 
// btn2: rate down 
// btn3: rate up
static int sw[4] = {0};
static int swcnt[4] = {0};
#define SWULIM  80  // 80 ms hysteresis
static void (*cb_func[4])(int)= {
    ampl_cb, ampl_cb, rate_cb, rate_cb
};

static void debounce()  // btn debounce
{
    for(int i = 0; i < 4; i++){
        swcnt[i] += (get_port() & (0x10 << i)) ? 1 : -1;   // pin port [4:7] : btn[0:3]
        if(swcnt[i] < 0){
            swcnt[i] = 0;
            sw[i] = 0;
        }else if(swcnt[i] > SWULIM){
            swcnt[i] = SWULIM;
            if(sw[i] == 0){ // sw[i] : 0 -> 1 edge detect
                //__asm__("nop"); // jalr d_stall bug --> 2022/05/09 "fix: jalr bug (ra unnecessary increment when d_stall)"
                (cb_func[i])(i%2);// jalr reg fwd bug??  lw a2,0(sp); jalr a2;  
            }
            sw[i] = 1;
        }
    }
}

static void timer_1ms() // 1ms timer handler
{
    led_scan();
    debounce();
}

static void thermo(float x, float scale)  // thermometer display
{
    int i, xi;
    xi = x * scale * 10 + 0.5f;
    for(i = 0; i <= xi; i++){
        if(i % 10 == 0) putchar('|');
        else putchar('*');
    }
    printf("\033[K\n"); // erase EOL

}

int main()
{
    float Tpwm = 16e-3f;    // LED drive pulse period (s)  , 512 count / period
    int prescale = Tpwm * f_clk / 512;  // prescaler count
    *PWMPS = prescale;      // set PWM pulse rate (rv_pwm)

    u64 tstart = clock();
    add_timer_irqh(timer_1ms);  // register 1ms timer handler

    printf("\n'q' for quit.\n");
    while(1){
        set_timer(0);
        printf(" LED amp: %3.1f  rate:%3.1f  sw:%02x \033[K\n", fu(ampl), fu(rate), get_port());
        // Arty-A7 board Voltage/Currect sensor (https://digilent.com/reference/programmable-logic/arty-a7/reference-manual)
        float temp   = XADC[0] * (503.975f / 4096) - 273.15f; // device temperature (degree)  (Xilinx ug480)
        float vs5v0  = XADC[1] * (5.99f / 4096); // VCC5V0 voltage (V)     0~6V
        float vsvu   = XADC[2] * (16.0f / 4096); // VU voltage (V)         0~16V
        float vaux4  = XADC[3] * (3.32f / 4096); // CK_A4 port voltage (V) 0~3.3V
        float vaux5  = XADC[4] * (3.32f / 4096); // CK_A5   
        float is5vp  = XADC[5] * (4.0f / 4096);  // 5V supply curr(A)      0~4A  5m ohm gain 50 
        float is0p95 = XADC[6] * (2.0f / 4096);  // VCCINT supply curr(A)  0~2A  10m ohm gain 50

        printf("temp: %4.1f deg", fu(temp)); thermo(temp,   1/10.0f);   // 10 char / 10 deg
        printf("V5v : %5.2f V ", fu(vs5v0)); thermo(vs5v0,  1.0f);      // 10 char / 1.0 V
        printf("VU  : %5.2f V ", fu(vsvu));  thermo(vsvu,   1/2.0f);    // 10 char / 2.0 V
        printf("Va4 : %5.3f V ", fu(vaux4)); thermo(vaux4,  1.0f);      // 10 char / 1.0 V
        printf("Va5 : %5.3f V ", fu(vaux5)); thermo(vaux5,  1.0f);
        printf("IU  : %5.3f A ", fu(is5vp)); thermo(is5vp,  1/0.1f);    // 10 char / 0.1 A
        printf("Iint: %5.3f A ", fu(is0p95));thermo(is0p95, 1/0.1f);
                                //注 fu()
        if(uart_rx_ready()){
            char c = getchar();
            if(c == 'q') break;
        }
        printf("\033[8A");  // cursor up 8 lines
        while(get_timer() < 1000) ;    // wait 1000 ms 
    }
    remove_timer_irqh();
    led_clear();
    
    int elapsed = (int)((clock() - tstart) * 1e3f / f_clk); // elapsed(ms)
    return elapsed;
}

//注） lib/fprint.c の簡易 printf() は、float 引数を u32 として渡す仕様にしているので、float 引数を macro fu() で型変換している。
//     fu() macro は include/types.h で定義。  
//     printf() への引数は可変長引数なので、コンパイラが float を double に暗黙に変換してしまう。(C言語の仕様)


