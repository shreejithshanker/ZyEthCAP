/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Filename      : present.v                                             %
% Function      : Top-module of the PRESENT BLOCK CIPHER implementation %
%                 - Integrates the Encoding, Decoding and Subkey        %
%                   generation logic                                    %
%                 - 80-bit Key and 64-bit block size                    %
% Author        : SHS                                                   %
% Version       : 1.0 - Base version                                    %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

/* Present Encoder-Decoder */

module PRESENT
(
    //encoder path
	enc_plaintext,
	enc_ciphertext,
	enc_start, 
    enc_ready,
    enc_hdr_start,
    enc_hdr_done,
//    encoder_ready,
    
    //decoder path
    dec_ciphertext,
    dec_plaintext,
    dec_start,
    dec_ready,
    dec_hdr_start,
    dec_hdr_done,
    
    // Key
    key,
    generate_key, // generate subkeys
    gen_key,
    // Clk,Rst
    clk_in, 
    rst_in

);

parameter b_64 = 64;
parameter b_80 = 80;
parameter b_2 = 2;
parameter b_4 = 4;
parameter b_5 = 5;
parameter b_32 = 32;

input  [b_64-1:0] enc_plaintext,dec_ciphertext;
output [b_64-1:0] enc_ciphertext,dec_plaintext;
input             enc_start, dec_start;
output            enc_ready,dec_ready;
input  [b_80-1:0] key;
input             generate_key;
input             clk_in,rst_in;
input             enc_hdr_start,dec_hdr_start;
output            enc_hdr_done,dec_hdr_done;
output            gen_key;
// output            encoder_ready;

reg    [b_64-1:0]  key_mem [b_32-1:0];
reg    [b_64-1:0]  time_key_mem [b_32-1:0];
reg    [b_64-1:0]  dec_subkey,enc_subkey;
reg                key_generated,gen_key;
reg    [1:0]       keygen_init;
reg    [b_80-1:0]  key_val;
reg                keymem_en,timemem_en,decoding_txt,encoding_txt;
reg                encoder_ready;

wire   [b_64-1:0]  subkey;
wire               keymem_wr;
wire   [b_5-1:0]   keymem_addr,enc_keymem_addr,dec_keymem_addr;
wire               keygen_done;
wire               dec_timekey_gen,enc_timekey_gen;
wire   [b_32-1:0]  enc_rxorer,dec_rxorer;

present_ENC present_ENC
(
	.plaintext(enc_plaintext),
	.ciphertext(enc_ciphertext),
	.start(enc_start&&key_generated), 
    .header_start(enc_hdr_start&&key_generated),
    .header_done(enc_hdr_done),
    .ready(enc_ready),
    .subkey(enc_subkey),
    .keymem_addr(enc_keymem_addr),
    .time_key_gen(enc_timekey_gen),
    .rxorer(enc_rxorer),
    .clk_in(clk_in), 
    .rst_in(rst_in)
);

present_DEC present_DEC
(
	.plaintext(dec_plaintext),
	.ciphertext(dec_ciphertext),
	.start(dec_start&&key_generated), 
    .header_start(dec_hdr_start&&key_generated),
    .header_done(dec_hdr_done),
    .ready(dec_ready),
    .subkey(dec_subkey),
    .keymem_addr(dec_keymem_addr),
    .time_key_gen(dec_timekey_gen),
    .rxorer(dec_rxorer),
    .clk_in(clk_in), 
    .rst_in(rst_in)

);


subkey_gen subkey_gen
(
    .key(key_val),
    .gen_key(gen_key),
    .keygen_done(keygen_done),
    
    .keymem_wr(keymem_wr),
    .subkey(subkey),
    .keymem_addr(keymem_addr),
    
    .clk_in(clk_in),
    .rst_in(rst_in)
);

// Subkey Memory
integer i;
always @ (posedge clk_in)
begin
    if (~rst_in) begin
        for (i=0;i<b_32;i=i+1)
            key_mem[i] <= 'd0;
        encoding_txt <= 1'b0;
        decoding_txt <= 1'b0;
    end
    else begin
        if (keymem_wr && keymem_en) 
            key_mem[keymem_addr] <= subkey;
        if (keymem_wr && timemem_en) 
            time_key_mem[keymem_addr] <= subkey;
        
        if (dec_start || decoding_txt) 
            dec_subkey <= time_key_mem[dec_keymem_addr];
        else 
            dec_subkey <= key_mem[dec_keymem_addr];
        if (enc_start || encoding_txt) 
            enc_subkey <= time_key_mem[enc_keymem_addr];
        else
            enc_subkey <= key_mem[enc_keymem_addr];
            
        if (dec_start) 
            decoding_txt <= 1'b1;
        else if (dec_ready)
            decoding_txt <= 1'b0;
        if (enc_start) 
            encoding_txt <= 1'b1;
        else if (enc_ready)
            encoding_txt <= 1'b0;

    end
end


always @ (posedge clk_in)
begin
    if (~rst_in) begin
        keygen_init   <= 2'd0;
        gen_key       <= 1'b0;
        key_generated <= 1'b0;
        key_val       <= key;
        keymem_en     <= 1'b1;
        timemem_en    <= 1'b0;
        encoder_ready <= 1'b0;
    end
    else begin
        gen_key <= 1'b0;
        case (keygen_init) 
            2'd0 : begin
                if (~key_generated) begin
                    keygen_init <= 2'd1;
                    key_val     <= key;
                    keymem_en   <= 1'b1;
                    timemem_en  <= 1'b0;
                end
            end
            2'd1 : begin
                gen_key <= 1'b1;
                keygen_init <= 2'd2;
            end
            2'd2: begin
                if (keygen_done) begin
                    keygen_init <= 2'd3;
                    key_generated <= 1'b1;
                    keymem_en   <= 1'b0;
                end
            end
            2'd3: begin
                if (generate_key) begin
                    keygen_init <= 2'd0;
                    timemem_en  <= 1'b0;
                    keymem_en   <= 1'b1;
                end
                else begin
                    keymem_en  <= 1'b0;
                    timemem_en <= 1'b1;
                    if (dec_timekey_gen) begin
                        key_val <= key ^ {dec_rxorer,dec_rxorer,dec_rxorer[31:16]};
                        gen_key <= 1'b1;
                    end
                    else if (enc_timekey_gen) begin
                        key_val <= key ^ {enc_rxorer,enc_rxorer,enc_rxorer[31:16]};
                        gen_key <= 1'b1;
                        encoder_ready <= 1'b0;
                    end
                    if ((~encoder_ready) && keygen_done)
                        encoder_ready <= 1'b1;
                end
            end
        endcase
        if(generate_key && key_generated)
            key_generated <= 1'b0;
    end
end

endmodule
