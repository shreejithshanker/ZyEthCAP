/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Filename      : roundkey.v                                            %
% Function      : PRESENT Round-key generation Logic                    %
%                 - Precomputes the 64-bit round subkey for each round  %
%                   from the given 80-bit key                           %
%                 - Sub-key used for data-encoding/decoding and is      %
%                   stored in sub-key memory                            %
% Author        : SHS                                                   %
% Version       : 1.0 - Base version                                    %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/


/* Round-key generation logic */

module subkey_gen
(
    key,
    gen_key,
    keygen_done,
    
    keymem_wr,
    subkey,
    keymem_addr,
    
    clk_in,
    rst_in
);

parameter b_64 = 64;
parameter b_80 = 80;
parameter round = 32;
parameter b_5  = 5;
parameter b_4  = 4;

input  [b_80-1:0]     key; 
input                 gen_key;
output                keygen_done;
output reg            keymem_wr;
output     [b_64-1:0] subkey; 
output reg [b_5-1:0]  keymem_addr;

input                 clk_in,rst_in;

wire [b_80-1:0] key_temp,interkey; 
wire [b_5-1:0]  key_rand;
wire [b_4-1:0]  key_updt;

reg  [b_5-1:0]  cntr_val;
reg  [b_80-1:0] key_reg; 
reg             cntr_en;

// Round Key updating

// Key Mux
always @ (posedge clk_in)
begin
    if (~rst_in) begin
        key_reg <= 'd0;
    end
    else begin
        if (cntr_en) begin
            if (|cntr_val)
                key_reg <= interkey;
            else 
                key_reg <= key;
        end
        else 
            key_reg <= 'd0;
    end
end

    
    assign key_temp = {key_reg[18:0],key_reg[79:19]}; // Rotate Key by 19 bits
    sbox key_sub // Pass upper bits of key through SBOx
        (
            .in(key_temp[79:76]),
            .out(key_updt)
        );
    assign key_rand = key_temp[19:15]^cntr_val;
    assign interkey = {key_updt, key_temp[75:20], key_rand, key_temp[14:0]};

// Round Counter    
always @ (posedge clk_in)
begin
    if (~rst_in) begin
        cntr_val <= 'd0;
        cntr_en  <= 1'b0;
        keymem_wr <= 1'b0;
        keymem_addr <= 'd0;        
    end
    else begin
        case (cntr_en) 
            1'b0: begin 
                if (gen_key) begin
                    cntr_en <= 1'b1;
                    cntr_val <= 'd0;
                end
            end
            1'b1: begin
                if (&cntr_val)
                    cntr_en <= 1'b0;
                else
                    cntr_val <= cntr_val + 1'b1;
            end
        endcase
        keymem_wr <= cntr_en;
        keymem_addr <= cntr_val;
    end
end

assign subkey = key_reg [b_80-1:b_80-b_64];
assign keygen_done = keymem_wr && (~cntr_en);

endmodule