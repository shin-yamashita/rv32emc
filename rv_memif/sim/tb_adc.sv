
`timescale 1ns/1ns
`include "rv_types.svh"

module tb_adc();
    logic clk = 0;
    logic xreset;

    // bus
    u5_t  adr;
    logic cs;
    logic rdy = 1;
    u4_t  we = 0;
    logic re = 0;
    u32_t dw = 0;
    u32_t dr;

    task reg_rd(input int a, output int data);
        //   @(posedge clk);
        #1
        we  = 0;
        re  = 1;
        adr = a;
        do
        @(posedge clk);
        while(!rdy);
        #1
        re  = 0;
        data = dr;
        //    $display("r a: %h d:%d", adr, data);
        @(posedge clk);
    endtask

    always #8       // 60MHz
        clk <= !clk;

    int data;
    initial begin
        xreset = 0;
        # 40
        @(posedge clk);
        #1
        xreset = 1;
        #200us
        repeat(50) begin
            repeat(10) @(posedge clk);
            for(int i = 0; i < 7; i = i + 1)begin
                reg_rd(i, data);
                repeat(3) @(posedge clk);
            end
        end
    end
    assign cs = re;

    // XADC clock
    logic adclk = 0;     // XADC dclk 100MHz
    // MIG 
    logic ui_clk = 0;    // MIG ui_clk
    u12_t device_temp;  // temperature data for MIG

    always #5       // 100MHz
        adclk <= !adclk;

    always #6       // 83.3M
        ui_clk <= !ui_clk;

    rv_adcif u_rv_adcif(
        .clk    (clk),   // cpu bus clock
        .xreset (xreset),
        // bus
        .adr    (adr),
        .cs     (cs),
        .rdy    (rdy),
        .we     (we),
        .re     (re),
        .dw     (dw),
        .dr     (dr),
        // XADC clock
        .adclk  (adclk),     // XADC dclk 100MHz
        .vauxp1 (vauxp1),
        .vauxn1 (vauxn1),
        .vauxp2 (vauxp2),
        .vauxn2 (vauxn2),
        .vauxp4 (vauxp4),
        .vauxn4 (vauxn4),
        .vauxp5 (vauxp5),
        .vauxn5 (vauxn5),
        .vauxp9 (vauxp9),
        .vauxn9 (vauxn9),
        .vauxp10(vauxp10),
        .vauxn10(vauxn10),
        // MIG 
        .ui_clk (ui_clk),    // MIG ui_clk
        .device_temp(device_temp)  // temperature data for MIG
        );


endmodule

