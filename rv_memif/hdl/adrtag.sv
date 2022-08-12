

`timescale 1ns/1ps

module adrtag (
  input  logic [3:0]  a,
  input  logic [19:0] d,
  input  logic [3:0]  dpra,
  input  logic        clk,
  input  logic        we,
  input  logic        xrst,
  output logic [19:0] dpo
);

// disable conflict avoidance logic
  reg [19:0] mem [15:0] /* synthesis syn_ramstyle=no_rw_check*/;

// 'write first' or transparent mode
  always @(posedge clk) begin
    if(!xrst) begin
      for(int i = 0; i < 16; i++)
        mem[i] <= d;
    end else if(we) begin
      mem[a] <= d;
    end
  end
  assign dpo = mem[dpra];

endmodule

