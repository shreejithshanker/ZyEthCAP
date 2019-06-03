/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Filename      : sbox_inv.v                                            %
% Function      : PRESENT Decoder SBOX Logic                            %
%                 - Non-linear Inverse substitution model for           %
%                   PRESENT DECODER                                     %
%                 - 4-bit substitution for decoding ciphertext          %
% Author        : SHS                                                   %
% Version       : 1.0 - Base version                                    %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

/* PRESENT DECODER SUBSTITUTION BOX */
module sbox_inv
(
    in,
    out
);

input  [3:0] in;
output reg [3:0] out;

always @ (*) 
begin
    case (in)
        4'hC: out <= 4'h0;
        4'h5: out <= 4'h1;
        4'h6: out <= 4'h2;
        4'hB: out <= 4'h3;
        4'h9: out <= 4'h4;
        4'h0: out <= 4'h5;
        4'hA: out <= 4'h6;
        4'hD: out <= 4'h7;
        4'h3: out <= 4'h8;
        4'hE: out <= 4'h9;
        4'hF: out <= 4'hA;
        4'h8: out <= 4'hB;
        4'h4: out <= 4'hC;
        4'h7: out <= 4'hD;
        4'h1: out <= 4'hE;
        4'h2: out <= 4'hF;

    endcase
end
endmodule