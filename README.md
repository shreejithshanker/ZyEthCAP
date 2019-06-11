# ZyEthCAP

ZyEthCAP extends ZyNCAP by adding capability for receiving bitstream over Ethernet. 
Frames are identified by an 8-byte Bitstream Identifier (0011AABBCCDDEEFF) followed by a 7-byte Length Field (bitstream length in bytes) and a 1-byte bitframe identifier -- the last 2 bits of the bitframe ID must be set to '10' for starting frame, '01' for ending frame and '00' for all others. 

The application sdk file ZyNCAP.c initialises the ethernet frame handling for the PSEMAC. 

The demo application does a regular ZyCAP operation and then prompts the user to perform the ZyEthCAP test using the provided Python script. 
The application prompts the user via the UART interface and user should type a 3 key press command (i.e. 'yes') to carry on (set to fixed 3 key press for easiness) 
Observe the console using a terminal application like TeraTerm (8-bit at a baud rate of 115200 bps)

Once the python script is done, type on 3 key press command on the UART console to read the time consumed by the ethernet operation, based on a hardware counter. The counter counts the time from the reception of the first frame to the completion of reconfiguration operation. 

### Tests

#### PCAP

### Info

Test on PynQ-Z1 board
Device xc7z020clg400-1 

Partial bitstream size: 151488 bytes
Time consumed: 270 to 305 ms