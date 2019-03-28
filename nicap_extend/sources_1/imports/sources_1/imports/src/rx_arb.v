module rxarb
(

input                  rxif_fifo_tvalid, // to rxunpacker -- valid
output	reg         rxif_fifo_tready, // from rxunpacker -- ready
input  [31:0]	  rxif_fifo_tdata, // to rxunpacker -- data
input			      rxif_fifo_tlast, // to rxunpacker -- last

output reg              slt1_fifo_tvalid,
input                      slt1_fifo_tready,
output reg [31:0]   slt1_fifo_tdata,
output reg              slt1_fifo_tlast,

output reg              slt2_fifo_tvalid,
input                      slt2_fifo_tready,
output reg [31:0]   slt2_fifo_tdata,
output reg              slt2_fifo_tlast,

output reg              passThru_fifo_tvalid,
input                      passThru_fifo_tready,
output reg [31:0]   passThru_fifo_tdata,
output reg              passThru_fifo_tlast,

input              arb_fifo_tvalid, // to rxunpacker -- valid
output	reg 	  arb_fifo_tready, // from rxunpacker -- ready
input  [1:0]	  arb_fifo_tdata, // to rxunpacker -- data

input               clk, rst
);

parameter arb_idle   = 2'd0,
          arb_setup_lnk = 2'd1,
		  arb_strm_pk  = 2'd2,
		  arb_release_lnk = 2'd3;


reg [1:0] arb_states;
reg [1:0] mux_cntrl; 
reg          fifo_tready;

always @ (posedge clk) begin
    if (~rst) begin
        arb_states <= arb_idle;
        mux_cntrl <= 2'd0;
        arb_fifo_tready <= 1'b0;
    end 
    else begin
        
        arb_fifo_tready <= 1'b0;
        
        case (arb_states)
            arb_idle: begin
                if (arb_fifo_tvalid) begin
                    mux_cntrl <= arb_fifo_tdata;
                    arb_fifo_tready <= 1'b1;
                    arb_states <= arb_setup_lnk;
                end
             end
             
            arb_setup_lnk: begin
                if (rxif_fifo_tlast) 
                    arb_states <= arb_release_lnk;
                else 
                    arb_states <= arb_strm_pk;
            end
            
            arb_strm_pk: begin
                if (rxif_fifo_tlast && fifo_tready) begin 
                    arb_states <= arb_release_lnk;
                    mux_cntrl <= 2'd0;
                end
            end
            
            arb_release_lnk: begin
                if (arb_fifo_tvalid) begin
                    mux_cntrl <= arb_fifo_tdata;
                    arb_fifo_tready <= 1'b1;
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
                slt1_fifo_tvalid         <= 'd0;
                slt1_fifo_tdata          <= 'd0;
                slt1_fifo_tlast           <= 'd0;
  
                slt2_fifo_tvalid         <= 'd0;
                slt2_fifo_tdata          <= 'd0;
                slt2_fifo_tlast           <= 'd0;
            
                passThru_fifo_tvalid <='d0;
                passThru_fifo_tdata <= 'd0;
                passThru_fifo_tlast  <= 'd0;
                
                rxif_fifo_tready          <= 'd0;
                fifo_tready                <= 'd0;
    end 
    else if (arb_states == arb_setup_lnk || arb_states == arb_strm_pk) begin
        case (mux_cntrl) 
            2'd0: begin
                slt1_fifo_tvalid         <= 'd0;
                slt1_fifo_tdata          <= 'd0;
                slt1_fifo_tlast           <= 'd0;
  
                slt2_fifo_tvalid         <= 'd0;
                slt2_fifo_tdata          <= 'd0;
                slt2_fifo_tlast           <= 'd0;
            
                passThru_fifo_tvalid <= rxif_fifo_tvalid; 
                passThru_fifo_tdata <= rxif_fifo_tdata;
                passThru_fifo_tlast  <= rxif_fifo_tlast;
                
                rxif_fifo_tready        <= passThru_fifo_tready;
                fifo_tready              <= passThru_fifo_tready;
                
            end
            2'd1: begin
                slt1_fifo_tvalid         <= rxif_fifo_tvalid; 
                slt1_fifo_tdata          <= rxif_fifo_tdata;
                slt1_fifo_tlast           <= rxif_fifo_tlast;
  
                slt2_fifo_tvalid         <= 'd0;
                slt2_fifo_tdata          <= 'd0;
                slt2_fifo_tlast           <= 'd0;
            
                passThru_fifo_tvalid <='d0;
                passThru_fifo_tdata <= 'd0;
                passThru_fifo_tlast  <= 'd0;
                
                rxif_fifo_tready          <= slt1_fifo_tready;
                fifo_tready              <= slt1_fifo_tready;
                
            end
            2'd2: begin
                slt1_fifo_tvalid         <= 'd0;
                slt1_fifo_tdata          <= 'd0;
                slt1_fifo_tlast           <= 'd0;
  
                slt2_fifo_tvalid         <= rxif_fifo_tvalid; 
                slt2_fifo_tdata          <= rxif_fifo_tdata;
                slt2_fifo_tlast           <= rxif_fifo_tlast;
            
                passThru_fifo_tvalid <='d0;
                passThru_fifo_tdata <= 'd0;
                passThru_fifo_tlast  <= 'd0;
                
                rxif_fifo_tready          <= slt2_fifo_tready;
                fifo_tready              <= slt2_fifo_tready;
             end
         endcase
    end
    else if (arb_states == arb_release_lnk || arb_states == arb_idle) begin
                    slt1_fifo_tvalid         <= 'd0;
                slt1_fifo_tdata          <= 'd0;
                slt1_fifo_tlast           <= 'd0;
  
                slt2_fifo_tvalid         <= 'd0;
                slt2_fifo_tdata          <= 'd0;
                slt2_fifo_tlast           <= 'd0;
            
                passThru_fifo_tvalid <='d0;
                passThru_fifo_tdata <= 'd0;
                passThru_fifo_tlast  <= 'd0;
                
                rxif_fifo_tready          <= 'd0;
                fifo_tready                <= 'd0;
    end
end

endmodule