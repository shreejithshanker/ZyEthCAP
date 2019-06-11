----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    11:26:10 07/21/2014 
-- Design Name: 
-- Module Name:    aes_module - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use work.types.all;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity aes_wrapper is
    port ( clk       : in     std_logic;
			  reset     : in     std_logic;
			  din       : in     std_logic_vector(127 downto 0);
           dout      : out    std_logic_vector(127 downto 0);
			  mode      : in     std_logic_vector(1 downto 0);
           aes_start : in     std_logic;
			  aes_done  : out    std_logic
			 );
end aes_wrapper;

architecture Behavioral of aes_wrapper is

signal conv_mode: aes_mode;

function my_func(inp : std_logic_vector(1 downto 0)) return aes_mode is
begin
    case inp is
        when "00" =>
            return ENCRYPT;
        when "01" =>
            return DECRYPT;
        when "10" =>
            return EXPAND_KEY;
        when others =>
            return EXPAND_KEY;
    end case;
end function my_func;

begin

conv_mode <= my_func(mode);

    aes_module: entity work.aes_module port map (clk => clk,    
                                                                              reset => reset,    
                                                                              din => din,
                                                                              dout => dout,
                                                                              mode => conv_mode,    
                                                                              aes_start => aes_start,
                                                                              aes_done => aes_done
                                                                            );

	

end Behavioral;

