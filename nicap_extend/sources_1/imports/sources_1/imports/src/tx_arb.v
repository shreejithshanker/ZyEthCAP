module txarb
(

output reg                   txif_fifo_tvalid, // to enet bridge -- valid
input                           txif_fifo_tready, // from slots -- ready
output reg [31:0]	        txif_fifo_tdata, // to enet bridge -- data
output reg			        txif_fifo_tlast, // to  enet bridge-- last

input                           slt1_fifo_tvalid,
output reg                  slt1_fifo_tready,
input [31:0]                slt1_fifo_tdata,
input                          slt1_fifo_tlast,

input                          slt2_fifo_tvalid,
output reg                  slt2_fifo_tready,
input [31:0]                slt2_fifo_tdata,
input                          slt2_fifo_tlast,

input                           passThru_fifo_tvalid,
output reg                  passThru_fifo_tready,
input [31:0]                passThru_fifo_tdata,
input                           passThru_fifo_tlast,

input               clk, rst
);

parameter arb_idle   = 2'd0,
          arb_setup_lnk = 2'd1,
		  arb_strm_pk  = 2'd2,
		  arb_release_lnk = 2'd3;


reg [1:0] arb_states;
reg [1:0] mux_cntrl; 
reg          fifo_tlast;

always @ (posedge clk) begin
    if (~rst) begin
        arb_states <= arb_idle;
        mux_cntrl <= 2'd0;
    end 
    else begin
                
        case  (arb_states)
            arb_idle: begin
                if (slt1_fifo_tvalid) begin
                    mux_cntrl <= 2'd1;
                    arb_states <= arb_setup_lnk;
                end
                else if (slt2_fifo_tvalid) begin
                    mux_cntrl <= 2'd2;
                    arb_states <= arb_setup_lnk;
                end
                else if (passThru_fifo_tvalid) begin
                    mux_cntrl <= 2'd0;
                    arb_states <= arb_setup_lnk;
                end
            end
            
            arb_setup_lnk: begin
                if (fifo_tlast) 
                    arb_states <= arb_release_lnk;
                else 
                    arb_states <= arb_strm_pk;
            end
            
            arb_strm_pk: begin
                if (txif_fifo_tready && fifo_tlast) begin 
                    arb_states <= arb_release_lnk;
                    mux_cntrl <= 2'd0;
                end
            end
            
            arb_release_lnk: begin
                if (slt1_fifo_tvalid) begin
                    mux_cntrl <= 2'd1;
                    arb_states <= arb_setup_lnk;
                end
                else if (slt2_fifo_tvalid) begin
                    mux_cntrl <= 2'd2;
                    arb_states <= arb_setup_lnk;
                end
                else if (passThru_fifo_tvalid) begin
                    mux_cntrl <= 2'd0;
                    arb_states <= arb_setup_lnk;
                end
                else 
                   arb_states <= arb_idle;
            end
        endcase
    end
end


always @ (*) begin
    if (~rst) begin
                slt1_fifo_tready         <= 'd0;
                slt2_fifo_tready          <= 'd0;         
                passThru_fifo_tready <='d0;
                txif_fifo_tvalid <= 'd0;
                txif_fifo_tdata  <= 'd0;
                txif_fifo_tlast          <= 'd0;
                fifo_tlast                <= 'd0;
    end 
    else if (arb_states == arb_setup_lnk || arb_states == arb_strm_pk) begin
        case (mux_cntrl) 
            2'd0: begin
                slt1_fifo_tready         <= 'd0;
  
                slt2_fifo_tready         <= 'd0;
                
                passThru_fifo_tready <= txif_fifo_tready;
                
                txif_fifo_tvalid          <= passThru_fifo_tvalid;
                txif_fifo_tdata           <= passThru_fifo_tdata; 
                txif_fifo_tlast            <= passThru_fifo_tlast; 
         
                fifo_tlast                   <= passThru_fifo_tlast;
                
            end
            2'd1: begin
                slt1_fifo_tready         <= txif_fifo_tready;
  
                slt2_fifo_tready         <= 'd0;
                
                passThru_fifo_tready <= 'd0;
                
                txif_fifo_tvalid          <= slt1_fifo_tvalid;
                txif_fifo_tdata           <= slt1_fifo_tdata; 
                txif_fifo_tlast            <= slt1_fifo_tlast; 
         
                fifo_tlast                   <= slt1_fifo_tlast;
                
            end
            2'd2: begin
                slt1_fifo_tready         <= 'd0;
  
                slt2_fifo_tready         <= txif_fifo_tready;
                
                passThru_fifo_tready <= 'd0;
                
                txif_fifo_tvalid          <= slt2_fifo_tvalid;
                txif_fifo_tdata           <= slt2_fifo_tdata; 
                txif_fifo_tlast            <= slt2_fifo_tlast; 
         
                fifo_tlast                   <= slt2_fifo_tlast;
             end
         endcase
    end
    else if (arb_states == arb_release_lnk || arb_states == arb_idle) begin
                slt1_fifo_tready         <= 'd0;
                slt2_fifo_tready          <= 'd0;         
                passThru_fifo_tready <='d0;
                txif_fifo_tvalid <= 'd0;
                txif_fifo_tdata  <= 'd0;
                txif_fifo_tlast          <= 'd0;
                fifo_tlast                <= 'd0;
    end
end

endmodule