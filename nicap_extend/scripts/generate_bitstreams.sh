#!/bin/bash

XILINX_SDK='/tools/Xilinx/SDK/2017.4/bin'

#proxychains /tools/Xilinx/Vivado/2017.4/bin/vivado -mode batch -source generate_bitstreams.tcl 
#/opt/Xilinx/Vivado/2017.4/bin/vivado -mode batch -source generate_bitstreams.tcl 
echo 'message:Finished Generating Bitstreams' | zenity --notification
rm ../bitstreams/*.bin
for i in $(find ../bitstreams -type f -name "*pblock*"); do
    name=$(echo $i | cut -d '_' -f 1 | rev | cut -d '/' -f 1 | rev)
    extension=".bif"
    touch ../bitstreams/$name$extension
    location=$(cd ../bitstreams && pwd)
    file=$(echo $i | rev | cut -d '/' -f 1 | rev)
    echo $file
    printf "the_ROM_image:\n{\n$location/$file\n}" > ../bitstreams/$name$extension
    $XILINX_SDK/bootgen -image $location/$name$extension -w -process_bitstream bin
done

for j in $(find ../bitstreams -type f -name "*.bit.bin*"); do
    k=${j%.bit*}
    mv $j $k.bin
done
