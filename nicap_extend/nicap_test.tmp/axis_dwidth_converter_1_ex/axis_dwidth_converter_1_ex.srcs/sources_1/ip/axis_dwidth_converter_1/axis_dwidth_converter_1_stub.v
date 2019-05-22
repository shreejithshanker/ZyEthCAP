// Copyright 1986-2017 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2017.4 (lin64) Build 2086221 Fri Dec 15 20:54:30 MST 2017
// Date        : Tue May 21 10:51:42 2019
// Host        : alex-pc running 64-bit Ubuntu 18.10
// Command     : write_verilog -force -mode synth_stub
//               /home/alex/GitHub/zynq-ip-cores/aes_1.0/src/axis_dwidth_converter_1_1/axis_dwidth_converter_1_stub.v
// Design      : axis_dwidth_converter_1
// Purpose     : Stub declaration of top-level module interface
// Device      : xc7z020clg400-1
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* X_CORE_INFO = "axis_dwidth_converter_v1_1_14_axis_dwidth_converter,Vivado 2017.4" *)
module axis_dwidth_converter_1(aclk, aresetn, s_axis_tvalid, s_axis_tready, 
  s_axis_tdata, m_axis_tvalid, m_axis_tready, m_axis_tdata)
/* synthesis syn_black_box black_box_pad_pin="aclk,aresetn,s_axis_tvalid,s_axis_tready,s_axis_tdata[31:0],m_axis_tvalid,m_axis_tready,m_axis_tdata[127:0]" */;
  input aclk;
  input aresetn;
  input s_axis_tvalid;
  output s_axis_tready;
  input [31:0]s_axis_tdata;
  output m_axis_tvalid;
  input m_axis_tready;
  output [127:0]m_axis_tdata;
endmodule
