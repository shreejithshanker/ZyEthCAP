//  (c) Copyright  2013 - 2019 Xilinx, Inc. All rights reserved.
//
//  This file contains confidential and proprietary information
//  of Xilinx, Inc. and is protected under U.S. and
//  international copyright and other intellectual property
//  laws.
//
//  DISCLAIMER
//  This disclaimer is not a license and does not grant any
//  rights to the materials distributed herewith. Except as
//  otherwise provided in a valid license issued to you by
//  Xilinx, and to the maximum extent permitted by applicable
//  law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
//  WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
//  AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
//  BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
//  INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
//  (2) Xilinx shall not be liable (whether in contract or tort,
//  including negligence, or under any other theory of
//  liability) for any loss or damage of any kind or nature
//  related to, arising under or in connection with these
//  materials, including for any direct, or any indirect,
//  special, incidental, or consequential loss or damage
//  (including loss of data, profits, goodwill, or any type of
//  loss or damage suffered as a result of any action brought
//  by a third party) even if such damage or loss was
//  reasonably foreseeable or Xilinx had been advised of the
//  possibility of the same.
//
//  CRITICAL APPLICATIONS
//  Xilinx products are not designed or intended to be fail-
//  safe, or for use in any application requiring fail-safe
//  performance, such as life-support or safety devices or
//  systems, Class III medical devices, nuclear facilities,
//  applications related to the deployment of airbags, or any
//  other applications that could lead to death, personal
//  injury, or severe property or environmental damage
//  (individually and collectively, "Critical
//  Applications"). Customer assumes the sole risk and
//  liability of any use of Xilinx products in Critical
//  Applications, subject only to applicable laws and
//  regulations governing limitations on product liability.
//
//  THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
//  PART OF THIS FILE AT ALL TIMES.
//-----------------------------------------------------------------------------

`default_nettype none
`timescale 1ps/1ps

module exdes_tb (
);
// Local parameters
// timescale is 1ps/1ps
localparam  ONE_NS      = 1000;
localparam time PER = 5.0*ONE_NS; // 5 ns clock
localparam integer LP_TIMEOUT = 100; // 100 = 10ms

// Reg  delcarations
reg clk = 1;
reg sys_rst = 0;
integer heartbeat_cnt = 0;
reg [15*8-1:0] test_phase = "";

// Wire declarations
wire done;
wire locked;
wire clk_p;
wire clk_n;


always begin
clk = #(PER/2) ~clk;
end
assign clk_p = clk;
assign clk_n = ~clk;


initial begin
// Set up any display statements using time to be readable
test_phase = "not_started";
$timeformat(-9, 2, "ns", 10);
sys_rst = 0;
$display ("%t: %m: Waiting for clock to lock", $time);
@(posedge locked)
$display ("%t: %m: Starting testbench", $time);
#(PER*16);
sys_rst = 1;
$display ("%t: %m: Asserting reset for 16 cycles", $time);
test_phase = "reset";
#(PER*16)
sys_rst = 0;
$display ("%t: %m: Reset complete", $time);
#(PER*16)
test_phase = "running";
@(posedge done)
test_phase = "success";
#(PER*2);
$display("%t: %m: SIMULATION PASSED", $time);
$display("%t: %m: Test Completed Successfully", $time);
$finish;
end



initial begin
forever begin
#(100000000) $display("%t: %m: Heartbeat",$time);
heartbeat_cnt = heartbeat_cnt + 1;
if (heartbeat_cnt >= LP_TIMEOUT) begin
test_phase = "failure";
$display("%t: %m: SIMULATION TIMEOUT FAILURE", $time);
$finish;
end

end
end

// Module instantiations
//  $product top
exdes_top exdes_top (
  .clk_p ( clk_p ),
  .clk_n ( clk_n ),
  .sys_rst ( sys_rst ),
  .done ( done ),
  .locked ( locked )
);

endmodule // exdes_tb
`default_nettype wire
