`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 03.06.2019 16:57:52
// Design Name: 
// Module Name: crypto_top
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module crypto_top #(
        // Parameters of Axi Slave Bus Interface S00_AXI
        parameter integer C_S00_AXI_DATA_WIDTH    = 32,
        parameter integer C_S00_AXI_ADDR_WIDTH    = 4,
        
        // Parameters of Axi Slave Bus Interface S00_AXIS
        parameter integer C_S00_AXIS_TDATA_WIDTH    = 32,
        
        // Parameters of Axi Master Bus Interface M00_AXIS
        parameter integer C_M00_AXIS_TDATA_WIDTH    = 32,
        parameter integer C_M00_AXIS_START_COUNT    = 32
)(
// Ports of Axi Slave Bus Interface S00_AXI
		input wire  s00_axi_aclk,
		input wire  s00_axi_aresetn,
		input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_awaddr,
		input wire [2 : 0] s00_axi_awprot,
		input wire  s00_axi_awvalid,
		output wire  s00_axi_awready,
		input wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_wdata,
		input wire [(C_S00_AXI_DATA_WIDTH/8)-1 : 0] s00_axi_wstrb,
		input wire  s00_axi_wvalid,
		output wire  s00_axi_wready,
		output wire [1 : 0] s00_axi_bresp,
		output wire  s00_axi_bvalid,
		input wire  s00_axi_bready,
		input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_araddr,
		input wire [2 : 0] s00_axi_arprot,
		input wire  s00_axi_arvalid,
		output wire  s00_axi_arready,
		output wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_rdata,
		output wire [1 : 0] s00_axi_rresp,
		output wire  s00_axi_rvalid,
		input wire  s00_axi_rready,

		// Ports of Axi Slave Bus Interface S00_AXIS
		input wire  s00_axis_aclk,
		input wire  s00_axis_aresetn,
		output wire  s00_axis_tready,
		input wire [C_S00_AXIS_TDATA_WIDTH-1 : 0] s00_axis_tdata,
		input wire [(C_S00_AXIS_TDATA_WIDTH/8)-1 : 0] s00_axis_tstrb,
		input wire  s00_axis_tlast,
		input wire  s00_axis_tvalid,

		// Ports of Axi Master Bus Interface M00_AXIS
		input wire  m00_axis_aclk,
		input wire  m00_axis_aresetn,
		output wire  m00_axis_tvalid,
		output wire [C_M00_AXIS_TDATA_WIDTH-1 : 0] m00_axis_tdata,
		output wire [(C_M00_AXIS_TDATA_WIDTH/8)-1 : 0] m00_axis_tstrb,
		output wire  m00_axis_tlast,
		input wire  m00_axis_tready
    );

present_0 present_inst (
  .s00_axi_aclk(s00_axi_aclk),          // input wire s00_axi_aclk
  .s00_axi_aresetn(s00_axi_aresetn),    // input wire s00_axi_aresetn
  .s00_axi_awaddr(s00_axi_awaddr),      // input wire [3 : 0] s00_axi_awaddr
  .s00_axi_awprot(s00_axi_awprot),      // input wire [2 : 0] s00_axi_awprot
  .s00_axi_awvalid(s00_axi_awvalid),    // input wire s00_axi_awvalid
  .s00_axi_awready(s00_axi_awready),    // output wire s00_axi_awready
  .s00_axi_wdata(s00_axi_wdata),        // input wire [31 : 0] s00_axi_wdata
  .s00_axi_wstrb(s00_axi_wstrb),        // input wire [3 : 0] s00_axi_wstrb
  .s00_axi_wvalid(s00_axi_wvalid),      // input wire s00_axi_wvalid
  .s00_axi_wready(s00_axi_wready),      // output wire s00_axi_wready
  .s00_axi_bresp(s00_axi_bresp),        // output wire [1 : 0] s00_axi_bresp
  .s00_axi_bvalid(s00_axi_bvalid),      // output wire s00_axi_bvalid
  .s00_axi_bready(s00_axi_bready),      // input wire s00_axi_bready
  .s00_axi_araddr(s00_axi_araddr),      // input wire [3 : 0] s00_axi_araddr
  .s00_axi_arprot(s00_axi_arprot),      // input wire [2 : 0] s00_axi_arprot
  .s00_axi_arvalid(s00_axi_arvalid),    // input wire s00_axi_arvalid
  .s00_axi_arready(s00_axi_arready),    // output wire s00_axi_arready
  .s00_axi_rdata(s00_axi_rdata),        // output wire [31 : 0] s00_axi_rdata
  .s00_axi_rresp(s00_axi_rresp),        // output wire [1 : 0] s00_axi_rresp
  .s00_axi_rvalid(s00_axi_rvalid),      // output wire s00_axi_rvalid
  .s00_axi_rready(s00_axi_rready),      // input wire s00_axi_rready
  .s00_axis_aclk(s00_axis_aclk),        // input wire s00_axis_aclk
  .s00_axis_aresetn(s00_axis_aresetn),  // input wire s00_axis_aresetn
  .s00_axis_tready(s00_axis_tready),    // output wire s00_axis_tready
  .s00_axis_tdata(s00_axis_tdata),      // input wire [31 : 0] s00_axis_tdata
  .s00_axis_tstrb(s00_axis_tstrb),      // input wire [3 : 0] s00_axis_tstrb
  .s00_axis_tlast(s00_axis_tlast),      // input wire s00_axis_tlast
  .s00_axis_tvalid(s00_axis_tvalid),    // input wire s00_axis_tvalid
  .m00_axis_aclk(m00_axis_aclk),        // input wire m00_axis_aclk
  .m00_axis_aresetn(m00_axis_aresetn),  // input wire m00_axis_aresetn
  .m00_axis_tvalid(m00_axis_tvalid),    // output wire m00_axis_tvalid
  .m00_axis_tdata(m00_axis_tdata),      // output wire [31 : 0] m00_axis_tdata
  .m00_axis_tstrb(m00_axis_tstrb),      // output wire [3 : 0] m00_axis_tstrb
  .m00_axis_tlast(m00_axis_tlast),      // output wire m00_axis_tlast
  .m00_axis_tready(m00_axis_tready)    // input wire m00_axis_tready
);
endmodule
