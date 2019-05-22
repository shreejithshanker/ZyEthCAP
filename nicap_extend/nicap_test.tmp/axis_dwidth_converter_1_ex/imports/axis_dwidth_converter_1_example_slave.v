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

`timescale 1ps/1ps

`default_nettype none

module
axis_dwidth_converter_1_example_slave
(
  /**************** Stream Signals ****************/
  input  wire                            s_axis_tvalid,
  output reg                             s_axis_tready,
  input  wire [128-1:0]     s_axis_tdata,
  /**************** System Signals ****************/
  input  wire                            aclk,
  input  wire                            aresetn,
  /**************** Idle Signal ****************/
  output reg                             idle = 1'b0
);

  /**************** Local Parameters ****************/
  localparam [8-1:0] P_S_AXIS_IDLE_NUM = 8'h64;

  /**************** Internal Wires/Regs ****************/
  reg [8-1:0]    icnt_i = {8{1'b0}};
  reg                          acco_i = 1'b0;
  reg                          accen_i = 1'b0;
  wire                         acci_i;
  reg [128-1:0]   tdata_i = {128{1'b0}};
  wire                         areset = ~aresetn;

  //**********************************************
  // TREADY
  //**********************************************
  always @(posedge aclk) begin
    if(areset) begin
      s_axis_tready <= 1'b0;
    end
    else
    begin
      // Toggle TREADY to demonstrate slave backpressure
      s_axis_tready <= ~s_axis_tready;
    end
  end

  //**********************************************
  // PROCESS INPUTS
  //**********************************************
  always @(posedge aclk) begin
    if(areset) begin
      acco_i <= 1'b0;
      accen_i <= 1'b0;
    end
    else
    begin
      accen_i <= (s_axis_tready && s_axis_tvalid) ? 1'b1 : 1'b0;

      tdata_i <= s_axis_tdata;
      acco_i <= accen_i ? (acco_i | acci_i) : acco_i;
    end
  end

assign acci_i =
                (|tdata_i) |
                1'b0;

  //**********************************************
  // IDLE
  //**********************************************
  always @(posedge aclk) begin
    if(areset) begin
      idle <= 1'b0;
      icnt_i <= {8{1'b0}};
    end
    else
    begin
      // Increment counters
      if(s_axis_tvalid) begin
        icnt_i <= {8{1'b0}};
      end
      else if(~s_axis_tvalid && (icnt_i < P_S_AXIS_IDLE_NUM)) begin
        icnt_i <= icnt_i + 1'b1;
      end
      else begin
        icnt_i <= icnt_i;
      end

      // Assert idle
      if(icnt_i == (P_S_AXIS_IDLE_NUM -1)) begin
        idle <= acco_i;
      end
      else if(icnt_i == P_S_AXIS_IDLE_NUM) begin
        idle <= 1'b1;
      end
      else begin
        idle <= 1'b0;
      end
    end
  end
endmodule

`default_nettype wire

