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

module exdes_top (
  input wire clk_p,
  input wire clk_n,
  input wire sys_rst,
  output wire done,
  output wire locked
);
// Register declarations
(* MARK_DEBUG *) reg [1:0] done_r = 2'b0;
// Wire declarations
wire aclk;
(* MARK_DEBUG *) wire locked_i;
(* MARK_DEBUG *) wire aresetn;
(* MARK_DEBUG *) wire s_axis_tready;
(* MARK_DEBUG *) wire s_axis_tvalid;
(* MARK_DEBUG *) wire m_axis_tvalid;
(* MARK_DEBUG *) wire m_axis_tready;
(* MARK_DEBUG *) wire [1-1:0]example_slave_idle;
(* MARK_DEBUG *) wire [32-1:0]s_axis_tdata;
(* MARK_DEBUG *) wire [1-1:0]example_master_done;
(* MARK_DEBUG *) wire [128-1:0]m_axis_tdata;

// Module instantiations
//  Example Clock Wizard 
clk_wiz_0 inst_clk_wiz (
  .clk_in1_p ( clk_p ),
  .clk_in1_n ( clk_n ),
  .clk_out1 ( aclk ),
  .locked ( locked_i )
);

// Example Reset syncrhonizer si
proc_sys_reset_0 inst_proc_sys_rst_0 (
  .slowest_sync_clk ( aclk ),
  .ext_reset_in ( sys_rst ),
  .aux_reset_in ( 1'b0 ),
  .mb_debug_sys_rst ( 1'b0 ),
  .dcm_locked ( locked ),
  .mb_reset (  ),
  .bus_struct_reset (  ),
  .peripheral_reset (  ),
  .interconnect_aresetn (  ),
  .peripheral_aresetn ( aresetn )
);

// Example master #0
axis_dwidth_converter_1_example_master #(
  .C_MASTER_ID ( 0 )
) inst_axis_dwidth_converter_1_example_master_0 (
  .aclk ( aclk ),
  .aresetn ( aresetn ),
  .m_axis_tvalid ( s_axis_tvalid ),
  .m_axis_tdata ( s_axis_tdata[0+:32] ),
  .m_axis_tready ( s_axis_tready ),
  .done ( example_master_done[0+:1] )
);

// DUT: axis_dwidth_converter_1
axis_dwidth_converter_1 dut (
  .aclk ( aclk ),
  .aresetn ( aresetn ),
  .s_axis_tready ( s_axis_tready ),
  .m_axis_tready ( m_axis_tready ),
  .s_axis_tvalid ( s_axis_tvalid ),
  .s_axis_tdata ( s_axis_tdata[0+:32] ),
  .m_axis_tvalid ( m_axis_tvalid ),
  .m_axis_tdata ( m_axis_tdata[0+:128] )
);

// Example slave #0
axis_dwidth_converter_1_example_slave inst_axis_dwidth_converter_1_example_slave_0 (
  .aclk ( aclk ),
  .aresetn ( aresetn ),
  .s_axis_tvalid ( m_axis_tvalid ),
  .s_axis_tdata ( m_axis_tdata[0+:128] ),
  .s_axis_tready ( m_axis_tready ),
  .idle ( example_slave_idle[0+:1] )
);

// Assign output
always @(posedge aclk) begin
  if (~aresetn) begin
    done_r <= 2'b0;
  end else begin
    done_r[0] <= (&example_master_done) & (&example_slave_idle);
    done_r[1] <= done_r[0];
  end
end
assign done = done_r[1];
assign locked = locked_i;


endmodule // exdes_top
`default_nettype wire
