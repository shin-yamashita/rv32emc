
//
// sr_cache_unit.vhd
//  d-chache i-cache

`timescale 1ns/1ns
`include "rv_types.svh"

module rv_cache_unit (
// bus
    input  logic cclk,   //	: in  std_logic;
    input  logic xreset, //	: in  std_logic;
    input  u32_t adr,    //	: in  unsigned(31:0);
    input  logic cs, //	: in  std_logic;  cs_cache 
    input  u4_t we,  //	: in  std_logic_vector(3:0);
    input  logic re, //	: in  std_logic;
    output logic rdy,   //	: out std_logic;
    input  u32_t dw, //	: in  unsigned(31:0);
    output u32_t dr,    //	: out unsigned(31:0);

    input  u32_t i_adr,  //	: in  unsigned(31:0);
    input  logic i_re,   //	: in  std_logic;
    output u32_t i_dr,  //	: out unsigned(31:0);

    input  logic cal_cmpl,   //: in  std_logic;

// axi i/f
    input  logic aclk,   //	: in  std_logic;
    input  logic arst_n, //	: in	std_logic;

    output logic awid,  //	: out	std_logic;
    output u28_t awaddr, //	: out	std_logic_vector(27:0);
    output u8_t awlen,  //	: out	std_logic_vector(7:0);
    output logic awvalid,   //	: out	std_logic;
    input  logic awready,    //	: in	std_logic;

    output u32_t wdata, //	: out	std_logic_vector(31:0);
    output u4_t wstrb,  //	: out	std_logic_vector(3:0);
    output logic wlast, //	: out	std_logic;
    output logic wvalid,    //	: out	std_logic;
    input  logic wready, //	: in	std_logic;

    input  logic bid,    //	: in	std_logic;
    input  u2_t bresp,   //	: in	std_logic_vector(1:0);
    input  logic bvalid, //	: in	std_logic;
    output logic bready, //	: out	std_logic;

    output logic arid,  //	: out	std_logic;
    output u28_t araddr,    //	: out	std_logic_vector(27:0);
    output u8_t arlen,  //	: out	std_logic_vector(7:0);
    output logic arvalid,   //	: out	std_logic;
    input  logic arready,    //	: in	std_logic;

    input  logic rid,    //	: in	std_logic;
    input  u32_t rdata,  //	: in	std_logic_vector(31:0);
    input  u2_t rresp,   //	: in	std_logic_vector(1:0);
    input  logic rlast,  //	: in	std_logic;
    input  logic rvalid, //	: in	std_logic;
    output logic rready, //	: out	std_logic;

// debug
    output u4_t fp  //	: out unsigned(3:0)
    );

logic i_rid;    //	: std_logic;	// axi_ic can't handle rid
u28_t araddr0, araddr1; //	: std_logic_vector(26:0);
u8_t  arlen0, arlen1;   //	: std_logic_vector(7:0);
logic arvalid0, arvalid1;   //	: std_logic;
logic arready0, arready1;   //	: std_logic;
logic rvalid0, rvalid1; //	: std_logic;
logic rlast0, rlast1;   //	: std_logic;
logic rready0, rready1;    //	: std_logic;

logic clreqi, clbsyi, clreq, clbsy, flreq, flbsy;   //   : std_logic;
logic re1;  //	: std_logic;
u32_t d_dr; //	: unsigned(31:0);
logic d_rdy, i_rdy, rdy_i;  //	: std_logic;
u4_t sfp;   //	: std_logic_vector(3:0);
logic cal_cmpl_s;

//fp(0) <= c3_calib_done;
//fp(1) <= vd;
 assign awid = '1;
 assign bready = '1;
 assign wstrb = 4'b1111;

rv_cache #(.base(8'h20)) u_rv_cache_d (
    .xrst    (xreset),

// chahe clear / flush request / busy status
    .clreq   (clreq),
    .clbsy   (clbsy),
    .flreq   (flreq),
    .flbsy   (flbsy),

// CPU bus
    .cclk    (cclk),
    .adr     (adr),
    .we      (we),
    .re      (re),
    .rdyin   (rdy_i),
    .rdy     (d_rdy),
    .dw      (dw),
    .dr      (d_dr),

// memc interface
    .aclk	(aclk),
    .arst_n	(arst_n),
    .awaddr	(awaddr),	//          : out   std_logic_vector(27:0);
    .awlen 	(awlen),	//          : out   std_logic_vector(7:0);
    .awvalid	(awvalid),	//         : out   std_logic;
    .awready	(awready),	//         : in    std_logic;

    .wr_data	(wdata),	//         : out   std_logic_vector(31:0);
    .wvalid 	(wvalid),	//         : out   std_logic;      // wr_en
    .wlast  	(wlast),	//         : out   std_logic;
    .wready 	(wready),	//         : in    std_logic;      // wr_full

    .araddr 	(araddr0),	//         : out   std_logic_vector(27:0);
    .arlen  	(arlen0),	//         : out   std_logic_vector(7:0);
    .arvalid	(arvalid0),	//         : out   std_logic;
    .arready	(arready0),	//         : in    std_logic;

    .rd_data	(rdata),	//         : in    std_logic_vector(31:0);
    .rvalid 	(rvalid0),	//         : in    std_logic;      // rd_en
    .rlast  	(rlast0),	//         : in    std_logic;      // rd_en
    .rready 	(rready0)	//         : out   std_logic       //
    );

// process
// begin
// wait until aclk'event and aclk = '1';
 always_ff@(posedge aclk) begin
  if(!arst_n) begin
    i_rid <= '0;
  end else if(arvalid0 & arready) begin
    i_rid <= '0;
  end else if(arvalid1 & arready) begin
    i_rid <= '1;
  end
 end

 assign arvalid = arvalid0 | arvalid1;
 assign arready0 = arready;
 assign arready1 = arready & !arvalid0;
// arid <= not arvalid0;
 assign arid = '0;
 assign araddr = arvalid0 ? araddr0 : araddr1;
 assign arlen = arvalid0 ? arlen0 : arlen1;
// arlen <= arlen0 when arvalid0 = '1' else arlen1;
//--------
 assign rvalid0 = rvalid && !i_rid;
 assign rlast0  = rlast && !i_rid;
 assign rvalid1 = rvalid && i_rid;
 assign rlast1  = rlast && i_rid;
 assign rready  = !i_rid ? rready0 : rready1;

rv_cache #(.base(8'h20)) u_rv_cache_i (
        .xrst    (xreset),

// chahe clear / flush request / busy status
        .clreq   (clreqi),
        .clbsy   (clbsyi),
        .flreq   ('0),
        .flbsy   (),

// CPU bus
        .cclk    (cclk),
        .adr     (i_adr),
        .we      (4'b0000),
        .re      (i_re),
        .rdyin   (rdy_i),
        .rdy     (i_rdy),
        .dw      (32'd0),
        .dr      (i_dr),

// memc interface
        .aclk	(aclk),
        .arst_n	(arst_n),
        .awaddr	(),	//          : out   std_logic_vector(29:0);
        .awlen 	(),	//          : out   std_logic_vector(7:0);
        .awvalid	(),	//         : out   std_logic;
        .awready	('1),	//         : in    std_logic;

        .wr_data	(),	//         : out   std_logic_vector(31:0);
        .wvalid 	(),	//         : out   std_logic;      // wr_en
        .wlast  	(),	//         : out   std_logic;
        .wready 	('1),	//         : in    std_logic;      // wr_full

        .araddr 	(araddr1),	//         : out   std_logic_vector(29:0);
        .arlen  	(arlen1),	//         : out   std_logic_vector(7:0);
        .arvalid	(arvalid1),	//         : out   std_logic;
        .arready	(arready1),	//         : in    std_logic;

        .rd_data	(rdata),	//         : in    std_logic_vector(31:0);
        .rvalid 	(rvalid1),	//         : in    std_logic;      // rd_en
        .rlast  	(rlast1),	//         : in    std_logic;      // rd_en
        .rready 	(rready1)	//         : out   std_logic       //
    );

  // control registers
//  process(cs, we, adr, dw)
  always_comb 
  begin
    if(cs && we[0] && adr[4:2] == '0) begin
        flreq  <= dw[1];
        clreq  <= dw[0];
        clreqi <= dw[2];
    end else begin
        flreq  <= '0;
        clreq  <= '0;
        clreqi <= '0;
    end
  end

//  process
//  begin
//  wait until cclk'event and cclk = '1';
  always_ff@(posedge cclk) begin
    re1 <= cs & re;
    cal_cmpl_s <= cal_cmpl;
  end

  assign dr = re1 ? {28'd0, cal_cmpl_s, clbsyi, flbsy, clbsy} : d_dr;

  assign rdy_i = i_rdy & d_rdy;
  assign rdy = rdy_i;

endmodule






