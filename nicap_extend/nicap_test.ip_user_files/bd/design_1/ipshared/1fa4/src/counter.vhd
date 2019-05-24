----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    14:57:31 07/13/2014 
-- Design Name: 
-- Module Name:    counter - Behavioral 
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
use IEEE.STD_LOGIC_unsigned.ALL;
use work.types.all;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity counter is
	port(
		clk   : in  std_logic;
		reset : in  std_logic;
 		y     : in  std_logic_vector(1 downto 0);
		d_out : out byte;
		x     : out std_logic -- boolean indicating if the tenth round is reached (d_out = 'A')

	);
end counter;

architecture Behavioral of counter is
signal reg_D, reg_Q : byte;

begin
	mux_3_1 : process(y, reg_Q)
	begin
		case y is 
			when "00"   => reg_D <= (others => '0');
			when "01"   => reg_D <= reg_Q + 1;
			when others => reg_D <= reg_Q;
		end case;
	end process mux_3_1;
	
	
	reg : process (reset, clk, reg_D)
	begin
			if reset = '1' then
				reg_Q <= (others => '0');
			elsif rising_edge(clk) then
				reg_Q <= reg_D;
			end if;
	end process reg;
	
	comp : process (reg_Q)
	begin
		if reg_Q = x"0A" then
			x <= '1';
		else
			x <= '0';
		end if;
	end process comp;

	d_out <= reg_Q;
	
end Behavioral;

