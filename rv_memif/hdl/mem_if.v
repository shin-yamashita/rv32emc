
//-- memif.vhd
//-- DDR3 interface
//--
`resetall
`timescale 1ps/1ps

module mem_if #(parameter ICACHE_EN = 1)(
//-- copu clk
//       input       cpclk,
//-- bus
       input         cclk,
       input         xreset,
       input [31:0]  adr,	
       input         cs,	// cs_cache
       input [3:0]   we,	
       input         re,	
       output        rdy,	
       input  [31:0] dw,	
       output [31:0] dr,
       input  [31:0] i_adr,	
       input         i_re,	
       output [31:0] i_dr,	
//-- dram
       input        	ref_clk,
       input        	sys_clk,
       input        	sys_rst,
       // Inouts
       inout [15:0]    ddr3_dq,
       inout [1:0]     ddr3_dqs_n,
       inout [1:0]     ddr3_dqs_p,
       // Outputs
       output [13:0]   ddr3_addr,
       output [2:0]    ddr3_ba,
       output          ddr3_reset_n,
       output          ddr3_ras_n,
       output          ddr3_cas_n,
       output          ddr3_we_n,
       output          ddr3_ck_p,
       output          ddr3_ck_n,
       output          ddr3_cke,
       output          ddr3_cs_n,
       output [1:0]    ddr3_dm,
       output          ddr3_odt,
//-- debug
       output         ui_clk,
       input [11:0]   tempmon,

       output [3:0]    fp
    );

  // Wire declarations
      
  wire            rst, aclk;
  wire            mmcm_locked;
  reg  [7:0]      aresetn;
  wire		  arst_n;
  wire            app_sr_active;
  wire            app_ref_ack;
  wire            app_zq_ack;
  wire            app_rd_data_valid;
  wire [64-1:0]   app_rd_data;

//***************************************************************************

wire [3 : 0] 	M00_AXI_AWID;
wire [27 : 0] 	M00_AXI_AWADDR;
wire [7 : 0] 	M00_AXI_AWLEN;
wire [2 : 0] 	M00_AXI_AWSIZE;
wire [1 : 0] 	M00_AXI_AWBURST;
wire 		M00_AXI_AWLOCK;
wire [3 : 0] 	M00_AXI_AWCACHE;
wire [2 : 0] 	M00_AXI_AWPROT;
wire [3 : 0] 	M00_AXI_AWQOS;
wire 		M00_AXI_AWVALID;
wire 		M00_AXI_AWREADY;
wire [127 : 0]	M00_AXI_WDATA;
wire [15 : 0] 	M00_AXI_WSTRB;
wire 		M00_AXI_WLAST;
wire 		M00_AXI_WVALID;
wire 		M00_AXI_WREADY;
wire [3 : 0] 	M00_AXI_BID;
wire [1 : 0] 	M00_AXI_BRESP;
wire 		M00_AXI_BVALID;
wire 		M00_AXI_BREADY;
wire [3 : 0] 	M00_AXI_ARID;
wire [27 : 0] 	M00_AXI_ARADDR;
wire [7 : 0] 	M00_AXI_ARLEN;
wire [2 : 0] 	M00_AXI_ARSIZE;
wire [1 : 0] 	M00_AXI_ARBURST;
wire 		M00_AXI_ARLOCK;
wire [3 : 0] 	M00_AXI_ARCACHE;
wire [2 : 0] 	M00_AXI_ARPROT;
wire [3 : 0] 	M00_AXI_ARQOS;
wire 		M00_AXI_ARVALID;
wire 		M00_AXI_ARREADY;
wire [3 : 0] 	M00_AXI_RID;
wire [127 : 0] 	M00_AXI_RDATA;
wire [1 : 0] 	M00_AXI_RRESP;
wire 		M00_AXI_RLAST;
wire 		M00_AXI_RVALID;
wire 		M00_AXI_RREADY;

/*
ila_0 u_ila_0 (
        .clk(cclk), // input wire clk

        .probe0(i_adr), // input wire [31:0]  probe0  
        .probe1({rdy,cs,css,ccs,we,re}) // input wire [7:0]  probe1
);
*/

// Start of User Design top instance
//***************************************************************************
// The User design is instantiated below. The memory interface ports are
// connected to the top-level and the application interface ports are
// connected to the traffic generator module. This provides a reference
// for connecting the memory controller to system.
//***************************************************************************
always @(posedge aclk or negedge sys_rst) begin
  if(sys_rst == 1'b0)  aresetn <= 8'b0;
  else begin
    aresetn <= {aresetn[6:0], mmcm_locked};
  end
end
assign arst_n = aresetn[7];
assign ui_clk = aclk;

  wire init_calib_complete;

  ddr3memc u_ddr3memc
      (
// Memory interface ports
       .ddr3_addr                      (ddr3_addr),
       .ddr3_ba                        (ddr3_ba),
       .ddr3_cas_n                     (ddr3_cas_n),
       .ddr3_ck_n                      ({ddr3_ck_n}),
       .ddr3_ck_p                      ({ddr3_ck_p}),
       .ddr3_cke                       ({ddr3_cke}),
       .ddr3_ras_n                     (ddr3_ras_n),
       .ddr3_reset_n                   (ddr3_reset_n),
       .ddr3_we_n                      (ddr3_we_n),
       .ddr3_dq                        (ddr3_dq),
       .ddr3_dqs_n                     (ddr3_dqs_n),
       .ddr3_dqs_p                     (ddr3_dqs_p),

       .init_calib_complete            (init_calib_complete),
      
       .ddr3_cs_n                      ({ddr3_cs_n}),
       .ddr3_dm                        (ddr3_dm),
       .ddr3_odt                       ({ddr3_odt}),
// Application interface ports
       .ui_clk                         (aclk),   // out
       .ui_clk_sync_rst                (rst),    // out
       .mmcm_locked                    (mmcm_locked),
       .aresetn                        (arst_n), // in
       .app_sr_req                     (1'b0),
       .app_ref_req                    (1'b0),
       .app_zq_req                     (1'b0),
       .app_sr_active                  (app_sr_active),
       .app_ref_ack                    (app_ref_ack),
       .app_zq_ack                     (app_zq_ack),

// Slave Interface Write Address Ports
       .s_axi_awid                     (M00_AXI_AWID),		// 4
       .s_axi_awaddr                   (M00_AXI_AWADDR),	// 28 (32)
       .s_axi_awlen                    (M00_AXI_AWLEN),		// 8
       .s_axi_awsize                   (M00_AXI_AWSIZE),	// 3
       .s_axi_awburst                  (M00_AXI_AWBURST),	// 2
       .s_axi_awlock                   ({M00_AXI_AWLOCK}),	// 1
       .s_axi_awcache                  (M00_AXI_AWCACHE),	// 4
       .s_axi_awprot                   (M00_AXI_AWPROT),	// 3
       .s_axi_awqos                    (M00_AXI_AWQOS),		// 4
       .s_axi_awvalid                  (M00_AXI_AWVALID),	// 1
       .s_axi_awready                  (M00_AXI_AWREADY),
// Slave Interface Write Data Ports
       .s_axi_wdata                    (M00_AXI_WDATA),	// 128
       .s_axi_wstrb                    (M00_AXI_WSTRB),	// 16
       .s_axi_wlast                    (M00_AXI_WLAST),
       .s_axi_wvalid                   (M00_AXI_WVALID),
       .s_axi_wready                   (M00_AXI_WREADY),
// Slave Interface Write Response Ports
       .s_axi_bid                      (M00_AXI_BID),
       .s_axi_bresp                    (M00_AXI_BRESP),
       .s_axi_bvalid                   (M00_AXI_BVALID),
       .s_axi_bready                   (M00_AXI_BREADY),
// Slave Interface Read Address Ports
       .s_axi_arid                     (M00_AXI_ARID),	// 4
       .s_axi_araddr                   (M00_AXI_ARADDR),	// 28 (32)
       .s_axi_arlen                    (M00_AXI_ARLEN),	// 8
       .s_axi_arsize                   (M00_AXI_ARSIZE),	// 3
       .s_axi_arburst                  (M00_AXI_ARBURST),	// 2
       .s_axi_arlock                   (M00_AXI_ARLOCK),
       .s_axi_arcache                  (M00_AXI_ARCACHE),	// 4
       .s_axi_arprot                   (M00_AXI_ARPROT),	// 3
       .s_axi_arqos                    (M00_AXI_ARQOS),	// 4
       .s_axi_arvalid                  (M00_AXI_ARVALID),
       .s_axi_arready                  (M00_AXI_ARREADY),
// Slave Interface Read Data Ports
       .s_axi_rid                      (M00_AXI_RID),	// 4
       .s_axi_rdata                    (M00_AXI_RDATA),	// 128
       .s_axi_rresp                    (M00_AXI_RRESP),
       .s_axi_rlast                    (M00_AXI_RLAST),
       .s_axi_rvalid                   (M00_AXI_RVALID),
       .s_axi_rready                   (M00_AXI_RREADY),
       
// System Clock Ports
       .device_temp_i                  (tempmon),
       .sys_clk_i                      (sys_clk),
       .clk_ref_i                      (ref_clk),
       .sys_rst                        (sys_rst)
       );
// End of User Design top instance

wire [0 : 0] 	S00_AXI_AWID,	S01_AXI_AWID,	S02_AXI_AWID,	S03_AXI_AWID;
wire [27 : 0]	S00_AXI_AWADDR,	S01_AXI_AWADDR,	S02_AXI_AWADDR,	S03_AXI_AWADDR;
wire [7 : 0] 	S00_AXI_AWLEN,	S01_AXI_AWLEN,	S02_AXI_AWLEN,	S03_AXI_AWLEN;
wire [2 : 0] 	S_AXI_AWSIZE;
wire [1 : 0] 	S_AXI_AWBURST;
wire 		S_AXI_AWLOCK;
wire [3 : 0] 	S_AXI_AWCACHE;
wire [2 : 0] 	S_AXI_AWPROT;
wire [3 : 0] 	S_AXI_AWQOS;
wire 		S00_AXI_AWVALID,S01_AXI_AWVALID,S02_AXI_AWVALID,S03_AXI_AWVALID;
wire 		S00_AXI_AWREADY,S01_AXI_AWREADY,S02_AXI_AWREADY,S03_AXI_AWREADY;
wire [31 : 0]	S00_AXI_WDATA,	S01_AXI_WDATA,	S02_AXI_WDATA,	S03_AXI_WDATA;
wire [3 : 0] 	S00_AXI_WSTRB,	S01_AXI_WSTRB,	S02_AXI_WSTRB,	S03_AXI_WSTRB;
wire 		S00_AXI_WLAST,	S01_AXI_WLAST,	S02_AXI_WLAST,	S03_AXI_WLAST;
wire 		S00_AXI_WVALID,	S01_AXI_WVALID,	S02_AXI_WVALID,	S03_AXI_WVALID;
wire 		S00_AXI_WREADY,	S01_AXI_WREADY,	S02_AXI_WREADY,	S03_AXI_WREADY;
wire [0 : 0] 	S00_AXI_BID,	S01_AXI_BID,	S02_AXI_BID,	S03_AXI_BID;
wire [1 : 0] 	S00_AXI_BRESP,	S01_AXI_BRESP,	S02_AXI_BRESP,	S03_AXI_BRESP;
wire 		S00_AXI_BVALID,	S01_AXI_BVALID,	S02_AXI_BVALID,	S03_AXI_BVALID;
wire 		S00_AXI_BREADY,	S01_AXI_BREADY,	S02_AXI_BREADY,	S03_AXI_BREADY;
wire [0 : 0] 	S00_AXI_ARID,	S01_AXI_ARID,	S02_AXI_ARID,	S03_AXI_ARID;
wire [27 : 0]	S00_AXI_ARADDR,	S01_AXI_ARADDR,	S02_AXI_ARADDR,	S03_AXI_ARADDR;
wire [7 : 0] 	S00_AXI_ARLEN,	S01_AXI_ARLEN,	S02_AXI_ARLEN,	S03_AXI_ARLEN;
wire [2 : 0] 	S_AXI_ARSIZE;
wire [1 : 0] 	S_AXI_ARBURST;
wire 		S_AXI_ARLOCK;
wire [3 : 0] 	S_AXI_ARCACHE;
wire [2 : 0] 	S_AXI_ARPROT;
wire [3 : 0] 	S_AXI_ARQOS;
wire 		S00_AXI_ARVALID,S01_AXI_ARVALID,S02_AXI_ARVALID,S03_AXI_ARVALID;
wire 		S00_AXI_ARREADY,S01_AXI_ARREADY,S02_AXI_ARREADY,S03_AXI_ARREADY;
wire [0 : 0] 	S00_AXI_RID,	S01_AXI_RID,	S02_AXI_RID,	S03_AXI_RID;
wire [31 : 0] 	S00_AXI_RDATA,	S01_AXI_RDATA,	S02_AXI_RDATA,	S03_AXI_RDATA;
wire [1 : 0] 	S00_AXI_RRESP,	S01_AXI_RRESP,	S02_AXI_RRESP,	S03_AXI_RRESP;
wire 		S00_AXI_RLAST,	S01_AXI_RLAST,	S02_AXI_RLAST,	S03_AXI_RLAST;
wire 		S00_AXI_RVALID,	S01_AXI_RVALID,	S02_AXI_RVALID,	S03_AXI_RVALID;
wire 		S00_AXI_RREADY,	S01_AXI_RREADY,	S02_AXI_RREADY,	S03_AXI_RREADY;

assign S_AXI_AWSIZE  = 3'b010;	// The maximum number of bytes to transfer in each data transfer, or beat, in a burst.  2^2 = 4 byte
assign S_AXI_AWBURST = 2'b01;	// Burst type 00:FIXED 01:INCR 
assign S_AXI_AWLOCK  = 1'b0;	// AXI4 does not support locked transactions. 
assign S_AXI_AWCACHE = 4'b0011;	// Memory types 0:Device Non-bufferable  1:Device Bufferable 
assign S_AXI_AWPROT  = 3'b000;	// Access permissions pp71
assign S_AXI_AWQOS   = 4'b0;	// Quarity of service

assign S_AXI_ARSIZE  = 3'b010;	// The maximum number of bytes to transfer in each data transfer, or beat, in a burst.  2^2 = 4 byte
assign S_AXI_ARBURST = 2'b01;	// Burst type 00:FIXED 01:INCR 
assign S_AXI_ARLOCK  = 1'b0;	// AXI4 does not support locked transactions. 
assign S_AXI_ARCACHE = 4'b0011;	// Memory types 0:Device Non-bufferable  1:Device Bufferable 
assign S_AXI_ARPROT  = 3'b000;	// Access permissions pp71
assign S_AXI_ARQOS   = 4'b0;	// Quarity of service

// input 
assign S00_AXI_AWID = {1'b0};
assign S00_AXI_AWADDR = 28'h00000000;
assign S00_AXI_AWLEN = 8'd0;
assign S00_AXI_WDATA = 32'h00000000;
assign S00_AXI_WSTRB = 4'b0000;
assign S00_AXI_WLAST = 1'b0;
assign S00_AXI_ARID  = {1'b0};
assign S00_AXI_ARADDR = 28'h00000000;
assign S00_AXI_ARLEN = 8'd0;

assign S00_AXI_AWVALID = 1'b0;
assign S00_AXI_WVALID  = 1'b0;
assign S00_AXI_ARVALID = 1'b0;
assign S00_AXI_BREADY  = 1'b1;
assign S00_AXI_RREADY  = 1'b1;

assign S02_AXI_AWID = {1'b0};
assign S02_AXI_AWADDR = 28'h00000000;
assign S02_AXI_AWLEN = 8'd0;
assign S02_AXI_WDATA = 32'h00000000;
assign S02_AXI_WSTRB = 4'b0000;
assign S02_AXI_WLAST = 1'b0;
assign S02_AXI_ARID  = {1'b0};
assign S02_AXI_ARADDR = 28'h00000000;
assign S02_AXI_ARLEN = 8'd0;

assign S02_AXI_AWVALID = 1'b0;
assign S02_AXI_WVALID  = 1'b0;
assign S02_AXI_ARVALID = 1'b0;
assign S02_AXI_BREADY  = 1'b1;
assign S02_AXI_RREADY  = 1'b1;

//***************************************************************************
axi_ic u_axi_ic (
  .INTERCONNECT_ACLK	(aclk),        		// input wire INTERCONNECT_ACLK
  .INTERCONNECT_ARESETN	(arst_n),  		// input wire INTERCONNECT_ARESETN
  .S00_AXI_ARESET_OUT_N	(),  			// output wire S00_AXI_ARESET_OUT_N
  .S00_AXI_ACLK		(aclk),                 // input wire S00_AXI_ACLK
  .S00_AXI_AWID		(S00_AXI_AWID),       	// input wire [0 : 0] S00_AXI_AWID
  .S00_AXI_AWADDR	({4'd0,S00_AXI_AWADDR}),       // input wire [31 : 0] S00_AXI_AWADDR
  .S00_AXI_AWLEN	(S00_AXI_AWLEN),        // input wire [7 : 0] S00_AXI_AWLEN
  .S00_AXI_AWSIZE	(S_AXI_AWSIZE),       // input wire [2 : 0] S00_AXI_AWSIZE
  .S00_AXI_AWBURST	(S_AXI_AWBURST),      // input wire [1 : 0] S00_AXI_AWBURST
  .S00_AXI_AWLOCK	(S_AXI_AWLOCK),       // input wire S00_AXI_AWLOCK
  .S00_AXI_AWCACHE	(S_AXI_AWCACHE),      // input wire [3 : 0] S00_AXI_AWCACHE
  .S00_AXI_AWPROT	(S_AXI_AWPROT),       // input wire [2 : 0] S00_AXI_AWPROT
  .S00_AXI_AWQOS	(S_AXI_AWQOS),        // input wire [3 : 0] S00_AXI_AWQOS
  .S00_AXI_AWVALID	(S00_AXI_AWVALID),      // input wire S00_AXI_AWVALID
  .S00_AXI_AWREADY	(S00_AXI_AWREADY),      // output wire S00_AXI_AWREADY
  .S00_AXI_WDATA	(S00_AXI_WDATA),        // input wire [31 : 0] S00_AXI_WDATA
  .S00_AXI_WSTRB	(S00_AXI_WSTRB),        // input wire [3 : 0] S00_AXI_WSTRB
  .S00_AXI_WLAST	(S00_AXI_WLAST),        // input wire S00_AXI_WLAST
  .S00_AXI_WVALID	(S00_AXI_WVALID),       // input wire S00_AXI_WVALID
  .S00_AXI_WREADY	(S00_AXI_WREADY),       // output wire S00_AXI_WREADY
  .S00_AXI_BID		(S00_AXI_BID),          // output wire [0 : 0] S00_AXI_BID
  .S00_AXI_BRESP	(S00_AXI_BRESP),        // output wire [1 : 0] S00_AXI_BRESP
  .S00_AXI_BVALID	(S00_AXI_BVALID),       // output wire S00_AXI_BVALID
  .S00_AXI_BREADY	(S00_AXI_BREADY),       // input wire S00_AXI_BREADY
  .S00_AXI_ARID		(S00_AXI_ARID),         // input wire [0 : 0] S00_AXI_ARID
  .S00_AXI_ARADDR	({4'd0,S00_AXI_ARADDR}),       // input wire [31 : 0] S00_AXI_ARADDR
  .S00_AXI_ARLEN	(S00_AXI_ARLEN),        // input wire [7 : 0] S00_AXI_ARLEN
  .S00_AXI_ARSIZE	(S_AXI_ARSIZE),       // input wire [2 : 0] S00_AXI_ARSIZE
  .S00_AXI_ARBURST	(S_AXI_ARBURST),      // input wire [1 : 0] S00_AXI_ARBURST
  .S00_AXI_ARLOCK	(S_AXI_ARLOCK),       // input wire S00_AXI_ARLOCK
  .S00_AXI_ARCACHE	(S_AXI_ARCACHE),      // input wire [3 : 0] S00_AXI_ARCACHE
  .S00_AXI_ARPROT	(S_AXI_ARPROT),       // input wire [2 : 0] S00_AXI_ARPROT
  .S00_AXI_ARQOS	(S_AXI_ARQOS),        // input wire [3 : 0] S00_AXI_ARQOS
  .S00_AXI_ARVALID	(S00_AXI_ARVALID),      // input wire S00_AXI_ARVALID
  .S00_AXI_ARREADY	(S00_AXI_ARREADY),      // output wire S00_AXI_ARREADY
  .S00_AXI_RID		(S00_AXI_RID),          // output wire [0 : 0] S00_AXI_RID
  .S00_AXI_RDATA	(S00_AXI_RDATA),        // output wire [31 : 0] S00_AXI_RDATA
  .S00_AXI_RRESP	(S00_AXI_RRESP),        // output wire [1 : 0] S00_AXI_RRESP
  .S00_AXI_RLAST	(S00_AXI_RLAST),        // output wire S00_AXI_RLAST
  .S00_AXI_RVALID	(S00_AXI_RVALID),       // output wire S00_AXI_RVALID
  .S00_AXI_RREADY	(S00_AXI_RREADY),       // input wire S00_AXI_RREADY

  .S01_AXI_ARESET_OUT_N	(),  			// output wire S01_AXI_ARESET_OUT_N
  .S01_AXI_ACLK		(aclk),                 // input wire S01_AXI_ACLK
  .S01_AXI_AWID		(S01_AXI_AWID),         // input wire [0 : 0] S01_AXI_AWID
  .S01_AXI_AWADDR	({4'd0,S01_AXI_AWADDR}),       // input wire [31 : 0] S01_AXI_AWADDR
  .S01_AXI_AWLEN	(S01_AXI_AWLEN),        // input wire [7 : 0] S01_AXI_AWLEN
  .S01_AXI_AWSIZE	(S_AXI_AWSIZE),       // input wire [2 : 0] S01_AXI_AWSIZE
  .S01_AXI_AWBURST	(S_AXI_AWBURST),      // input wire [1 : 0] S01_AXI_AWBURST
  .S01_AXI_AWLOCK	(S_AXI_AWLOCK),       // input wire S01_AXI_AWLOCK
  .S01_AXI_AWCACHE	(S_AXI_AWCACHE),      // input wire [3 : 0] S01_AXI_AWCACHE
  .S01_AXI_AWPROT	(S_AXI_AWPROT),       // input wire [2 : 0] S01_AXI_AWPROT
  .S01_AXI_AWQOS	(S_AXI_AWQOS),        // input wire [3 : 0] S01_AXI_AWQOS
  .S01_AXI_AWVALID	(S01_AXI_AWVALID),      // input wire S01_AXI_AWVALID
  .S01_AXI_AWREADY	(S01_AXI_AWREADY),      // output wire S01_AXI_AWREADY
  .S01_AXI_WDATA	(S01_AXI_WDATA),        // input wire [31 : 0] S01_AXI_WDATA
  .S01_AXI_WSTRB	(S01_AXI_WSTRB),        // input wire [3 : 0] S01_AXI_WSTRB
  .S01_AXI_WLAST	(S01_AXI_WLAST),        // input wire S01_AXI_WLAST
  .S01_AXI_WVALID	(S01_AXI_WVALID),       // input wire S01_AXI_WVALID
  .S01_AXI_WREADY	(S01_AXI_WREADY),       // output wire S01_AXI_WREADY
  .S01_AXI_BID		(S01_AXI_BID),          // output wire [0 : 0] S01_AXI_BID
  .S01_AXI_BRESP	(S01_AXI_BRESP),        // output wire [1 : 0] S01_AXI_BRESP
  .S01_AXI_BVALID	(S01_AXI_BVALID),       // output wire S01_AXI_BVALID
  .S01_AXI_BREADY	(S01_AXI_BREADY),       // input wire S01_AXI_BREADY
  .S01_AXI_ARID		(S01_AXI_ARID),         // input wire [0 : 0] S01_AXI_ARID
  .S01_AXI_ARADDR	({4'd0,S01_AXI_ARADDR}),       // input wire [31 : 0] S01_AXI_ARADDR
  .S01_AXI_ARLEN	(S01_AXI_ARLEN),        // input wire [7 : 0] S01_AXI_ARLEN
  .S01_AXI_ARSIZE	(S_AXI_ARSIZE),       // input wire [2 : 0] S01_AXI_ARSIZE
  .S01_AXI_ARBURST	(S_AXI_ARBURST),      // input wire [1 : 0] S01_AXI_ARBURST
  .S01_AXI_ARLOCK	(S_AXI_ARLOCK),       // input wire S01_AXI_ARLOCK
  .S01_AXI_ARCACHE	(S_AXI_ARCACHE),      // input wire [3 : 0] S01_AXI_ARCACHE
  .S01_AXI_ARPROT	(S_AXI_ARPROT),       // input wire [2 : 0] S01_AXI_ARPROT
  .S01_AXI_ARQOS	(S_AXI_ARQOS),        // input wire [3 : 0] S01_AXI_ARQOS
  .S01_AXI_ARVALID	(S01_AXI_ARVALID),      // input wire S01_AXI_ARVALID
  .S01_AXI_ARREADY	(S01_AXI_ARREADY),      // output wire S01_AXI_ARREADY
  .S01_AXI_RID		(S01_AXI_RID),          // output wire [0 : 0] S01_AXI_RID
  .S01_AXI_RDATA	(S01_AXI_RDATA),        // output wire [31 : 0] S01_AXI_RDATA
  .S01_AXI_RRESP	(S01_AXI_RRESP),        // output wire [1 : 0] S01_AXI_RRESP
  .S01_AXI_RLAST	(S01_AXI_RLAST),        // output wire S01_AXI_RLAST
  .S01_AXI_RVALID	(S01_AXI_RVALID),       // output wire S01_AXI_RVALID
  .S01_AXI_RREADY	(S01_AXI_RREADY),       // input wire S01_AXI_RREADY

  .S02_AXI_ARESET_OUT_N	(),  			// output wire S02_AXI_ARESET_OUT_N
  .S02_AXI_ACLK		(aclk),                 // input wire S02_AXI_ACLK
  .S02_AXI_AWID		(S02_AXI_AWID),         // input wire [0 : 0] S02_AXI_AWID
  .S02_AXI_AWADDR	({4'd0,S02_AXI_AWADDR}),       // input wire [31 : 0] S02_AXI_AWADDR
  .S02_AXI_AWLEN	(S02_AXI_AWLEN),        // input wire [7 : 0] S02_AXI_AWLEN
  .S02_AXI_AWSIZE	(S_AXI_AWSIZE),       // input wire [2 : 0] S02_AXI_AWSIZE
  .S02_AXI_AWBURST	(S_AXI_AWBURST),      // input wire [1 : 0] S02_AXI_AWBURST
  .S02_AXI_AWLOCK	(S_AXI_AWLOCK),       // input wire S02_AXI_AWLOCK
  .S02_AXI_AWCACHE	(S_AXI_AWCACHE),      // input wire [3 : 0] S02_AXI_AWCACHE
  .S02_AXI_AWPROT	(S_AXI_AWPROT),       // input wire [2 : 0] S02_AXI_AWPROT
  .S02_AXI_AWQOS	(S_AXI_AWQOS),        // input wire [3 : 0] S02_AXI_AWQOS
  .S02_AXI_AWVALID	(S02_AXI_AWVALID),      // input wire S02_AXI_AWVALID
  .S02_AXI_AWREADY	(S02_AXI_AWREADY),      // output wire S02_AXI_AWREADY
  .S02_AXI_WDATA	(S02_AXI_WDATA),        // input wire [31 : 0] S02_AXI_WDATA
  .S02_AXI_WSTRB	(S02_AXI_WSTRB),        // input wire [3 : 0] S02_AXI_WSTRB
  .S02_AXI_WLAST	(S02_AXI_WLAST),        // input wire S02_AXI_WLAST
  .S02_AXI_WVALID	(S02_AXI_WVALID),       // input wire S02_AXI_WVALID
  .S02_AXI_WREADY	(S02_AXI_WREADY),       // output wire S02_AXI_WREADY
  .S02_AXI_BID		(S02_AXI_BID),          // output wire [0 : 0] S02_AXI_BID
  .S02_AXI_BRESP	(S02_AXI_BRESP),        // output wire [1 : 0] S02_AXI_BRESP
  .S02_AXI_BVALID	(S02_AXI_BVALID),       // output wire S02_AXI_BVALID
  .S02_AXI_BREADY	(S02_AXI_BREADY),       // input wire S02_AXI_BREADY
  .S02_AXI_ARID		(S02_AXI_ARID),         // input wire [0 : 0] S02_AXI_ARID
  .S02_AXI_ARADDR	({4'd0,S02_AXI_ARADDR}),       // input wire [31 : 0] S02_AXI_ARADDR
  .S02_AXI_ARLEN	(S02_AXI_ARLEN),        // input wire [7 : 0] S02_AXI_ARLEN
  .S02_AXI_ARSIZE	(S_AXI_ARSIZE),       // input wire [2 : 0] S02_AXI_ARSIZE
  .S02_AXI_ARBURST	(S_AXI_ARBURST),      // input wire [1 : 0] S02_AXI_ARBURST
  .S02_AXI_ARLOCK	(S_AXI_ARLOCK),       // input wire S02_AXI_ARLOCK
  .S02_AXI_ARCACHE	(S_AXI_ARCACHE),      // input wire [3 : 0] S02_AXI_ARCACHE
  .S02_AXI_ARPROT	(S_AXI_ARPROT),       // input wire [2 : 0] S02_AXI_ARPROT
  .S02_AXI_ARQOS	(S_AXI_ARQOS),        // input wire [3 : 0] S02_AXI_ARQOS
  .S02_AXI_ARVALID	(S02_AXI_ARVALID),      // input wire S02_AXI_ARVALID
  .S02_AXI_ARREADY	(S02_AXI_ARREADY),      // output wire S02_AXI_ARREADY
  .S02_AXI_RID		(S02_AXI_RID),          // output wire [0 : 0] S02_AXI_RID
  .S02_AXI_RDATA	(S02_AXI_RDATA),        // output wire [31 : 0] S02_AXI_RDATA
  .S02_AXI_RRESP	(S02_AXI_RRESP),        // output wire [1 : 0] S02_AXI_RRESP
  .S02_AXI_RLAST	(S02_AXI_RLAST),        // output wire S02_AXI_RLAST
  .S02_AXI_RVALID	(S02_AXI_RVALID),       // output wire S02_AXI_RVALID
  .S02_AXI_RREADY	(S02_AXI_RREADY),       // input wire S02_AXI_RREADY

  .S03_AXI_ARESET_OUT_N	(),  			// output wire S03_AXI_ARESET_OUT_N
  .S03_AXI_ACLK		(aclk),                 // input wire S03_AXI_ACLK
  .S03_AXI_AWID		(S03_AXI_AWID),           // input wire [0 : 0] S03_AXI_AWID
  .S03_AXI_AWADDR	({4'd0,S03_AXI_AWADDR}),         // input wire [31 : 0] S03_AXI_AWADDR
  .S03_AXI_AWLEN	(S03_AXI_AWLEN),          // input wire [7 : 0] S03_AXI_AWLEN
  .S03_AXI_AWSIZE	(S_AXI_AWSIZE),         // input wire [2 : 0] S03_AXI_AWSIZE
  .S03_AXI_AWBURST	(S_AXI_AWBURST),        // input wire [1 : 0] S03_AXI_AWBURST
  .S03_AXI_AWLOCK	(S_AXI_AWLOCK),         // input wire S03_AXI_AWLOCK
  .S03_AXI_AWCACHE	(S_AXI_AWCACHE),        // input wire [3 : 0] S03_AXI_AWCACHE
  .S03_AXI_AWPROT	(S_AXI_AWPROT),         // input wire [2 : 0] S03_AXI_AWPROT
  .S03_AXI_AWQOS	(4'b0),                // input wire [3 : 0] S03_AXI_AWQOS
  .S03_AXI_AWVALID	(S03_AXI_AWVALID),        // input wire S03_AXI_AWVALID
  .S03_AXI_AWREADY	(S03_AXI_AWREADY),        // output wire S03_AXI_AWREADY
  .S03_AXI_WDATA	(S03_AXI_WDATA),          // input wire [31 : 0] S03_AXI_WDATA
  .S03_AXI_WSTRB	(S03_AXI_WSTRB),          // input wire [3 : 0] S03_AXI_WSTRB
  .S03_AXI_WLAST	(S03_AXI_WLAST),          // input wire S03_AXI_WLAST
  .S03_AXI_WVALID	(S03_AXI_WVALID),         // input wire S03_AXI_WVALID
  .S03_AXI_WREADY	(S03_AXI_WREADY),         // output wire S03_AXI_WREADY
  .S03_AXI_BID		(S03_AXI_BID),            // output wire [0 : 0] S03_AXI_BID
  .S03_AXI_BRESP	(S03_AXI_BRESP),          // output wire [1 : 0] S03_AXI_BRESP
  .S03_AXI_BVALID	(S03_AXI_BVALID),         // output wire S03_AXI_BVALID
  .S03_AXI_BREADY	(S03_AXI_BREADY),         // input wire S03_AXI_BREADY
  .S03_AXI_ARID		(S03_AXI_ARID),           // input wire [0 : 0] S03_AXI_ARID
  .S03_AXI_ARADDR	({4'd0,S03_AXI_ARADDR}),         // input wire [31 : 0] S03_AXI_ARADDR
  .S03_AXI_ARLEN	(S03_AXI_ARLEN),          // input wire [7 : 0] S03_AXI_ARLEN
  .S03_AXI_ARSIZE	(S_AXI_ARSIZE),         // input wire [2 : 0] S03_AXI_ARSIZE
  .S03_AXI_ARBURST	(S_AXI_ARBURST),        // input wire [1 : 0] S03_AXI_ARBURST
  .S03_AXI_ARLOCK	(S_AXI_ARLOCK),         // input wire S03_AXI_ARLOCK
  .S03_AXI_ARCACHE	(S_AXI_ARCACHE),        // input wire [3 : 0] S03_AXI_ARCACHE
  .S03_AXI_ARPROT	(S_AXI_ARPROT),         // input wire [2 : 0] S03_AXI_ARPROT
  .S03_AXI_ARQOS	(4'b0),                // input wire [3 : 0] S03_AXI_ARQOS
  .S03_AXI_ARVALID	(S03_AXI_ARVALID),        // input wire S03_AXI_ARVALID
  .S03_AXI_ARREADY	(S03_AXI_ARREADY),        // output wire S03_AXI_ARREADY
  .S03_AXI_RID		(S03_AXI_RID),            // output wire [0 : 0] S03_AXI_RID
  .S03_AXI_RDATA	(S03_AXI_RDATA),          // output wire [31 : 0] S03_AXI_RDATA
  .S03_AXI_RRESP	(S03_AXI_RRESP),          // output wire [1 : 0] S03_AXI_RRESP
  .S03_AXI_RLAST	(S03_AXI_RLAST),          // output wire S03_AXI_RLAST
  .S03_AXI_RVALID	(S03_AXI_RVALID),         // output wire S03_AXI_RVALID
  .S03_AXI_RREADY	(S03_AXI_RREADY),         // input wire S03_AXI_RREADY

  .M00_AXI_ARESET_OUT_N	(), 			// output wire M00_AXI_ARESET_OUT_N
  .M00_AXI_ACLK		(aclk),                 // input wire M00_AXI_ACLK
  .M00_AXI_AWID		(M00_AXI_AWID),         // output wire [3 : 0] M00_AXI_AWID
  .M00_AXI_AWADDR	(M00_AXI_AWADDR),       // output wire [31 : 0] M00_AXI_AWADDR
  .M00_AXI_AWLEN	(M00_AXI_AWLEN),        // output wire [7 : 0] M00_AXI_AWLEN
  .M00_AXI_AWSIZE	(M00_AXI_AWSIZE),       // output wire [2 : 0] M00_AXI_AWSIZE
  .M00_AXI_AWBURST	(M00_AXI_AWBURST),      // output wire [1 : 0] M00_AXI_AWBURST
  .M00_AXI_AWLOCK	(M00_AXI_AWLOCK),       // output wire M00_AXI_AWLOCK
  .M00_AXI_AWCACHE	(M00_AXI_AWCACHE),      // output wire [3 : 0] M00_AXI_AWCACHE
  .M00_AXI_AWPROT	(M00_AXI_AWPROT),       // output wire [2 : 0] M00_AXI_AWPROT
  .M00_AXI_AWQOS	(M00_AXI_AWQOS),        // output wire [3 : 0] M00_AXI_AWQOS
  .M00_AXI_AWVALID	(M00_AXI_AWVALID),      // output wire M00_AXI_AWVALID
  .M00_AXI_AWREADY	(M00_AXI_AWREADY),      // input wire M00_AXI_AWREADY
  .M00_AXI_WDATA	(M00_AXI_WDATA),        // output wire [127 : 0] M00_AXI_WDATA
  .M00_AXI_WSTRB	(M00_AXI_WSTRB),        // output wire [15 : 0] M00_AXI_WSTRB
  .M00_AXI_WLAST	(M00_AXI_WLAST),        // output wire M00_AXI_WLAST
  .M00_AXI_WVALID	(M00_AXI_WVALID),       // output wire M00_AXI_WVALID
  .M00_AXI_WREADY	(M00_AXI_WREADY),       // input wire M00_AXI_WREADY
  .M00_AXI_BID		(M00_AXI_BID),          // input wire [3 : 0] M00_AXI_BID
  .M00_AXI_BRESP	(M00_AXI_BRESP),        // input wire [1 : 0] M00_AXI_BRESP
  .M00_AXI_BVALID	(M00_AXI_BVALID),       // input wire M00_AXI_BVALID
  .M00_AXI_BREADY	(M00_AXI_BREADY),       // output wire M00_AXI_BREADY
  .M00_AXI_ARID		(M00_AXI_ARID),         // output wire [3 : 0] M00_AXI_ARID
  .M00_AXI_ARADDR	(M00_AXI_ARADDR),       // output wire [31 : 0] M00_AXI_ARADDR
  .M00_AXI_ARLEN	(M00_AXI_ARLEN),        // output wire [7 : 0] M00_AXI_ARLEN
  .M00_AXI_ARSIZE	(M00_AXI_ARSIZE),       // output wire [2 : 0] M00_AXI_ARSIZE
  .M00_AXI_ARBURST	(M00_AXI_ARBURST),      // output wire [1 : 0] M00_AXI_ARBURST
  .M00_AXI_ARLOCK	(M00_AXI_ARLOCK),       // output wire M00_AXI_ARLOCK
  .M00_AXI_ARCACHE	(M00_AXI_ARCACHE),      // output wire [3 : 0] M00_AXI_ARCACHE
  .M00_AXI_ARPROT	(M00_AXI_ARPROT),       // output wire [2 : 0] M00_AXI_ARPROT
  .M00_AXI_ARQOS	(M00_AXI_ARQOS),        // output wire [3 : 0] M00_AXI_ARQOS
  .M00_AXI_ARVALID	(M00_AXI_ARVALID),      // output wire M00_AXI_ARVALID
  .M00_AXI_ARREADY	(M00_AXI_ARREADY),      // input wire M00_AXI_ARREADY
  .M00_AXI_RID		(M00_AXI_RID),          // input wire [3 : 0] M00_AXI_RID
  .M00_AXI_RDATA	(M00_AXI_RDATA),        // input wire [127 : 0] M00_AXI_RDATA
  .M00_AXI_RRESP	(M00_AXI_RRESP),        // input wire [1 : 0] M00_AXI_RRESP
  .M00_AXI_RLAST	(M00_AXI_RLAST),        // input wire M00_AXI_RLAST
  .M00_AXI_RVALID	(M00_AXI_RVALID),       // input wire M00_AXI_RVALID
  .M00_AXI_RREADY	(M00_AXI_RREADY)        // output wire M00_AXI_RREADY
);

assign S01_AXI_BREADY  = 1'b1;
//assign S03_AXI_BREADY  = 1'b1;

assign S01_AXI_AWID = {1'b0};
assign S01_AXI_AWADDR = 28'h00000000;
assign S01_AXI_AWLEN = 8'd0;
assign S01_AXI_WDATA = 32'h00000000;
assign S01_AXI_WSTRB = 4'b0000;
assign S01_AXI_WLAST = 1'b0;
assign S01_AXI_ARID  = {1'b0};
assign S01_AXI_ARADDR = 28'h00000000;
assign S01_AXI_ARLEN = 8'd0;

assign S01_AXI_AWVALID = 1'b0;
assign S01_AXI_WVALID  = 1'b0;
assign S01_AXI_ARVALID = 1'b0;
assign S01_AXI_RREADY  = 1'b1;

rv_cache_unit #(.ICACHE_EN(ICACHE_EN)) u_rv_cache_unit (
//-- bus
       .cclk         (cclk),		// in  std_logic;
       .xreset       (xreset),	// in  std_logic;
       .adr          (adr),		// in  unsigned(31 downto 0);
       .cs           (cs),		// in  std_logic;
       .we           (we),		// in  std_logic_vector(3 downto 0);
       .re           (re),		// in  std_logic;
       .rdy          (rdy),		// out std_logic;
       .dw           (dw),		// in  unsigned(31 downto 0);
       .dr           (dr),		// out unsigned(31 downto 0);

       .i_adr        (i_adr),	// in  unsigned(31 downto 0);
       .i_re         (i_re),		// in  std_logic;
       .i_dr         (i_dr),		// out unsigned(31 downto 0);

       .cal_cmpl     (init_calib_complete),
//-- axi i/f
       .aclk         (aclk),		// in  std_logic;
       .arst_n       (arst_n),	// in	std_logic;

       .awid         (S03_AXI_AWID),		// out	std_logic;
       .awaddr       (S03_AXI_AWADDR),	// out	unsigned(27 downto 0);
       .awlen        (S03_AXI_AWLEN),	// out	unsigned(7 downto 0);
       .awvalid      (S03_AXI_AWVALID),	// out	std_logic;
       .awready      (S03_AXI_AWREADY),	// in	std_logic;

       .wdata        (S03_AXI_WDATA),	// out	unsigned(31 downto 0);
       .wstrb        (S03_AXI_WSTRB),	// out	unsigned(3 downto 0);
       .wlast        (S03_AXI_WLAST),	// out	std_logic;
       .wvalid       (S03_AXI_WVALID),	// out	std_logic;
       .wready       (S03_AXI_WREADY),	// in	std_logic;

       .bid          (S03_AXI_BID),		// in	std_logic;
       .bresp        (S03_AXI_BRESP),	// in	unsigned(1 downto 0);
       .bvalid       (S03_AXI_BVALID),	// in	std_logic;
       .bready       (S03_AXI_BREADY),	// out	std_logic;

       .arid         (S03_AXI_ARID),		// out	std_logic;
       .araddr       (S03_AXI_ARADDR),	// out	unsigned(27 downto 0);
       .arlen        (S03_AXI_ARLEN),	// out	unsigned(7 downto 0);
       .arvalid      (S03_AXI_ARVALID),	// out	std_logic;
       .arready      (S03_AXI_ARREADY),	// in	std_logic;

       .rid          (S03_AXI_RID),		// in	std_logic;
       .rdata        (S03_AXI_RDATA),	// in	unsigned(31 downto 0);
       .rresp        (S03_AXI_RRESP),	// in	unsigned(1 downto 0);
       .rlast        (S03_AXI_RLAST),	// in	std_logic;
       .rvalid       (S03_AXI_RVALID),	// in	std_logic;
       .rready       (S03_AXI_RREADY),	// out	std_logic;

//-- debug
       .fp    ()	// out unsigned(3 downto 0)
    );


endmodule

