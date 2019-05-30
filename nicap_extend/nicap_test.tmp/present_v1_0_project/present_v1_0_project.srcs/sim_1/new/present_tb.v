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

///////////////////////////  AXI_LITE (MASTER)  //////////////////////////////////
reg M_AW_VALID = 0;
reg [3:0] M_AW_ADDR = 0;
reg [3:0] M_W_STRB = 4'b1111;
wire M_AW_READY;

reg M_W_VALID = 0;
wire M_W_READY;
reg [31:0] M_W_DATA = 0;

reg [3:0] M_AR_ADDR = 0;
reg M_AR_VALID = 0;
wire M_AR_READY;

reg M_B_READY = 0;
wire M_B_VALID = 0;
wire M_B_RESP; // Write information (not required)

///////////////////////////  AXI_STREAM (MASTER)  ////////////////////////////////

reg S_AXIS_VALID = 0;
wire S_AXIS_READY;
reg [31:0] S_AXIS_DATA = 0;

///////////////////////////  AXI_STREAM (SLAVE)  ////////////////////////////////

wire M_AXIS_VALID;
reg M_AXIS_READY = 1;
wire [31:0] M_AXIS_DATA;

///////////////////////////////  AES_STATUS  ////////////////////////////////////


wire AES_DONE;
wire STATUS_0;
wire STATUS_1;


present_v1_0 dut (
 .s00_axi_aclk(CLK),
   .s00_axi_aresetn(RST),
       /* WRITE BUS */
   .s00_axi_awaddr(M_AW_ADDR),
//    .s00_axi_awprot(),
   .s00_axi_awvalid(M_AW_VALID),
   .s00_axi_awready(M_AW_READY),
   .s00_axi_wdata(M_W_DATA),
   .s00_axi_wstrb(M_W_STRB),
   .s00_axi_wvalid(M_W_VALID),
   .s00_axi_wready(M_W_READY),
//    .s00_axi_bresp(),
   .s00_axi_bvalid(),
//    .s00_axi_bready(),
   .s00_axi_araddr(M_AR_ADDR),
//    .s00_axi_arprot(),
   .s00_axi_arvalid(M_AR_VALID),
   .s00_axi_arready(M_AR_READY),
       /* READ BUS */
//    .s00_axi_rdata(),
//    .s00_axi_rresp(),
//    .s00_axi_rvalid(),
//    .s00_axi_rready(),

// Ports of Axi Slave Bus Interface S00_AXIS
.s00_axis_aclk(CLK),
.s00_axis_aresetn(RST),
.s00_axis_tready(S_AXIS_READY),
.s00_axis_tdata(S_AXIS_DATA),
//input wire [(C_S00_AXIS_TDATA_WIDTH/8)-1 : 0] s00_axis_tstrb,
//.s00_axis_tlast,
.s00_axis_tvalid(S_AXIS_VALID),

// Ports of Axi Master Bus Interface M00_AXIS
.m00_axis_aclk(CLK),
.m00_axis_aresetn(RST),
.m00_axis_tvalid(M_AXIS_VALID),
.m00_axis_tdata(M_AXIS_DATA),
//output wire [(C_M00_AXIS_TDATA_WIDTH/8)-1 : 0] m00_axis_tstrb,
//output wire  m00_axis_tlast,
.m00_axis_tready(M_AXIS_READY)

);

task axi_write;
    input [3:0] i_add;
    input [31:0] i_data; 
    begin
      // demonstrates driving external Global Reg
      M_AW_VALID    = 1'b1;
      M_AW_ADDR     = i_add;
      M_W_VALID    = 1'b1;
      M_W_DATA = i_data;

      #20
      if(M_AW_READY) begin
        #20 M_AW_VALID    = 1'b0;
        M_W_VALID    = 1'b0;
      end
    end
 endtask
 
task axis_write;
     input [31:0] i_data; 
     begin
       // demonstrates driving external Global Reg
       S_AXIS_VALID = 1'b1;
       if(S_AXIS_READY) begin
          S_AXIS_DATA = i_data;
       end 
       #10;
     end
  endtask

parameter PERIOD = 10;

always begin
  CLK = 1'b0;
  #(PERIOD/2) CLK = 1'b1;
  #(PERIOD/2);
end

initial begin
    RST = 0;
    #20 RST = 1;
    #50 axi_write(0,3);
    #20 axis_write(100);
    #1000 $finish;
end


endmodule
