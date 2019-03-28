`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 10.12.2018 16:38:23
// Design Name: 
// Module Name: sys_top
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


module sys_top(

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

    FIXED_IO_ddr_vrn,
    FIXED_IO_ddr_vrp,
    FIXED_IO_mio,
    FIXED_IO_ps_clk,
    FIXED_IO_ps_porb,
    FIXED_IO_ps_srstb,
    leds_0);

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
  inout FIXED_IO_ddr_vrn;
  inout FIXED_IO_ddr_vrp;
  inout [53:0]FIXED_IO_mio;
  inout FIXED_IO_ps_clk;
  inout FIXED_IO_ps_porb;
  inout FIXED_IO_ps_srstb;
      output [3:0]leds_0;

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
   wire useLcl_0;
      
      design_1_wrapper design_1_wrapper
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
                    .useLcl_0(useLcl_0));
            
                    wire                rxif_fifo_tvalid;
                    wire                rxif_fifo_tvalid;
                    wire                rxif_fifo_tready;
                    wire   [31:0]       rxif_fifo_tdata; 
                    wire                rxif_fifo_tlast; 
                    wire     [3:0]      rxif_fifo_tuser;
                    
                    wire                txif_fifo_tvalid;
                    wire                txif_fifo_tready;
                    wire    [31:0]      txif_fifo_tdata;
                    wire                txif_fifo_tlast;
                    wire       [3:0]    txif_fifo_tuser;       
                    
                    wire                slt1_fifo_tvalid;
                    wire                slt1_fifo_tready;
                    wire    [31:0]      slt1_fifo_tdata;
                    wire                slt1_fifo_tlast;
                    wire       [3:0]    slt1_fifo_tuser;       
                    
                    wire                slt2_fifo_tvalid;
                    wire                slt2_fifo_tready;
                    wire    [31:0]      slt2_fifo_tdata;
                    wire                slt2_fifo_tlast;
                    wire       [3:0]    slt2_fifo_tuser;       
                    
                    wire                passThru_fifo_tvalid;
                    wire                passThru_fifo_tready;
                    wire    [31:0]      passThru_fifo_tdata;
                    wire                passThru_fifo_tlast;
                    wire       [3:0]    passThru_fifo_tuser;       
                    
                    wire                slt1_fifo_tvalid_tx;
                    wire                slt1_fifo_tready_tx;
                    wire [31:0]         slt1_fifo_tdata_tx;
                    wire                slt1_fifo_tlast_tx;
                    
                    wire                slt2_fifo_tvalid_tx;
                    wire                slt2_fifo_tready_tx;
                    wire [31:0]         slt2_fifo_tdata_tx;
                    wire                slt2_fifo_tlast_tx;
                    
                    wire                passThru_fifo_tvalid_tx;
                    wire                passThru_fifo_tready_tx;
                    wire [31:0]         passThru_fifo_tdata_tx;
                    wire                passThru_fifo_tlast_tx;
                    
                    
                    wire     [1:0]      rx_arb_fifo_tdata;       
                    wire                rx_arb_fifo_tready;
                    wire                rx_arb_fifo_tvalid;
                    wire     [31:0]     BRAM_PORTA_1_dout1;
                    wire     [31:0]     sha1_read_val;
                    wire                sha1_read_en;
                            
                    eth_bridge eth_bridge 
                            (
                                //from ethernet client bram controller
                                .rx_clk(BRAM_PORTA_clk),
                                .rx_rst_n(~BRAM_PORTA_rst),
                                .rx_address(BRAM_PORTA_addr),
                                .rx_wdata(BRAM_PORTA_din),
                                .rx_write(BRAM_PORTA_we),
                                .rx_read(BRAM_PORTA_en), 
                                .rx_rdata(BRAM_PORTA_dout), 
                                // back to Processing System
                                .rx1_clk(BRAM_PORTA_1_clk),
                                .rx1_rst_n(~BRAM_PORTA_1_rst),
                                .rx1_address(BRAM_PORTA_1_addr),
                                .rx1_wdata(BRAM_PORTA_1_din), 
                                .rx1_write(BRAM_PORTA_1_we), 
                                .rx1_read(BRAM_PORTA_1_en),
                                .rx1_rdata(BRAM_PORTA_1_dout),
                                // TO Rx Arb
                                .rxif_fifo_tvalid(rxif_fifo_tvalid), 
                                .rxif_fifo_tready(rxif_fifo_tready), 
                                .rxif_fifo_tdata(rxif_fifo_tdata), 
                                .rxif_fifo_tlast(rxif_fifo_tlast), 
                                .rxif_fifo_tuser(rxif_fifo_tuser),
                                
                                // From Tx Arb
                                .txif_fifo_tvalid(txif_fifo_tvalid),
                                .txif_fifo_tready(txif_fifo_tready),
                                .txif_fifo_tdata(txif_fifo_tdata),
                                .txif_fifo_tlast(txif_fifo_tlast),
                                .txif_fifo_tuser(txif_fifo_tuser),
                                
                                // Rx arbitration signals
                                .arb_fifo_tvalid(rx_arb_fifo_tvalid), // to arb -- valid
                                .arb_fifo_tready(rx_arb_fifo_tready), // from arb -- ready  
                                .arb_fifo_tdata(rx_arb_fifo_tdata), // to arb -- data
                                
                                // Buffered bitstream
                                 .Lcl_M_AXIS_MM2S_TDATA(Lcl_M_AXIS_MM2S_TDATA_0),
                                 .Lcl_M_AXIS_MM2S_TLAST(Lcl_M_AXIS_MM2S_TLAST_0),
                                 .Lcl_M_AXIS_MM2S_TREADY(Lcl_M_AXIS_MM2S_TREADY_0),
                                 .Lcl_M_AXIS_MM2S_TVALID(Lcl_M_AXIS_MM2S_TVALID_0),
                                 .useLcl(useLcl_0),
                                 
                                 //System Interrupt in case of Configuration Command
                                .interrupt_system(EnIn)        
                            );
                            
                            rxarb rxarb 
                            (
                                .rxif_fifo_tvalid(rxif_fifo_tvalid), 
                                .rxif_fifo_tready(rxif_fifo_tready),
                                .rxif_fifo_tdata(rxif_fifo_tdata),  
                                .rxif_fifo_tlast(rxif_fifo_tlast),  
                                
                                .slt1_fifo_tvalid(slt1_fifo_tvalid),
                                .slt1_fifo_tready(slt1_fifo_tready),
                                .slt1_fifo_tdata(slt1_fifo_tdata),
                                .slt1_fifo_tlast(slt1_fifo_tlast),
                                
                                .slt2_fifo_tvalid(slt2_fifo_tvalid),
                                .slt2_fifo_tready(slt2_fifo_tready),
                                .slt2_fifo_tdata(slt2_fifo_tdata),
                                .slt2_fifo_tlast(slt2_fifo_tlast),
                                
                                .passThru_fifo_tvalid(passThru_fifo_tvalid),
                                .passThru_fifo_tready(passThru_fifo_tready),
                                .passThru_fifo_tdata(passThru_fifo_tdata),
                                .passThru_fifo_tlast(passThru_fifo_tlast),
                                
                                .arb_fifo_tvalid(rx_arb_fifo_tvalid),  
                                .arb_fifo_tready(rx_arb_fifo_tready),  
                                .arb_fifo_tdata(rx_arb_fifo_tdata), 
                                
                                .clk(BRAM_PORTA_1_clk), 
                                .rst(~BRAM_PORTA_1_rst)
                            );
                            
                            
                            txarb txarb
                            (
                    
                                .txif_fifo_tvalid(txif_fifo_tvalid), 
                                .txif_fifo_tready(txif_fifo_tready), 
                                .txif_fifo_tdata(txif_fifo_tdata),
                                .txif_fifo_tlast(txif_fifo_tlast),
                                
                                .slt1_fifo_tvalid(slt1_fifo_tvalid_tx),
                                .slt1_fifo_tready(slt1_fifo_tready_tx),
                                .slt1_fifo_tdata(slt1_fifo_tdata_tx),
                                .slt1_fifo_tlast(slt1_fifo_tlast_tx),
                                
                                .slt2_fifo_tvalid(slt2_fifo_tvalid_tx),
                                .slt2_fifo_tready(slt2_fifo_tready_tx),
                                .slt2_fifo_tdata(slt2_fifo_tdata_tx),
                                .slt2_fifo_tlast(slt2_fifo_tlast_tx),
                                
                                .passThru_fifo_tvalid(passThru_fifo_tvalid_tx),
                                .passThru_fifo_tready(passThru_fifo_tready_tx),
                                .passThru_fifo_tdata(passThru_fifo_tdata_tx),
                                .passThru_fifo_tlast(passThru_fifo_tlast_tx),
                    
                                .clk(BRAM_PORTA_1_clk), 
                                .rst(~BRAM_PORTA_1_rst)
                            );
                            
                            
                         /*   sha1_pr_top_wrap userlogic1
                            (
                            .clk(BRAM_PORTA_1_clk),
                            .rst_n(~BRAM_PORTA_1_rst),
                            .read_val(sha1_read_val),
                            .read_en(sha1_read_en),
                            .enable(1'b1),
                            .done(sha1_done),                
                            .rxif_fifo_tvalid(slt1_fifo_tvalid), 
                            .rxif_fifo_tready(slt1_fifo_tready), 
                            .rxif_fifo_tdata(slt1_fifo_tdata), 
                            .rxif_fifo_tlast(slt1_fifo_tlast), 
                            .rxif_fifo_tuser('d0),
                            .txif_fifo_tvalid(slt1_fifo_tvalid_tx),
                            .txif_fifo_tready(slt1_fifo_tready_tx),
                            .txif_fifo_tdata(slt1_fifo_tdata_tx),
                            .txif_fifo_tlast(slt1_fifo_tlast_tx),
                            .txif_fifo_tuser()
                            ); */
                                    
                              framedecrementer_wrap userlogic1
                                                      (
                                                      .clk(BRAM_PORTA_1_clk),
                                                      .rst_n(~BRAM_PORTA_1_rst),
                                                      .rxif_fifo_tvalid(slt1_fifo_tvalid), 
                                                      .rxif_fifo_tready(slt1_fifo_tready), 
                                                      .rxif_fifo_tdata(slt1_fifo_tdata), 
                                                      .rxif_fifo_tlast(slt1_fifo_tlast), 
                                                      .rxif_fifo_tuser('d0),
                                                      .txif_fifo_tvalid(slt1_fifo_tvalid_tx),
                                                      .txif_fifo_tready(slt1_fifo_tready_tx),
                                                      .txif_fifo_tdata(slt1_fifo_tdata_tx),
                                                      .txif_fifo_tlast(slt1_fifo_tlast_tx),
                                                      .txif_fifo_tuser()
                                                      );
                            
                            frameincrementer_wrap userlogic2
                            (
                            .clk(BRAM_PORTA_1_clk),
                            .rst_n(~BRAM_PORTA_1_rst),
                            .rxif_fifo_tvalid(slt2_fifo_tvalid), 
                            .rxif_fifo_tready(slt2_fifo_tready), 
                            .rxif_fifo_tdata(slt2_fifo_tdata), 
                            .rxif_fifo_tlast(slt2_fifo_tlast), 
                            .rxif_fifo_tuser('d0),
                            .txif_fifo_tvalid(slt2_fifo_tvalid_tx),
                            .txif_fifo_tready(slt2_fifo_tready_tx),
                            .txif_fifo_tdata(slt2_fifo_tdata_tx),
                            .txif_fifo_tlast(slt2_fifo_tlast_tx),
                            .txif_fifo_tuser()
                            );
                            
                                    
                            framePassThru userlogic3
                            (
                            .clk(BRAM_PORTA_1_clk),
                            .rst_n(~BRAM_PORTA_1_rst),
                            .rxif_fifo_tvalid(passThru_fifo_tvalid), 
                            .rxif_fifo_tready(passThru_fifo_tready), 
                            .rxif_fifo_tdata(passThru_fifo_tdata), 
                            .rxif_fifo_tlast(passThru_fifo_tlast), 
                            .rxif_fifo_tuser('d0),
                            .txif_fifo_tvalid(passThru_fifo_tvalid_tx),
                            .txif_fifo_tready(passThru_fifo_tready_tx),
                            .txif_fifo_tdata(passThru_fifo_tdata_tx),
                            .txif_fifo_tlast(passThru_fifo_tlast_tx),
                            .txif_fifo_tuser()
                            );
                            
                            
                     //assign sha1_read_en = (BRAM_PORTA_1_addr[15:12] == 'hA)? 1'b1: 1'b0; 
                     //assign BRAM_PORTA_1_dout = (sha1_read_en)? sha1_read_val : BRAM_PORTA_1_dout1;


endmodule
