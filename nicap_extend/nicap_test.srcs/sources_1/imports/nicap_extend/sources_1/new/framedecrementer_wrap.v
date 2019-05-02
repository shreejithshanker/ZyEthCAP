`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 20.12.2018 13:21:01
// Design Name: 
// Module Name: framedecrementer_wrap
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


module framedecrementer_wrap
(
input            clk,
input            rst_n,
input            rxif_fifo_tvalid, // to rxunpacker -- valid
output    		 rxif_fifo_tready, // from rxunpacker -- ready
input [31:0]	 rxif_fifo_tdata, // to rxunpacker -- data
input			 rxif_fifo_tlast, // to rxunpacker -- last
input  [3:0]     rxif_fifo_tuser,
output           txif_fifo_tvalid,
input            txif_fifo_tready,
output    [31:0] txif_fifo_tdata,
output  		 txif_fifo_tlast,
output    [3:0]  txif_fifo_tuser
);

framedecrementer ul1
                (
                .clk(clk),
                .rst_n(rst_n),
                .rxif_fifo_tvalid(rxif_fifo_tvalid), 
                .rxif_fifo_tready(rxif_fifo_tready), 
                .rxif_fifo_tdata(rxif_fifo_tdata), 
                .rxif_fifo_tlast(rxif_fifo_tlast), 
                .rxif_fifo_tuser(rxif_fifo_tuser),
                .txif_fifo_tvalid(txif_fifo_tvalid),
                .txif_fifo_tready(txif_fifo_tready),
                .txif_fifo_tdata(txif_fifo_tdata),
                .txif_fifo_tlast(txif_fifo_tlast),
                .txif_fifo_tuser(txif_fifo_tuser)
                );
endmodule
