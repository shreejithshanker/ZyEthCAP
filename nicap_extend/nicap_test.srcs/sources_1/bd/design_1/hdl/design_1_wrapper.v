//Copyright 1986-2017 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2017.4 (lin64) Build 2086221 Fri Dec 15 20:54:30 MST 2017
//Date        : Wed May 22 14:43:13 2019
//Host        : alex-warc running 64-bit Ubuntu 18.04.2 LTS
//Command     : generate_target design_1_wrapper.bd
//Design      : design_1_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module design_1_wrapper
   (BRAM_PORTA_1_addr,
    BRAM_PORTA_1_clk,
    BRAM_PORTA_1_din,
    BRAM_PORTA_1_dout,
    BRAM_PORTA_1_en,
    BRAM_PORTA_1_rst,
    BRAM_PORTA_1_we,
    BRAM_PORTA_addr,
    BRAM_PORTA_clk,
    BRAM_PORTA_din,
    BRAM_PORTA_dout,
    BRAM_PORTA_en,
    BRAM_PORTA_rst,
    BRAM_PORTA_we,
    DDR_addr,
    DDR_ba,
    DDR_cas_n,
    DDR_ck_n,
    DDR_ck_p,
    DDR_cke,
    DDR_cs_n,
    DDR_dm,
    DDR_dq,
    DDR_dqs_n,
    DDR_dqs_p,
    DDR_odt,
    DDR_ras_n,
    DDR_reset_n,
    DDR_we_n,
    EnIn,
    FIXED_IO_ddr_vrn,
    FIXED_IO_ddr_vrp,
    FIXED_IO_mio,
    FIXED_IO_ps_clk,
    FIXED_IO_ps_porb,
    FIXED_IO_ps_srstb,
    Lcl_M_AXIS_MM2S_TDATA_0,
    Lcl_M_AXIS_MM2S_TLAST_0,
    Lcl_M_AXIS_MM2S_TREADY_0,
    Lcl_M_AXIS_MM2S_TVALID_0,
    leds_0,
    status_0,
    status_1,
    useLcl_0);
  output [15:0]BRAM_PORTA_1_addr;
  output BRAM_PORTA_1_clk;
  output [31:0]BRAM_PORTA_1_din;
  input [31:0]BRAM_PORTA_1_dout;
  output BRAM_PORTA_1_en;
  output BRAM_PORTA_1_rst;
  output [3:0]BRAM_PORTA_1_we;
  output [15:0]BRAM_PORTA_addr;
  output BRAM_PORTA_clk;
  output [31:0]BRAM_PORTA_din;
  input [31:0]BRAM_PORTA_dout;
  output BRAM_PORTA_en;
  output BRAM_PORTA_rst;
  output [3:0]BRAM_PORTA_we;
  inout [14:0]DDR_addr;
  inout [2:0]DDR_ba;
  inout DDR_cas_n;
  inout DDR_ck_n;
  inout DDR_ck_p;
  inout DDR_cke;
  inout DDR_cs_n;
  inout [3:0]DDR_dm;
  inout [31:0]DDR_dq;
  inout [3:0]DDR_dqs_n;
  inout [3:0]DDR_dqs_p;
  inout DDR_odt;
  inout DDR_ras_n;
  inout DDR_reset_n;
  inout DDR_we_n;
  input EnIn;
  inout FIXED_IO_ddr_vrn;
  inout FIXED_IO_ddr_vrp;
  inout [53:0]FIXED_IO_mio;
  inout FIXED_IO_ps_clk;
  inout FIXED_IO_ps_porb;
  inout FIXED_IO_ps_srstb;
  input [31:0]Lcl_M_AXIS_MM2S_TDATA_0;
  input Lcl_M_AXIS_MM2S_TLAST_0;
  output Lcl_M_AXIS_MM2S_TREADY_0;
  input Lcl_M_AXIS_MM2S_TVALID_0;
  output [3:0]leds_0;
  output status_0;
  output status_1;
  input useLcl_0;

  wire [15:0]BRAM_PORTA_1_addr;
  wire BRAM_PORTA_1_clk;
  wire [31:0]BRAM_PORTA_1_din;
  wire [31:0]BRAM_PORTA_1_dout;
  wire BRAM_PORTA_1_en;
  wire BRAM_PORTA_1_rst;
  wire [3:0]BRAM_PORTA_1_we;
  wire [15:0]BRAM_PORTA_addr;
  wire BRAM_PORTA_clk;
  wire [31:0]BRAM_PORTA_din;
  wire [31:0]BRAM_PORTA_dout;
  wire BRAM_PORTA_en;
  wire BRAM_PORTA_rst;
  wire [3:0]BRAM_PORTA_we;
  wire [14:0]DDR_addr;
  wire [2:0]DDR_ba;
  wire DDR_cas_n;
  wire DDR_ck_n;
  wire DDR_ck_p;
  wire DDR_cke;
  wire DDR_cs_n;
  wire [3:0]DDR_dm;
  wire [31:0]DDR_dq;
  wire [3:0]DDR_dqs_n;
  wire [3:0]DDR_dqs_p;
  wire DDR_odt;
  wire DDR_ras_n;
  wire DDR_reset_n;
  wire DDR_we_n;
  wire EnIn;
  wire FIXED_IO_ddr_vrn;
  wire FIXED_IO_ddr_vrp;
  wire [53:0]FIXED_IO_mio;
  wire FIXED_IO_ps_clk;
  wire FIXED_IO_ps_porb;
  wire FIXED_IO_ps_srstb;
  wire [31:0]Lcl_M_AXIS_MM2S_TDATA_0;
  wire Lcl_M_AXIS_MM2S_TLAST_0;
  wire Lcl_M_AXIS_MM2S_TREADY_0;
  wire Lcl_M_AXIS_MM2S_TVALID_0;
  wire [3:0]leds_0;
  wire status_0;
  wire status_1;
  wire useLcl_0;

  design_1 design_1_i
       (.BRAM_PORTA_1_addr(BRAM_PORTA_1_addr),
        .BRAM_PORTA_1_clk(BRAM_PORTA_1_clk),
        .BRAM_PORTA_1_din(BRAM_PORTA_1_din),
        .BRAM_PORTA_1_dout(BRAM_PORTA_1_dout),
        .BRAM_PORTA_1_en(BRAM_PORTA_1_en),
        .BRAM_PORTA_1_rst(BRAM_PORTA_1_rst),
        .BRAM_PORTA_1_we(BRAM_PORTA_1_we),
        .BRAM_PORTA_addr(BRAM_PORTA_addr),
        .BRAM_PORTA_clk(BRAM_PORTA_clk),
        .BRAM_PORTA_din(BRAM_PORTA_din),
        .BRAM_PORTA_dout(BRAM_PORTA_dout),
        .BRAM_PORTA_en(BRAM_PORTA_en),
        .BRAM_PORTA_rst(BRAM_PORTA_rst),
        .BRAM_PORTA_we(BRAM_PORTA_we),
        .DDR_addr(DDR_addr),
        .DDR_ba(DDR_ba),
        .DDR_cas_n(DDR_cas_n),
        .DDR_ck_n(DDR_ck_n),
        .DDR_ck_p(DDR_ck_p),
        .DDR_cke(DDR_cke),
        .DDR_cs_n(DDR_cs_n),
        .DDR_dm(DDR_dm),
        .DDR_dq(DDR_dq),
        .DDR_dqs_n(DDR_dqs_n),
        .DDR_dqs_p(DDR_dqs_p),
        .DDR_odt(DDR_odt),
        .DDR_ras_n(DDR_ras_n),
        .DDR_reset_n(DDR_reset_n),
        .DDR_we_n(DDR_we_n),
        .EnIn(EnIn),
        .FIXED_IO_ddr_vrn(FIXED_IO_ddr_vrn),
        .FIXED_IO_ddr_vrp(FIXED_IO_ddr_vrp),
        .FIXED_IO_mio(FIXED_IO_mio),
        .FIXED_IO_ps_clk(FIXED_IO_ps_clk),
        .FIXED_IO_ps_porb(FIXED_IO_ps_porb),
        .FIXED_IO_ps_srstb(FIXED_IO_ps_srstb),
        .Lcl_M_AXIS_MM2S_TDATA_0(Lcl_M_AXIS_MM2S_TDATA_0),
        .Lcl_M_AXIS_MM2S_TLAST_0(Lcl_M_AXIS_MM2S_TLAST_0),
        .Lcl_M_AXIS_MM2S_TREADY_0(Lcl_M_AXIS_MM2S_TREADY_0),
        .Lcl_M_AXIS_MM2S_TVALID_0(Lcl_M_AXIS_MM2S_TVALID_0),
        .leds_0(leds_0),
        .status_0(status_0),
        .status_1(status_1),
        .useLcl_0(useLcl_0));
endmodule
