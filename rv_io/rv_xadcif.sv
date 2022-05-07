
// rv_adcif
// Arty-A7 XADC interface
// 2022/05/05
// 

`timescale 1ns/1ns
`include "rv_types.svh"

module rv_adcif(
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

// XADC clock
    input  logic adclk,     // XADC dclk 100MHz
    input vauxp1,
    input vauxn1,
    input vauxp2,
    input vauxn2,
    input vauxp4,
    input vauxn4,
    input vauxp5,
    input vauxn5,
    input vauxp9,
    input vauxn9,
    input vauxp10,
    input vauxn10,

// MIG 
    input  logic ui_clk,    // MIG ui_clk
    output u12_t device_temp  // temperature data for MIG
    );

// XADC signals
logic eoc_out, alarm_out, eos_out, busy_out;
logic den_in, dwe_in, drdy_out, dclk_in, reset_in;
u7_t  daddr_in;
u5_t  channel_out;
u16_t do_out, di_in;

logic drdy;
logic dvalid;
u12_t adcdata[0:6];

logic dvalid_cs;
u12_t adcdata_cs[0:6];
u12_t adcdata_cs2[0:6];

// async
always_ff@(posedge clk) begin
    dvalid_cs <= dvalid;
    for(int i = 0; i < 7; i++)
        adcdata_cs[i] <= adcdata[i];
    if(dvalid_cs)
        for(int i = 0; i < 7; i++)
            adcdata_cs2[i] <= adcdata_cs[i];
end

// rv_core data bus interface  
always_ff @(posedge clk) begin
    if(rdy) begin
        if(re && cs && adr[4:1] < 'd7) begin    // 16bit access
            if(adr[1])
                dr <= adcdata_cs2[adr[4:1]] << 16;
            else
                dr <= adcdata_cs2[adr[4:1]];
        end else begin
            dr <= '0;
        end
    end
end

// async
logic dvalid_s;
u12_t device_temp_s;

always_ff@(posedge ui_clk) begin
    dvalid_s <= dvalid;
    device_temp_s <= adcdata[0];
    if(dvalid_s)
        device_temp <= device_temp_s;
end

// XADC data capture, continuous mode
//
// Arty-A7 ADC analog sources
//   ch  
// 0 0x0  temperature  
// 1 0x11 aux1  vsns5v0
// 2 0x12 aux2  vsnsvu
// 3 0x14 aux4 
// 4 0x15 aux5 
// 5 0x19 aux9  isns5v0
// 6 0x1a aux10 isns0v95
//
always_ff@(posedge dclk_in) begin
    drdy <= drdy_out;
    dvalid <= {drdy,drdy_out} == '0;
    if(drdy) begin
        case(channel_out)
        5'h00 : adcdata[0] <= do_out[15:4];
        5'h11 : adcdata[1] <= do_out[15:4];
        5'h12 : adcdata[2] <= do_out[15:4];
        5'h14 : adcdata[3] <= do_out[15:4];
        5'h15 : adcdata[4] <= do_out[15:4];
        5'h19 : adcdata[5] <= do_out[15:4];
        5'h1a : adcdata[6] <= do_out[15:4];
        default : ;
        endcase
    end
end

assign den_in = eoc_out;
assign daddr_in = {2'b00, channel_out};
assign di_in = 16'd0;
assign dclk_in = adclk;
assign dwe_in = 1'b0;
assign reset_in = ~xreset;

xadcif u_xadcif (   // ../ip/xadcif/xadcif.xcix
  .di_in(di_in),              // input wire [15 : 0] di_in
  .daddr_in(daddr_in),        // input wire [6 : 0] daddr_in
  .den_in(den_in),            // input wire den_in
  .dwe_in(dwe_in),            // input wire dwe_in
  .drdy_out(drdy_out),        // output wire drdy_out
  .do_out(do_out),            // output wire [15 : 0] do_out
  .dclk_in(dclk_in),          // input wire dclk_in
  .reset_in(reset_in),        // input wire reset_in
  .vp_in(vp_in),              // input wire vp_in
  .vn_in(vn_in),              // input wire vn_in
  .vauxp1(vauxp1),            // input wire vauxp1
  .vauxn1(vauxn1),            // input wire vauxn1
  .vauxp2(vauxp2),            // input wire vauxp2
  .vauxn2(vauxn2),            // input wire vauxn2
  .vauxp4(vauxp4),            // input wire vauxp4
  .vauxn4(vauxn4),            // input wire vauxn4
  .vauxp5(vauxp5),            // input wire vauxp5
  .vauxn5(vauxn5),            // input wire vauxn5
  .vauxp9(vauxp9),            // input wire vauxp9
  .vauxn9(vauxn9),            // input wire vauxn9
  .vauxp10(vauxp10),          // input wire vauxp10
  .vauxn10(vauxn10),          // input wire vauxn10
  .channel_out(channel_out),  // output wire [4 : 0] channel_out
  .eoc_out(eoc_out),          // output wire eoc_out
  .alarm_out(alarm_out),      // output wire alarm_out
  .eos_out(eos_out),          // output wire eos_out
  .busy_out(busy_out)        // output wire busy_out
);

endmodule
