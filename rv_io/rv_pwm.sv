
// rv_pwm
// Arty-A7 PWM for RGB LED
// 2022/05/05
// 

`timescale 1ns/1ns
`include "rv_types.svh"

module rv_pwm(
    input  logic clk,   // cpu bus clock
    input  logic xreset,
// bus
    input  u5_t  adr,
    input  logic cs,
    input  logic rdy,
    input  u4_t  we,
    input  logic re,
    input  u32_t dw,
    output u32_t dr,

    output u12_t led
    );

// 16ms 512 counter
u12_t prescaler;
u12_t precnt;
logic [8:0] counter;

always_ff@(posedge clk) begin   
    if(!xreset) begin
        precnt  <= 'd0;
        counter <= 'd0;
    end else begin
        if(precnt < prescaler) begin
            precnt <= precnt + 'd1;
        end else begin
            precnt  <= 'd0;
            counter <= counter + 'd1;
        end
    end
end

// pwm control registers
u8_t pwm[0:11];

always_ff@(posedge clk) begin
    if(!xreset) begin
        prescaler <= 'd1875;    // 32kHz @ 60MHz clk
        for(int i = 0; i < 12; i = i + 1)
            pwm[i] <= '0;
    end else if(rdy) begin
        if(cs && re) begin
            if(adr[4:2] < 3) begin
                int i = adr[4:2] * 4;
                dr <= {pwm[i],pwm[i+1],pwm[i+2],pwm[i+3]};
            end else if(adr[4:2] == 'd4) begin
                dr <= prescaler;
            end else begin
                dr <= 'd0;
            end
        end else begin
            dr <= 'd0;
        end
        if(cs && we != 4'b0000) begin
            if(adr[4:2] < 3) begin
                int i = adr[4:2] * 4;
                {pwm[i],pwm[i+1],pwm[i+2],pwm[i+3]} 
                    <= {we[0]?dw[7:0]:pwm[i], 
                        we[1]?dw[15:8]:pwm[i+1], 
                        we[2]?dw[23:16]:pwm[i+2], 
                        we[3]?dw[31:24]:pwm[i+3]};
            end else if(adr[4:2] == 'd4) begin
                prescaler <= u12_t'(dw);
            end
        end
    end
end

// pwm pulse genaration
always_ff@(posedge clk) begin
    for(int i = 0; i < 12; i++) begin
        led[i] <= pwm[i] > counter;
    end
end

endmodule
