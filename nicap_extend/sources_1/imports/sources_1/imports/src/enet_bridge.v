module eth_bridge #(parameter unpack_dst_addr = 48'hAABBCCDDEEFF)(
//from ethernet client bram controller
input    	      rx_clk,
input    	      rx_rst_n,
input  [15:0]  	  rx_address,
input  [31:0]     rx_wdata,
input   [3:0]     rx_write,
input             rx_read, // rx_enable
output reg [31:0]     rx_rdata, // unconnected
// back to Processing System
input    	      rx1_clk,
input    	      rx1_rst_n,
input  [15:0]  	  rx1_address,
input  [31:0]     rx1_wdata, // unconnected
input   [3:0]     rx1_write, // unconnected
input             rx1_read, // rx_enable
output reg [31:0] rx1_rdata,


output            rxif_fifo_tvalid, // to rxunpacker -- valid
input			  rxif_fifo_tready, // from rxunpacker -- ready
output [31:0]	  rxif_fifo_tdata, // to rxunpacker -- data
output			  rxif_fifo_tlast, // to rxunpacker -- last
output  [3:0]     rxif_fifo_tuser,
input             txif_fifo_tvalid,
output            txif_fifo_tready,
input [31:0]	  txif_fifo_tdata,
input			  txif_fifo_tlast,
input	[3:0]     txif_fifo_tuser,

output              arb_fifo_tvalid, // to arb -- valid
input                arb_fifo_tready, // from arb -- ready
output [1:0]     arb_fifo_tdata, // to arb -- data

// Buffered bitstream from FIFO
output [31:0]Lcl_M_AXIS_MM2S_TDATA,
output Lcl_M_AXIS_MM2S_TLAST,
input Lcl_M_AXIS_MM2S_TREADY,
output Lcl_M_AXIS_MM2S_TVALID,
output reg useLcl,


output	reg		  interrupt_system
);


reg [15:0] pkt1_len_cntr,pkt2_len_cntr,pkt1_len,pkt2_len;
reg [47:0] dest_addr, src_addr;
reg [0:0] state;
reg [3:0] config_state;
reg no_filter;
reg fifo1_wr;
reg fifo2_wr, fifo2_rd;
reg fifo1_last, fifo2_last;
reg [31:0] fifo_wrdata;
wire [31:0] fifo1_rddata, fifo2_rddata;
wire [12:0] fifo1_datacount, fifo2_datacount;
reg len_to_check;
reg [95:0] filter_dest_src_id;
reg [1:0] arb_fifo_wrdata;
reg          arb_fifo_wr;
wire   [7:0] arb_fifo_td_l;
assign arb_fifo_tdata = arb_fifo_td_l[1:0];
reg [15:0] rx_addr;
reg           rx_rd;
reg           bit_sm_check;
reg   [1:0] bit_sm;
reg [55:0] bit_len_val;

parameter idle           = 4'd0,
		  sniff_pkt_f1   = 4'd1,
		  sniff_pkt_f2   = 4'd2,
		  sniff_pkt_f3   = 4'd3,
		  sniff_pkt_f4   = 4'd4,
		  wait_interrupt_l2 = 4'd5,
		  wait_interrupt_l3 = 4'd6,
		  pkt_wait     = 4'd7,
          pkt_eth_bitstream = 4'd8, 
          pkt_ip_bitstream = 4'd9,
          pkt_ignore = 4'd10;
parameter idle_state    = 1'b0, 
		  stream_pkt_f1 = 1'b1;
parameter CONFIG_PKT_MATCH = 64'hAABBCCDDEEFF0011;
parameter CONFIG_BIT_MATCH = 64'h0011AABBCCDDEEFF;
parameter SLOT1_DEF_DEST = 48'h001122334455;
parameter SLOT1_DEF_SRC  =  48'h112233445566;
parameter SLOT2_DEF_DEST = 48'h123456123456;
parameter SLOT2_DEF_SRC  = 48'h012345012345;

wire pkt_fifo_write = (&rx_write) && rx_read;
reg [31:0] rx_wrdata; 
reg        pkt_fifo_wr;
reg [3:0]  pkt_wr_info;
integer	   byte_index;
reg [95:0] intr_timer;

always @(posedge rx_clk) 
begin
    if(~rx_rst_n)
	begin
		rx_wrdata  <= 'd0;
        rx_addr <= 0;
		pkt_fifo_wr <= 0;
	end
	else begin
		rx_wrdata <= rx_wdata;
        rx_addr    <= rx_address;
        rx_rd        <= rx_read;
		if ((&rx_write) && rx_read) begin // a write operation
			pkt_fifo_wr <= 1;
		end
		else 
			pkt_fifo_wr <= 0;
			
	end
end
		
always @(posedge rx_clk)
begin
    if(~rx_rst_n)
	begin
	    fifo1_wr                <=  'd0;
	    fifo2_wr                <=  'd0;
	    fifo_wrdata                <=  'd0;
		state                   <=  idle_state;
		pkt1_len_cntr           <=  4'd0;
		pkt2_len_cntr           <=  4'd0;
		pkt1_len                <=  'd1024;
		pkt2_len                <=  'd1024;
		len_to_check            <=  'b0;
		fifo1_last <= 1'b0;
		fifo2_last <= 1'b0;		
		dest_addr  <= 'd0;
		src_addr   <= 'd0;
	end
	else begin
		fifo1_wr <= 1'b0;
		fifo2_wr <= 1'b0;
		fifo1_last <= 1'b0;
		fifo2_last <= 1'b0;
		
	    case(state)
	        idle_state:begin
				if(pkt_fifo_wr && (rx_addr[11:2] == 'd0)) //start of write at address x000
		        begin					
					len_to_check <= 1'b1;
					dest_addr[47:16] <= {rx_wrdata[7:0],rx_wrdata[15:8],rx_wrdata[23:16],rx_wrdata[31:24]};
					fifo_wrdata <= {rx_wrdata[7:0],rx_wrdata[15:8],rx_wrdata[23:16],rx_wrdata[31:24]};
					fifo1_wr <= 1'b1;
					state <= stream_pkt_f1;
					pkt1_len_cntr <= 'd4;  //count the size of the pkt.						
		        end
            end
			stream_pkt_f1:begin
				if (pkt_fifo_wr) begin
					pkt1_len_cntr <= pkt1_len_cntr + 'd4;
					fifo1_wr <= 1'b1;
					fifo_wrdata <= {rx_wrdata[7:0],rx_wrdata[15:8],rx_wrdata[23:16],rx_wrdata[31:24]};
					if (pkt1_len_cntr == 'd4) begin
						dest_addr [15:0] <= {rx_wrdata[7:0],rx_wrdata[15:8]};
						src_addr [47:32] <= {rx_wrdata[23:16],rx_wrdata[31:24]};
					end
					else if (pkt1_len_cntr == 'd8) begin
						src_addr [31:0]  <= {rx_wrdata[7:0],rx_wrdata[15:8],rx_wrdata[23:16],rx_wrdata[31:24]};
					end
					else if (pkt1_len_cntr == 'd12) begin
						if ({rx_wrdata[7:0], rx_wrdata[15:8]} >= 'd1500) // check for ethertype
							len_to_check <= 'b1;
						else begin
							len_to_check <= 'b0;
							pkt1_len         <= {rx_wrdata[7:0], rx_wrdata[15:8]};
						end
					end
					else if ((pkt1_len_cntr == 'd16) && len_to_check) begin // 12-13 ether type 0800, 14/15 Identifiers, 16-17 length
					    pkt1_len         <= {rx_wrdata[7:0], rx_wrdata[15:8]}; 
						len_to_check     <= 'b0;
					end
			    
					if(pkt1_len_cntr > 'd16)
					begin
						if (pkt1_len_cntr >= pkt1_len + 'd8) // last write
						begin
							state <= idle_state;
							fifo1_last <= 1'b1;
						end
					end	
				end	
			end
			
	    endcase
	end
end
reg [15:0] rx1_addr_d1;
wire fifo1_read = rx1_read && ~(|rx1_write);
reg rx1_read_d1;
always @ (posedge rx1_clk)
begin
    if(~rx1_rst_n)
	begin
	    fifo2_rd                <=    'd0;
		rx1_addr_d1				<=    'd0;
        rx1_read_d1              <= 'd0;
	end
	else begin
		fifo2_rd <= 1'b0;
        rx1_read_d1 <= rx1_read;
		rx1_addr_d1 <= rx1_address;
		if(fifo1_read && (rx1_address != rx1_addr_d1)) //PIO reads can be tricky, so check address before reading
		begin
			if (rx1_address[15:12] == 'd6) begin
				fifo2_rd <= 1'b1; // Interface 1 is read only
				//rx1_rdata <= fifo2_rddata;
			end
		end
	end
end
// configuration matching
reg [63:0] config_match_reg, config_shiftReg, config_frameData;
reg        interrupt_ack;
reg [31:0] config_rddata;
reg [95:0] slot1_config_data, slot2_config_data;
always @ (posedge rx1_clk) 
begin
	if(~rx1_rst_n) begin
		config_match_reg <= CONFIG_PKT_MATCH;
		interrupt_ack    <= 1'b0;
		config_rddata    <= 'd0;
        slot1_config_data <= {SLOT1_DEF_DEST,SLOT1_DEF_SRC};
        slot2_config_data <= {SLOT2_DEF_DEST,SLOT2_DEF_SRC};
	end
	else begin
		if (~interrupt_system && interrupt_ack) 
			interrupt_ack <= 1'b0;
		if (rx1_read && (rx1_address[15:12] == 'd7)) begin 
			if (|rx1_write) begin
				if (rx1_address[5:3] == 'd0) begin
					if (rx1_address[2]) begin
						if (rx1_write[0]) 
							config_match_reg [39:32] <= rx1_wdata[7:0];
						if (rx1_write[1])
							config_match_reg [47:40] <= rx1_wdata[15:8];
						if (rx1_write[2])
							config_match_reg [55:48] <= rx1_wdata[23:8];
						if (rx1_write[3]) 
							config_match_reg [63:56] <= rx1_wdata[31:24];
					end
					else begin
						if (rx1_write[0]) 
							config_match_reg [7:0] <= rx1_wdata[7:0];
						if (rx1_write[1])
							config_match_reg [15:8] <= rx1_wdata[15:8];
						if (rx1_write[2])
							config_match_reg [23:16] <= rx1_wdata[23:8];
						if (rx1_write[3])
							config_match_reg [31:24] <= rx1_wdata[31:24];
					end
				end
                else begin
                    case (rx1_address[5:2]) 
                        4'd7: slot1_config_data[31:0] <= rx1_wdata;
                        4'd8: slot1_config_data[63:32] <= rx1_wdata;
                        4'd9: slot1_config_data[95:64] <= rx1_wdata;
                        4'd10: slot2_config_data[31:0] <= rx1_wdata;
                        4'd11: slot2_config_data[63:32] <= rx1_wdata;
                        4'd12: slot2_config_data[95:64] <= rx1_wdata;
                    endcase
                end
			end
			else begin
				case (rx1_address[5:2])
					4'd0 : config_rddata <= config_match_reg[31:0];
					4'd1 : config_rddata <= config_match_reg[63:32];
					4'd2 : config_rddata <= config_shiftReg [31:0];
					4'd3 : config_rddata <= config_shiftReg [63:32];
					4'd4 : config_rddata <= config_frameData[31:0];
					4'd5 : config_rddata <= config_frameData[63:32];
					4'd6 : config_rddata <= src_addr[31:0];
					4'd7 : config_rddata <= {16'd0,src_addr[47:32]};
					4'd8 : config_rddata <= dest_addr[31:0];
					4'd9 : config_rddata <= {16'd0,dest_addr[47:32]};	
                    4'd10 : config_rddata <= intr_timer[31:0];
                    4'd11 : config_rddata <= intr_timer[63:32];
                    4'd12 : config_rddata <= intr_timer[95:64];
					default : config_rddata <= 'd0;
				endcase
			end
			if ((rx1_address[4:2] == 3'd4) && interrupt_system)
				interrupt_ack <= 1'b1;
		end
		
	end
end
// Actual PACKET SNIFFING
always @ (posedge rx_clk)
begin
	if (~rx_rst_n) begin
		config_shiftReg  <= 'd0;
		config_frameData <= 'd0;
		config_state <= idle;
		interrupt_system <= 1'b0;
        filter_dest_src_id <= 'd0;
        arb_fifo_wrdata <= 2'd0; //pass through, configuration frames are not fed to the slots
        arb_fifo_wr <= 1'b0;
        bit_sm_check <= 0;
	end
	else begin
        arb_fifo_wr <= 1'b0;
        bit_sm_check <= 0;
        if (interrupt_ack) 
            interrupt_system <= 1'b0;
		case (config_state) 
			idle : begin
				if ((state == stream_pkt_f1) && pkt_fifo_wr && (pkt1_len_cntr == 'd12)) begin
					if ({rx_wrdata[7:0], rx_wrdata[15:8]} >= 'd1500) begin // check for ethertype  
						if ({rx_wrdata[7:0], rx_wrdata[15:8]} != 'h0800)//if it it not an IP packet, then ignore it. 
							config_state <= pkt_ignore;
						else 
							config_state <= sniff_pkt_f3;
					end
					else begin // Layer-2 packet
						config_shiftReg [63:48] <= {rx_wrdata[23:16], rx_wrdata[31:24]};
						config_state <= sniff_pkt_f1;
					end
				end
				//interrupt_system <= 1'b0;
			end
			sniff_pkt_f1 : begin // Layer-2 packet, pkt_len_cntr == 16, 20 and 24 to capture reconfig command, and mode name (64-bits each)
				if ((state == stream_pkt_f1) && pkt_fifo_wr && (pkt1_len_cntr == 'd16)) begin 
					config_shiftReg [47:16] <= {rx_wrdata[7:0], rx_wrdata[15:8], rx_wrdata[23:16],rx_wrdata[31:24]};
					//config_frameData <= {config_frameData[15:0], rx_wrdata[23:16], rx_wrdata[31:24]};
					//config_state <= sniff_pkt_f2;
				end
                else if ((state == stream_pkt_f1) && pkt_fifo_wr && (pkt1_len_cntr == 'd20)) begin 
                	config_shiftReg[15:0] <= {rx_wrdata[7:0], rx_wrdata[15:8]};
					config_frameData[63:48] <= {rx_wrdata[23:16], rx_wrdata[31:24]};
				end
                else if ((state == stream_pkt_f1) && pkt_fifo_wr && (pkt1_len_cntr == 'd24)) begin 
					config_frameData[47:16] <= {rx_wrdata[7:0], rx_wrdata[15:8], rx_wrdata[23:16], rx_wrdata[31:24]};
                    config_state <= sniff_pkt_f2;
				end
			end
			sniff_pkt_f2 : begin // Layer-2 packet, pkt_len_cntr == 28, final 16-bits of the mode name.
				if ((state == stream_pkt_f1) && pkt_fifo_wr && (pkt1_len_cntr == 'd28)) begin 
					config_frameData[15:0] <= {rx_wrdata[7:0], rx_wrdata[15:8]};
					if (config_shiftReg == config_match_reg) begin
						interrupt_system <= 1'b1;
						config_state 	 <= wait_interrupt_l2;
                        filter_dest_src_id[95:80] <= {rx_wrdata[23:16], rx_wrdata[31:24]};
					end
					else if (config_shiftReg == CONFIG_BIT_MATCH) begin
                               bit_sm_check <= 1;
                               config_state <= pkt_eth_bitstream;
                               // the other 16-bits here is padding, so ignore it.
                    end
					else begin
                        config_state     <= pkt_wait; // not config packet, so check if there is a match in slot sts
						config_shiftReg  <= 'd0;
						config_frameData <= 'd0;
					end
				end		
			end
			sniff_pkt_f3: begin // IP Packet, Data starts at 33 onwards
				if ((state == stream_pkt_f1) && pkt_fifo_wr && (pkt1_len_cntr == 'd32)) begin 
					config_shiftReg [63:32] <= {rx_wrdata[7:0],rx_wrdata[15:8],rx_wrdata[23:16],rx_wrdata[31:24]};
					//config_state <= sniff_pkt_f4;
				end
                else if ((state == stream_pkt_f1) && pkt_fifo_wr && (pkt1_len_cntr == 'd36)) begin 
					config_shiftReg[31:0] <= {rx_wrdata[7:0],rx_wrdata[15:8],rx_wrdata[23:16],rx_wrdata[31:24]};
					//config_state <= sniff_pkt_f4;
				end
                else if ((state == stream_pkt_f1) && pkt_fifo_wr && (pkt1_len_cntr == 'd40)) begin 
					config_frameData[63:32] <= {rx_wrdata[7:0],rx_wrdata[15:8],rx_wrdata[23:16],rx_wrdata[31:24]};
					config_state <= sniff_pkt_f4;
				end
			end
			sniff_pkt_f4: begin // IP Packet, Data starts at 33 onwards, final 32-bits of the mode name.
				if ((state == stream_pkt_f1) && pkt_fifo_wr && (pkt1_len_cntr == 'd44)) begin 
					config_frameData[31:0] <= {rx_wrdata[7:0],rx_wrdata[15:8],rx_wrdata[23:16],rx_wrdata[31:24]};
					if (config_shiftReg == config_match_reg) begin
						interrupt_system <= 1'b1;
						config_state 	 <= wait_interrupt_l3;
					end
                    else if (config_shiftReg == CONFIG_BIT_MATCH) begin
                               bit_sm_check <= 1;
                               config_state <= pkt_ip_bitstream;
                               // Assumes that Frame data starts at position 33 (have some issue when using scapy)
                    end
					else begin
                    	config_state     <= pkt_wait; // not config packet, so check if there is a match in slot sts
						config_shiftReg  <= 'd0;
						config_frameData <= 'd0;
					end
				end
			end
			wait_interrupt_l2 : begin
				if (interrupt_ack) 
					interrupt_system <= 1'b0;
                
                if ((state == stream_pkt_f1) && pkt_fifo_wr && (pkt1_len_cntr == 'd32))  
                        filter_dest_src_id[79:48] <= {rx_wrdata[7:0],rx_wrdata[15:8],rx_wrdata[23:16],rx_wrdata[31:24]}; // Dest ID Completed
                else if ((state == stream_pkt_f1) && pkt_fifo_wr && (pkt1_len_cntr == 'd36))  
                        filter_dest_src_id[47:16] <= {rx_wrdata[7:0],rx_wrdata[15:8],rx_wrdata[23:16],rx_wrdata[31:24]}; 
                else if  ((state == stream_pkt_f1) && pkt_fifo_wr && (pkt1_len_cntr == 'd36))  
                        filter_dest_src_id[15:0] <= {rx_wrdata[7:0],rx_wrdata[15:8]}; // Last 16-bits of Src ID
                
				if (state == idle) begin
					config_state <= idle;
                    arb_fifo_wrdata <= 2'd0; //pass through, configuration frames are not fed to the slots
                    arb_fifo_wr <= 1'b1;
                end
			end
            wait_interrupt_l3: begin
            	if (interrupt_ack) 
					interrupt_system <= 1'b0;
                    
                if ((state == stream_pkt_f1) && pkt_fifo_wr && (pkt1_len_cntr == 'd48))  
                        filter_dest_src_id [95:64]<= {rx_wrdata[7:0],rx_wrdata[15:8],rx_wrdata[23:16],rx_wrdata[31:24]}; // Dest ID
                else if ((state == stream_pkt_f1) && pkt_fifo_wr && (pkt1_len_cntr == 'd52))   
                        filter_dest_src_id[63:32]<= {rx_wrdata[7:0],rx_wrdata[15:8],rx_wrdata[23:16],rx_wrdata[31:24]}; // last 2 bytes of Dest ID + 2 bytes Src ID
                else if  ((state == stream_pkt_f1) && pkt_fifo_wr && (pkt1_len_cntr == 'd56))  
                        filter_dest_src_id[31:0] <= {rx_wrdata[7:0],rx_wrdata[15:8],rx_wrdata[23:16],rx_wrdata[31:24]}; // Last 32-bits of Src ID    
                    
				if (state == idle) begin
					config_state <= idle;
                    arb_fifo_wrdata <= 2'd0; //pass through, configuration frames are not fed to the slots
                    arb_fifo_wr <= 1'b1;
                end
			end
            pkt_wait : begin
                // Filtering based on L2 for now, can be extended to L3 (IP packets also).
                if (fifo1_last) begin
                    if ((dest_addr == slot1_config_data[95:48]) && (src_addr == slot1_config_data[47:0]))
                        arb_fifo_wrdata <= 2'd1;
                    else if  ((dest_addr == slot2_config_data[95:48]) && (src_addr == slot2_config_data[47:0]))
                        arb_fifo_wrdata <= 2'd2;
                    else 
                        arb_fifo_wrdata <= 2'd0; //pass through
                end
                if (state == idle) begin
                    config_state <= idle;
                    arb_fifo_wr <= 1'b1;
                end
            end
            pkt_eth_bitstream: begin
                if (state == idle) begin
                    config_state <= idle; // packets ignored are taken as pass throughs
                    arb_fifo_wrdata <= 2'd0; //pass through, configuration frames are not fed to the slots
                    arb_fifo_wr <= 1'b1;
                end
            end
            pkt_ip_bitstream: begin
                if (state == idle) begin
                    config_state <= idle; // packets ignored are taken as pass throughs
                    arb_fifo_wrdata <= 2'd0; //pass through, configuration frames are not fed to the slots
                    arb_fifo_wr <= 1'b1;
                end
            end
            pkt_ignore: begin
				if (state == idle) begin
					config_state <= idle; // packets ignored are taken as pass throughs
                    arb_fifo_wrdata <= 2'd0; //pass through, configuration frames are not fed to the slots
                    arb_fifo_wr <= 1'b1;
                end
			end
		endcase
	end
end
		
reg [31:0] bit_fifo_data;
reg            bit_fifo_wr;
reg [55:0] cur_bit_len;
reg [1:0]   cur_bit_flag;
reg            bit_fifo_rdLast_valid;
reg            bit_fifo_last;
wire [31:0]    axis_dc,axis_rddc;
wire          bit_s_axis_tready;
reg            timer_strt; 
reg            timer_end;
// bitstream buffering SM
always @ (posedge rx_clk) 
begin
    if (~rx_rst_n) begin
		bit_len_val  <= 'd0;
		bit_sm <= 'd0;
		cur_bit_flag <= 0;
        cur_bit_len <= 0;
        bit_fifo_rdLast_valid <= 0;
        bit_fifo_last <= 0;
        useLcl <= 0;
        timer_strt <= 0;
        timer_end <= 0;
	end
	else begin
        bit_fifo_wr <= 0;
        bit_fifo_last <= 0;
        timer_strt <= 0;
        timer_end <= 0;        
		case (bit_sm) 
            2'd0: begin
                bit_fifo_rdLast_valid <= 0;
                useLcl <= 0;
                
                if (bit_sm_check && (config_frameData[1:0] == 2'b10)) begin // start = 1, end = 0
                    bit_len_val <= config_frameData[63:8];
                    cur_bit_flag <= config_frameData[1:0];
                    cur_bit_len <= 0; 
                    if (pkt_fifo_wr) begin
                        cur_bit_len <=  4;
                        bit_fifo_data <=  {rx_wrdata[7:0], rx_wrdata[15:8], rx_wrdata[23:16],rx_wrdata[31:24]};
                        bit_fifo_wr <= 1;
                    end
                    useLcl  <= 1;
                    bit_sm <= 2'd1;
                    timer_strt <= 1;
                end
             end
             2'd1: begin 
                if (pkt_fifo_wr) begin
                    cur_bit_len <=  cur_bit_len + 4;
                    bit_fifo_data <=  {rx_wrdata[7:0], rx_wrdata[15:8], rx_wrdata[23:16],rx_wrdata[31:24]};
                    bit_fifo_wr <= 1;
                    if (pkt1_len_cntr >= pkt1_len + 'd8) // last write
						begin
							 bit_fifo_last <= 1;
						end
                end
                if (fifo1_last) begin
                    
                    if ((cur_bit_len  >= bit_len_val))  begin
                        bit_sm <= 2'd3;
                    end
                    else begin
                        bit_sm <= 2'd2; 
                        useLcl  <= 1;
                    end
                end
                if (cur_bit_flag == 2'b1) // start = 0, end = 1
                    bit_fifo_rdLast_valid <= 1;
                else
                    bit_fifo_rdLast_valid <= 0;
            end
            2'd2: begin
                if (bit_sm_check) begin
                    cur_bit_flag <= config_frameData[1:0];
                    if (config_frameData[1:0] ==  2'b00) // start = 0, end = 0, in between packet
                        bit_fifo_rdLast_valid <= 0;
                    else if  (config_frameData[1:0] ==  2'b01) // start = 0, end = 1, last packet of bitstream
                        bit_fifo_rdLast_valid <= 1;
                    
                    if (pkt_fifo_wr) begin
                        cur_bit_len <=  cur_bit_len + 4;
                        bit_fifo_data <=  {rx_wrdata[7:0], rx_wrdata[15:8], rx_wrdata[23:16],rx_wrdata[31:24]};
                        bit_fifo_wr <= 1;
                    end
                     bit_sm <= 2'd1;
                 end
             end
             2'd3: begin
                     if (Lcl_tlast) begin   
                        cur_bit_flag <= 0;
                        cur_bit_len <= 0;
                        bit_len_val <= 0;
                        bit_sm  <= 0;
                        timer_end <= 1;
                    end
            end
         endcase
    end
 end

reg intr_timer_state; 
 
always @ (posedge rx_clk) 
begin
    if (~rx_rst_n) begin
		intr_timer <= 0;
        intr_timer_state <= 0;
	end
	else begin
        case (intr_timer_state) 
            1'b0 : begin
                if (timer_strt) begin
                    intr_timer <= 32;
                    intr_timer_state <= 1;
                end
            end
            1'b1 : begin
                if (timer_end) begin
                    intr_timer_state <= 0;
                end
                if (~(&intr_timer)) 
                    intr_timer <= intr_timer + 1;
            end
        endcase
    end
end
            
    // bit buffering fifo
bit_buffer_fifo bit_buffer (
  .s_axis_aresetn(rx_rst_n),
  .m_axis_aresetn(rx_rst_n),
  .s_axis_aclk(rx_clk),
  .s_axis_tvalid(bit_fifo_wr),
  .s_axis_tready(bit_s_axis_tready),
  .s_axis_tdata(bit_fifo_data),
  .s_axis_tlast(bit_fifo_last),
  .m_axis_aclk(rx_clk),
  .m_axis_tvalid(Lcl_M_AXIS_MM2S_TVALID),
  .m_axis_tready(Lcl_M_AXIS_MM2S_TREADY),
  .m_axis_tdata(Lcl_M_AXIS_MM2S_TDATA),
  .m_axis_tlast(Lcl_tlast),
  .axis_data_count(axis_dc),
  .axis_wr_data_count(),
  .axis_rd_data_count(axis_rddc)
);    
 
assign  Lcl_M_AXIS_MM2S_TLAST = Lcl_tlast && bit_fifo_rdLast_valid;

 
	// Enet to Accelerator Fifo
	fifo_generator_0 fifo1 
	(
		.s_aclk(rx_clk),
		.s_aresetn(rx_rst_n),
		.s_axis_tvalid(fifo1_wr),
		.s_axis_tready(),
		.s_axis_tdata(fifo_wrdata),
		.s_axis_tlast(fifo1_last),
		.s_axis_tuser(4'd0),
		.axis_data_count(fifo1_datacount),
		.m_axis_tvalid(rxif_fifo_tvalid), // to rxunpacker -- valid
		.m_axis_tready(rxif_fifo_tready), // from rxunpacker -- ready
		.m_axis_tdata(rxif_fifo_tdata), // to rxunpacker -- data
		.m_axis_tlast(rxif_fifo_tlast), // to rxunpacker -- last
		.m_axis_tuser(rxif_fifo_tuser) // open, not needed in rxunpacker.
    );
	// Accelerator to PS Fifo
	fifo_generator_0 fifo2 
	(
		.s_aclk(rx_clk),
		.s_aresetn(rx_rst_n),
		.s_axis_tvalid(txif_fifo_tvalid),
		.s_axis_tready(txif_fifo_tready),
		.s_axis_tdata(txif_fifo_tdata),
		.s_axis_tlast(txif_fifo_tlast),
		.s_axis_tuser(txif_fifo_tuser),
		.axis_data_count(fifo2_datacount),
		.m_axis_tvalid(fifo2_valid), // to rxunpacker -- valid
		.m_axis_tready(fifo2_rd), // from rxunpacker -- ready
		.m_axis_tdata(fifo2_rddata), // to rxunpacker -- data
		.m_axis_tlast(), // to rxunpacker -- last
		.m_axis_tuser() // open, not needed in rxunpacker.
    );
    
    fifo_generator_1 arb_fifo // 2 bit slot identifier fifo for the arbiter.
	(
		.s_aclk(rx_clk),
		.s_aresetn(rx_rst_n),
		.s_axis_tvalid(arb_fifo_wr),
		.s_axis_tready(),
		.s_axis_tdata({6'd0,arb_fifo_wrdata}),
	//	.s_axis_tlast(arb_fifo_wr),
		.s_axis_tuser(4'd0),
		//.axis_data_count(),
		.m_axis_tvalid(arb_fifo_tvalid), // to rxunpacker -- valid
		.m_axis_tready(arb_fifo_tready), // from rxunpacker -- ready
		.m_axis_tdata(arb_fifo_td_l), // to rxunpacker -- data
	//	.m_axis_tlast(arb_fifo_tlast), // to rxunpacker -- last
		.m_axis_tuser() // open, not needed in rxunpacker.
    );
// these writes to be updated to support redirection to accelerator slots.
// 0x80000000 -- 0x80005FFF	
wire bram00_enable = (rx_addr[15:12] == 'd0) && rx_rd; // 0x0000
wire bram10_enable = (rx_addr[15:12] == 'd1) && rx_rd; // 0x1000
wire bram20_enable = (rx_address[15:12] == 'd2) && (rx_read || rx_rd); // 0x2000 -- Tx Buffer1 possibly - Enet DMA Read
wire bram30_enable = (rx_address[15:12] == 'd3) && (rx_read || rx_rd); // 0x3000 -- Tx Buffer2 possibly - Enet DMA Read
wire bram40_enable = (rx_address[15:12] == 'd4) && (rx_read || rx_rd); // 0x4000 -- Modified back data from Accelerator for Enet DMA - Buf2
wire bram50_enable = (rx_address[15:12] == 'd5) && (rx_read || rx_rd); // 0x5000 -- Modified back data from Accelerator for Enet DMA - Buf3

// 0x80010000 -- 0x80015FFF
wire bram01_enable = (rx1_address[15:12] == 'd0) && (rx1_read || rx1_read_d1); // 0x0000
wire bram11_enable = (rx1_address[15:12] == 'd1) && (rx1_read || rx1_read_d1); // 0x1000
wire bram21_enable = (rx1_address[15:12] == 'd2) && (rx1_read || rx1_read_d1 ); // 0x2000 -- Tx Buffer1 possibly - For PS Init
wire bram31_enable = (rx1_address[15:12] == 'd3) && (rx1_read || rx1_read_d1 ); // 0x3000 -- Tx Buffer2 possibly - For PS Init
wire bram41_enable = (rx1_address[15:12] == 'd4) && (rx1_read || rx1_read_d1); // 0x4000 -- Modified back data from Accelerator for PS read - Buf1
wire bram51_enable = (rx1_address[15:12] == 'd5) && (rx1_read || rx1_read_d1); // 0x5000 -- Modified back data from Accelerator for PS read - Buf2
wire fifo2_enable  = (rx1_address[15:12] == 'd6) && (rx1_read || rx1_read_d1); // 0x6000 -- Fifo2 readback so that it doesn't get optimised away
wire config_enable = (rx1_address[15:12] == 'd7) && (rx1_read || rx1_read_d1); // 0x7000 -- Config-1 address to determine configuration info.

wire [31:0] bram0_data, bram1_data,bram2_data,bram3_data,bram4_data,bram5_data,bram6_data,bram7_data;
reg [1:0] write_back_state;
reg       type1_write, type2_write, write;
reg [31:0] brams_wb_data;
reg [9:0]  brams_wb_address;
reg        toggles;
reg [31:0] rx_rdata_d1, rx1_rdata_d1;

always @ (*) begin
	if (bram01_enable) 
		rx1_rdata <= bram0_data;
	else if (bram11_enable) 
		rx1_rdata <= bram1_data;
	else if (bram41_enable) 
		rx1_rdata <= bram4_data;
	else if (bram51_enable)
		rx1_rdata <= bram5_data;
	else if (fifo2_enable)
		rx1_rdata <= fifo2_rddata;
	else if (config_enable)
		rx1_rdata <= config_rddata;
	else 
		rx1_rdata <= rx1_rdata_d1;
end
always @ (posedge rx1_clk) begin
	if (~rx1_rst_n) 
		rx1_rdata_d1 <= 'd0;
	else
		rx1_rdata_d1 <= rx1_rdata;
end
always @ (*) begin
	if (bram20_enable) 
		rx_rdata <= bram2_data;
	else if (bram30_enable) 
		rx_rdata <= bram3_data;
	else if (bram40_enable) 
		rx_rdata <= bram6_data;
	else if (bram50_enable)
		rx_rdata <= bram7_data;
	else 
		rx_rdata <= rx_rdata_d1;
end
always @ (posedge rx_clk) begin
	if (~rx_rst_n) 
		rx_rdata_d1 <= 'd0;
	else
		rx_rdata_d1 <= rx_rdata;
end
// Map FIFO Write-back from Accelerator to BRAMs as well.

always @ (posedge rx1_clk)
begin
	if (~rx1_rst_n) begin
		type1_write <= 1'b0;
		type2_write <= 1'b0;
		brams_wb_data <= 'd0;
		brams_wb_address <= 'd0;
		toggles <= 1'b0;
		write_back_state <= 'd0;
		write <= 1'b0;
	end 
	else begin
		write <= 1'b0;
		case (write_back_state) 
			2'd0 : begin
				if(txif_fifo_tvalid && txif_fifo_tready) begin
					brams_wb_address <= 'd0;
					brams_wb_data <= txif_fifo_tdata;
					if (toggles) begin
						type2_write <= 1'b1;
						write_back_state <= 2'd2;
						write <= 1'b1;
					end
					else begin
						type1_write <= 1'b1;
						write_back_state <= 2'd1;
						write <= 1'b1;
					end
				end
			end
			2'd1: begin
				if(txif_fifo_tvalid && txif_fifo_tready) begin
					brams_wb_address <= brams_wb_address + 1;
					brams_wb_data <= txif_fifo_tdata;
					type1_write <= 1'b1;
					write <= 1'b1;
					if (txif_fifo_tlast) begin 
						write_back_state <= 2'd3;
					end
				end
			end
			2'd2: begin
				if(txif_fifo_tvalid && txif_fifo_tready) begin
					brams_wb_address <= brams_wb_address + 1;
					brams_wb_data <= txif_fifo_tdata;
					type2_write <= 1'b1;
					write <= 1'b1;
					if (txif_fifo_tlast) begin 
						write_back_state <= 2'd3;
					end
				end
			end
			2'd3 : begin 
				type1_write <= 1'b0;
				type2_write <= 1'b0;
				write_back_state <= 2'd0;
				toggles <= ~toggles;
			end
		endcase
	end
end	
	// BRAMs for Incoming Packets and Read back to PS
	blk_mem_gen_0 bram0
	 (
		.clka  (rx_clk),
		.ena   (bram00_enable),
		.wea   ({4{pkt_fifo_wr}}),
		.addra (rx_addr[11:2]),
		.dina  (rx_wrdata),
		.clkb  (rx1_clk),
		.enb   (1'b1),
		.addrb (rx1_address[11:2]),
		.doutb (bram0_data)
	);

	blk_mem_gen_0 bram1
	 (
		.clka  (rx_clk),
		.ena   (bram10_enable),
		.wea   ({4{pkt_fifo_wr}}),
		.addra (rx_addr[11:2]),
		.dina  (rx_wrdata),
		.clkb  (rx1_clk),
		.enb   (1'b1),
		.addrb (rx1_address[11:2]),
		.doutb (bram1_data)
	);
	
	// BRAMs for TxBuffer, iniitalised by PS
	blk_mem_gen_0 bram2
	 (
		.clka  (rx1_clk),
		.ena   (bram21_enable),
		.wea   (rx1_write),
		.addra (rx1_address[11:2]),
		.dina  (rx1_wdata),
		.clkb  (rx_clk),
		.enb   (bram20_enable),
		.addrb (rx_address[11:2]),
		.doutb (bram2_data)
	);
	
	blk_mem_gen_0 bram3
	(
		.clka  (rx1_clk),
		.ena   (bram31_enable),
		.wea   (rx1_write),
		.addra (rx1_address[11:2]),
		.dina  (rx1_wdata),
		.clkb  (rx_clk),
		.enb   (bram21_enable),
		.addrb (rx_address[11:2]),
		.doutb (bram3_data)
	);
	
	// BRAMs for Accelerator Write-back // PS Read
	blk_mem_gen_0 bram4
	(
		.clka  (rx1_clk),
		.ena   (write),
		.wea   ({4{type1_write}}),
		.addra (brams_wb_address),
		.dina  (brams_wb_data),
		.clkb  (rx1_clk),
		.enb   (1'b1),
		.addrb (rx1_address[11:2]),
		.doutb (bram4_data)
	);
	
	blk_mem_gen_0 bram5
	(
		.clka  (rx1_clk),
		.ena   (write),
		.wea   ({4{type2_write}}),
		.addra (brams_wb_address),
		.dina  (brams_wb_data),
		.clkb  (rx1_clk),
		.enb   (1'b1),
		.addrb (rx1_address[11:2]),
		.doutb (bram5_data)
	);
	// BRAMs for Accelerator Write-back // Enet DMA Read
	blk_mem_gen_0 bram6
	(
		.clka  (rx1_clk),
		.ena   (write),
		.wea   ({4{type1_write}}),
		.addra (brams_wb_address),
		.dina  (brams_wb_data),
		.clkb  (rx_clk),
		.enb   (bram40_enable),
		.addrb (rx_address[11:2]),
		.doutb (bram6_data)
	);
	
	blk_mem_gen_0 bram7
	(
		.clka  (rx1_clk),
		.ena   (write),
		.wea   ({4{type2_write}}),
		.addra (brams_wb_address),
		.dina  (brams_wb_data),
		.clkb  (rx_clk),
		.enb   (bram50_enable),
		.addrb (rx_address[11:2]),
		.doutb (bram7_data)
	);
		
endmodule
