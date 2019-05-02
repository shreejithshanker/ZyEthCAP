module framePassThru
(
input            clk,
input            rst_n,
input            rxif_fifo_tvalid, // to rxunpacker -- valid
output reg		 rxif_fifo_tready, // from rxunpacker -- ready
input [31:0]	 rxif_fifo_tdata, // to rxunpacker -- data
input			 rxif_fifo_tlast, // to rxunpacker -- last
input  [3:0]     rxif_fifo_tuser,
output reg       txif_fifo_tvalid,
input            txif_fifo_tready,
output reg [31:0] txif_fifo_tdata,
output reg		  txif_fifo_tlast,
output reg	[3:0] txif_fifo_tuser
);

reg [9:0] len_counter;
reg [1:0] frame_state;
always @ (posedge clk) 
begin
	if (~rst_n) begin
		frame_state <= 'd0;
		rxif_fifo_tready <= 1'b0;
		txif_fifo_tvalid <= 1'b0;
		txif_fifo_tdata <= 'd0;
		txif_fifo_tlast <= 'd0; 
		txif_fifo_tuser <= 'd0;
		len_counter <= 'd0;
	end
	else begin
		rxif_fifo_tready <= 1'b1;
		txif_fifo_tvalid <= rxif_fifo_tvalid;
		txif_fifo_tuser  <= rxif_fifo_tuser;
		txif_fifo_tlast  <= rxif_fifo_tlast;
		case (frame_state)
		2'b0: begin
			if (rxif_fifo_tvalid) begin
				len_counter <= 4;
				txif_fifo_tdata <= rxif_fifo_tdata;
				frame_state <= 2'd1;
			end
		end
		2'd1: begin
			if (rxif_fifo_tvalid) begin
				if (len_counter >= 16)
					txif_fifo_tdata <= rxif_fifo_tdata + 0;
				else 
					txif_fifo_tdata <= rxif_fifo_tdata;
				len_counter <= len_counter + 4;
				if (rxif_fifo_tlast) 
					frame_state <= 2'd2;
			end
		end
		2'd2 : begin
			len_counter <= 'd0;
			frame_state <= 'd0;
		end
		endcase
	end
end
endmodule