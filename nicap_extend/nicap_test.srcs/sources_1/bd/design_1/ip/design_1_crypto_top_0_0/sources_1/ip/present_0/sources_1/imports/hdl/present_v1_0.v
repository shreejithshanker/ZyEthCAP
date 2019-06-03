
`timescale 1 ns / 1 ps

	module present_v1_0 #
	(
		// Users to add parameters here

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
		output wire  m00_axis_tlast,
		input wire  m00_axis_tready
	);
	
    wire [1:0] sel;
    reg enc_start, dec_start;
    reg [63:0] enc_in;
    wire [63:0] enc_out;
    wire enc_ready;
    reg [63:0] dec_in;
    wire [63:0] dec_out;
    wire dec_ready;
    wire enc_hdr_start;
    wire dec_hdr_start; 
    reg [63:0] out;
    wire [63:0] in;
    wire [31:0] key_0, key_1, key_2;
    wire [79:0] key;
    
    assign key = {key_0, key_1, key_2};
    
    
// Instantiation of Axi Bus Interface S00_AXI
	present_v1_0_S00_AXI # ( 
		.C_S_AXI_DATA_WIDTH(C_S00_AXI_DATA_WIDTH),
		.C_S_AXI_ADDR_WIDTH(C_S00_AXI_ADDR_WIDTH)
	) present_v1_0_S00_AXI_inst (
	    .mode(sel),
	    .key_0(key_0),
	    .key_1(key_1),
	    .key_2(key_2),
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
	
	wire in_aclk, in_aresetn;
    wire in_s_axis_tvalid;
    assign in_s_axis_tvalid = s00_axis_tvalid;
    wire in_s_axis_tready;
    assign s00_axis_tready = in_s_axis_tready;
    wire [31:0] in_s_axis_tdata;
    wire in_m_axis_tvalid;
    wire in_m_axis_tready;
    wire [63:0] in_m_axis_tdata;
    assign in = in_m_axis_tdata;
    
    wire out_aclk, out_aresetn;
    wire out_s_axis_tvalid;
    wire out_s_axis_tready;
    wire [63:0] out_s_axis_tdata;
    wire out_m_axis_tvalid;
    assign out_m_axis_tvalid = m00_axis_tvalid;
    wire out_m_axis_tready;
    wire [31:0] out_m_axis_tdata;
    
    reg generate_key = 0;
    wire gen_key;
	
    PRESENT present (
            //encoder path
        .enc_plaintext(enc_in),
        .enc_ciphertext(enc_out),
        .enc_start(enc_start), 
        .enc_ready(enc_ready),
        .enc_hdr_start(enc_start),
        .enc_hdr_done(),
    //    encoder_ready,
        
        //decoder path
        .dec_ciphertext(dec_in),
        .dec_plaintext(dec_out),
        .dec_start(dec_start),
        .dec_ready(dec_ready),
        .dec_hdr_start(dec_start),
        .dec_hdr_done(),
        
        // Key
        .key(key),
        .generate_key(generate_key), // generate subkeys
        .gen_key(),
        // Clk,Rst
        .clk_in(s00_axis_aclk), 
        .rst_in(s00_axis_aresetn)
    );
    
    // AXIS IN
    
    assign in_s_axis_tdata = s00_axis_tdata;
    assign in_aclk = s00_axis_aclk;
    assign in_aresetn = s00_axis_aresetn;
    assign s00_axis_tready = in_s_axis_tready;
    
    axis_dwidth_converter_0 input_axis (
      .aclk(in_aclk),                    // input wire aclk
      .aresetn(in_aresetn),              // input wire aresetn
      .s_axis_tvalid(in_s_axis_tvalid),  // input wire s_axis_tvalid
      .s_axis_tready(in_s_axis_tready),  // output wire s_axis_tready
      .s_axis_tdata(in_s_axis_tdata),    // input wire [31 : 0] s_axis_tdata
      .m_axis_tvalid(in_m_axis_tvalid),  // output wire m_axis_tvalid
      .m_axis_tready(in_m_axis_tready),  // input wire m_axis_tready
      .m_axis_tdata(in_m_axis_tdata)    // output wire [63 : 0] m_axis_tdata
    );
    
    // AXIS OUT
    
    assign out_m_axis_tdata = m00_axis_tdata;
    assign out_aclk = m00_axis_aclk;
    assign out_aresetn = m00_axis_aresetn;
    assign m00_axis_tvalid = out_m_axis_tvalid;
    
    axis_dwidth_converter_1 output_axis (
      .aclk(out_aclk),                    // input wire aclk
      .aresetn(out_aresetn),              // input wire aresetn
      .s_axis_tvalid(out_s_axis_tvalid),  // input wire s_axis_tvalid
      .s_axis_tready(out_s_axis_tready),  // output wire s_axis_tready
      .s_axis_tdata(out),    // input wire [31 : 0] s_axis_tdata
      .m_axis_tvalid(out_m_axis_tvalid),  // output wire m_axis_tvalid
      .m_axis_tready(out_m_axis_tready),  // input wire m_axis_tready
      .m_axis_tdata(out_m_axis_tdata)    // output wire [63 : 0] m_axis_tdata
    );
    
    
    // Mode Select
//    assign in_m_axis_tready = sel ? dec_ready : enc_ready;
    
    reg [2:0] key_cnt = 0;
    
    
    always @ (posedge s00_axis_aclk) begin
      case (sel)
      
        0 : begin
                enc_start <= 0;
                dec_start <= 0;
                out <= 0;
                enc_in <= 0;
                dec_in <= 0;
                key_cnt <= 0;
            end
        1 : begin
                enc_start <= 1;
                dec_start <= 0;
                out <= enc_out;
                enc_in <= in;
                dec_in <= 0;
                key_cnt <= 0;
               end
        2 : begin
                enc_start <= 0;
                dec_start <= 1;
                out <= dec_out;
                dec_in <= in;
                enc_in <= 0;
                key_cnt <= 0;
               end
        3 : begin
               enc_start <= 0;
               dec_start <= 0;
               out <= 0;
               dec_in <= 0;
               enc_in <= 0;
               generate_key <= 1;
               if(key_cnt == 2) begin
                  generate_key <= 0;
               end
               else begin
                  key_cnt <= key_cnt + 1;
               end
              end
      endcase
    end


	// Add user logic here

	// User logic ends

	endmodule
