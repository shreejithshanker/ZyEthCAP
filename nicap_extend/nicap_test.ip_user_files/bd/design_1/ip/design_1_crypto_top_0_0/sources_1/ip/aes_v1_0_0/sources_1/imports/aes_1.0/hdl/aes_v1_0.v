
`timescale 1 ns / 1 ps

	module aes_v1_0 #
	(
		// Users to add parameters here
		parameter integer C_CRYPTO_AXIS_TDATA_WIDTH	= 128,

		// User parameters ends
		// Do not modify the parameters beyond this line


		// Parameters of Axi Slave Bus Interface S00_AXI
		parameter integer C_S00_AXI_DATA_WIDTH	= 32,
		parameter integer C_S00_AXI_ADDR_WIDTH	= 4,

		// Parameters of Axi Slave Bus Interface S00_AXIS
		parameter integer C_S00_AXIS_TDATA_WIDTH	= 32,

		// Parameters of Axi Master Bus Interface M00_AXIS
		parameter integer C_M00_AXIS_TDATA_WIDTH	= 32,
		parameter integer C_M00_AXIS_START_COUNT	= 32
	)
	(
		// Users to add ports here
		output wire aes_done,
        output wire status_0,
        output reg status_1,
		
		// User ports ends
		// Do not modify the ports beyond this line


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
		output reg  m00_axis_tlast = 0,
		input wire  m00_axis_tready
	);
	
	wire [C_M00_AXIS_TDATA_WIDTH-1:0] reg0;
    wire [1:0] axi_mode;
    assign axi_mode = reg0 [1:0];
    wire [C_M00_AXIS_TDATA_WIDTH-1:0] reg1;
    wire axi_start;
    assign axi_start = reg1 [0:0];
	
// Instantiation of Axi Bus Interface S00_AXI
	aes_v1_0_S00_AXI # ( 
		.C_S_AXI_DATA_WIDTH(C_S00_AXI_DATA_WIDTH),
		.C_S_AXI_ADDR_WIDTH(C_S00_AXI_ADDR_WIDTH)
	) aes_v1_0_S00_AXI_inst (
	    .reg0(reg0),
	    .reg1(reg1),
		.S_AXI_ACLK(s00_axi_aclk),
		.S_AXI_ARESETN(s00_axi_aresetn),
		.S_AXI_AWADDR(s00_axi_awaddr),
		.S_AXI_AWPROT(s00_axi_awprot),
		.S_AXI_AWVALID(s00_axi_awvalid),
		.S_AXI_AWREADY(s00_axi_awready),
		.S_AXI_WDATA(s00_axi_wdata),
		.S_AXI_WSTRB(s00_axi_wstrb),
		.S_AXI_WVALID(s00_axi_wvalid),
		.S_AXI_WREADY(s00_axi_wready),
		.S_AXI_BRESP(s00_axi_bresp),
		.S_AXI_BVALID(s00_axi_bvalid),
		.S_AXI_BREADY(s00_axi_bready),
		.S_AXI_ARADDR(s00_axi_araddr),
		.S_AXI_ARPROT(s00_axi_arprot),
		.S_AXI_ARVALID(s00_axi_arvalid),
		.S_AXI_ARREADY(s00_axi_arready),
		.S_AXI_RDATA(s00_axi_rdata),
		.S_AXI_RRESP(s00_axi_rresp),
		.S_AXI_RVALID(s00_axi_rvalid),
		.S_AXI_RREADY(s00_axi_rready)
	);

	// Add user logic here	

    // Data In
    
    wire m_crypto_axis_tvalid;
    reg m_crypto_axis_tready = 1;
    wire [C_CRYPTO_AXIS_TDATA_WIDTH-1:0] m_crypto_axis_tdata;
    
    axis_dwidth_converter_1 #(
//        .C_S_AXIS_TDATA_WIDTH(C_S00_AXIS_TDATA_WIDTH),
//        .C_M_AXIS_TDATA_WIDTH(C_CRYPTO_AXIS_TDATA_WIDTH)
//        .C_AXIS_TID_WIDTH(1),
//        .C_AXIS_TDEST_WIDTH(1),
//        .C_S_AXIS_TUSER_WIDTH(1),
//        .C_M_AXIS_TUSER_WIDTH(1),
//        .C_AXIS_SIGNAL_SET('B00000000000000000000000000000011)
      ) din (
        .aclk(s00_axis_aclk),
        .aresetn(s00_axis_aresetn),
//        .aclken(1'H1),
        .s_axis_tvalid(s00_axis_tvalid),
        .s_axis_tready(s00_axis_tready),
        .s_axis_tdata(s00_axis_tdata),
//        .s_axis_tstrb(4'HF),
//        .s_axis_tkeep(4'HF),
//        .s_axis_tlast(1'H1),
//        .s_axis_tid(1'H0),
//        .s_axis_tdest(1'H0),
//        .s_axis_tuser(1'H0),
        .m_axis_tvalid(m_crypto_axis_tvalid),
        .m_axis_tready(m_crypto_axis_tready),
        .m_axis_tdata(m_crypto_axis_tdata)
      );
      
     // Data Out
     
     wire s_crypto_axis_tvalid;
     wire s_crypto_axis_tready;
     wire [C_CRYPTO_AXIS_TDATA_WIDTH-1:0] s_crypto_axis_tdata;

          axis_dwidth_converter_0 #(
//          .C_S_AXIS_TDATA_WIDTH(C_CRYPTO_AXIS_TDATA_WIDTH),
//          .C_M_AXIS_TDATA_WIDTH(C_M00_AXIS_TDATA_WIDTH)
//          .C_AXIS_TID_WIDTH(1),
//          .C_AXIS_TDEST_WIDTH(1),
//          .C_S_AXIS_TUSER_WIDTH(1),
//          .C_M_AXIS_TUSER_WIDTH(1),
//          .C_AXIS_SIGNAL_SET('B00000000000000000000000000000011)
        ) dout (
          .aclk(m00_axis_aclk),
          .aresetn(m00_axis_aresetn),
//          .aclken(1'H1),
          .s_axis_tvalid(s_crypto_axis_tvalid),
          .s_axis_tready(s_crypto_axis_tready),
          .s_axis_tdata(s_crypto_axis_tdata),
//          .s_axis_tstrb(4'HF),
//          .s_axis_tkeep(4'HF),
//          .s_axis_tlast(1'H1),
//          .s_axis_tid(1'H0),
//          .s_axis_tdest(1'H0),
//          .s_axis_tuser(1'H0),
          .m_axis_tvalid(m00_axis_tvalid),
          .m_axis_tready(m00_axis_tready),
          .m_axis_tdata(m00_axis_tdata)
        );
    
    
    	aes_wrapper aes (
           .clk(s00_axis_aclk),
           .reset(s00_axis_aresetn),
           .mode(axi_mode),
           .din(m_crypto_axis_tdata),
           .dout(s_crypto_axis_tdata),
           .aes_done(aes_done),
           .aes_start(axi_start)
        );
        
        assign status_0 = aes_done;
//        assign s00_axis_tready = aes_done;

        always @ (posedge m00_axis_aclk) begin
        
            if(axi_mode == 0'b00) begin
                status_1 <= 1;
            end
            else begin
                status_1 <= 0;
            end
        end

        
	// User logic ends

	endmodule
