#!/bin/bash

proxychains /tools/Xilinx/Vivado/2017.4/bin/vivado -mode batch -source generate_bitstreams.tcl 
echo 'message:Finished Generating Bitstreams' | zenity --notification
for i in $(find ../bitstreams -type f -name "*pblock*"); do
    name=$(echo $i | cut -d '_' -f 1 | rev | cut -d '/' -f 1 | rev)
    extension=".bif"
    touch ../bitstreams/$name$extension
    printf "the_ROM_image:\n{\n$(echo $i | rev | cut -d '/' -f 1 | rev)\n}" > ../bitstreams/$name$extension
    /tools/Xilinx/SDK/2017.4/bin/bootgen -image ../bitstreams/$name$extension -w -process_bitstream bin
    find ../bitstreams -type f -name "*.bit.bin*"
done