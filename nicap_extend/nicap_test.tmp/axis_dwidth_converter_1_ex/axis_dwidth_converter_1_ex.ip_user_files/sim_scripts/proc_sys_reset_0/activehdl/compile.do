vlib work
vlib activehdl

vlib activehdl/xil_defaultlib
vlib activehdl/xpm
vlib activehdl/lib_cdc_v1_0_2
vlib activehdl/proc_sys_reset_v5_0_12

vmap xil_defaultlib activehdl/xil_defaultlib
vmap xpm activehdl/xpm
vmap lib_cdc_v1_0_2 activehdl/lib_cdc_v1_0_2
vmap proc_sys_reset_v5_0_12 activehdl/proc_sys_reset_v5_0_12

vlog -work xil_defaultlib  -sv2k12 \
"/tools/Xilinx/Vivado/2017.4/data/ip/xpm/xpm_cdc/hdl/xpm_cdc.sv" \
"/tools/Xilinx/Vivado/2017.4/data/ip/xpm/xpm_fifo/hdl/xpm_fifo.sv" \
"/tools/Xilinx/Vivado/2017.4/data/ip/xpm/xpm_memory/hdl/xpm_memory.sv" \

vcom -work xpm -93 \
"/tools/Xilinx/Vivado/2017.4/data/ip/xpm/xpm_VCOMP.vhd" \

vcom -work lib_cdc_v1_0_2 -93 \
"../../../ipstatic/hdl/lib_cdc_v1_0_rfs.vhd" \

vcom -work proc_sys_reset_v5_0_12 -93 \
"../../../ipstatic/hdl/proc_sys_reset_v5_0_vh_rfs.vhd" \

vcom -work xil_defaultlib -93 \
"../../../../axis_dwidth_converter_1_ex.srcs/sources_1/ip/proc_sys_reset_0/sim/proc_sys_reset_0.vhd" \

vlog -work xil_defaultlib \
"glbl.v"

