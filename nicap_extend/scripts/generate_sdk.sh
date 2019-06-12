XILINX_SDK='/opt/Xilinx/SDK/2017.4/bin'
XILINX_VIVADO='/opt/Xilinx/Vivado/2017.4/bin/vivado'

rm -rf ../software/.metadata
$XILINX_VIVADO -mode batch -source generate_sdk.tcl 
