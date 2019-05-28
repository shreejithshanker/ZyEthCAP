`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 28.05.2019 12:29:40
// Design Name: 
// Module Name: present_tb
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


module present_tb();

reg CLK, RST = 0;

// AXI

// AXIS_SLAVE
reg [31:0] s_axis_tdata;
reg s_axis_tvalid;

// AXIS_MASTER


present_v1_0 dut (
.s00_axi_aclk(CLK),
.s00_axi_aresetn(RST),
.s00_axi_awaddr(),
//input wire [2 : 0] s00_axi_awprot,
.s00_axi_awvalid(),
.s00_axi_awready(),
.s00_axi_wdata(),
.s00_axi_wstrb(),
.s00_axi_wvalid(),
.s00_axi_wready(),
.s00_axi_bresp(),
.s00_axi_bvalid(),
.s00_axi_bready(),
.s00_axi_araddr(),
//input wire [2 : 0] s00_axi_arprot,
.s00_axi_arvalid(),
.s00_axi_arready(),
.s00_axi_rdata(),
.s00_axi_rresp(),
.s00_axi_rvalid(),
.s00_axi_rready(),

// Ports of Axi Slave Bus Interface S00_AXIS
.s00_axis_aclk(CLK),
.s00_axis_aresetn(RST),
.s00_axis_tready(),
.s00_axis_tdata(s_axis_tdata),
//input wire [(C_S00_AXIS_TDATA_WIDTH/8)-1 : 0] s00_axis_tstrb,
//.s00_axis_tlast,
.s00_axis_tvalid(s_axis_tvalid),

// Ports of Axi Master Bus Interface M00_AXIS
.m00_axis_aclk(CLK),
.m00_axis_aresetn(RST),
.m00_axis_tvalid(),
.m00_axis_tdata(),
//output wire [(C_M00_AXIS_TDATA_WIDTH/8)-1 : 0] m00_axis_tstrb,
//output wire  m00_axis_tlast,
.m00_axis_tready()

);

parameter PERIOD = 10;

always begin
  CLK = 1'b0;
  #(PERIOD/2) CLK = 1'b1;
  #(PERIOD/2);
end

initial begin
    RST = 0;
    s_axis_tdata = 32;
    #20 RST = 1;
    #20 s_axis_tvalid = 1;
end


endmodule
