vlib work
vlib riviera

vlib riviera/xil_defaultlib
vlib riviera/xpm
vlib riviera/axis_infrastructure_v1_1_0
vlib riviera/axis_register_slice_v1_1_15
vlib riviera/axis_dwidth_converter_v1_1_14

vmap xil_defaultlib riviera/xil_defaultlib
vmap xpm riviera/xpm
vmap axis_infrastructure_v1_1_0 riviera/axis_infrastructure_v1_1_0
vmap axis_register_slice_v1_1_15 riviera/axis_register_slice_v1_1_15
vmap axis_dwidth_converter_v1_1_14 riviera/axis_dwidth_converter_v1_1_14

vlog -work xil_defaultlib  -sv2k12 "+incdir+../../../ipstatic/hdl" "+incdir+../../../ipstatic/hdl" \
"/tools/Xilinx/Vivado/2017.4/data/ip/xpm/xpm_cdc/hdl/xpm_cdc.sv" \
"/tools/Xilinx/Vivado/2017.4/data/ip/xpm/xpm_fifo/hdl/xpm_fifo.sv" \
"/tools/Xilinx/Vivado/2017.4/data/ip/xpm/xpm_memory/hdl/xpm_memory.sv" \

vcom -work xpm -93 \
"/tools/Xilinx/Vivado/2017.4/data/ip/xpm/xpm_VCOMP.vhd" \

vlog -work axis_infrastructure_v1_1_0  -v2k5 "+incdir+../../../ipstatic/hdl" "+incdir+../../../ipstatic/hdl" \
"../../../ipstatic/hdl/axis_infrastructure_v1_1_vl_rfs.v" \

vlog -work axis_register_slice_v1_1_15  -v2k5 "+incdir+../../../ipstatic/hdl" "+incdir+../../../ipstatic/hdl" \
"../../../ipstatic/hdl/axis_register_slice_v1_1_vl_rfs.v" \

vlog -work axis_dwidth_converter_v1_1_14  -v2k5 "+incdir+../../../ipstatic/hdl" "+incdir+../../../ipstatic/hdl" \
"../../../ipstatic/hdl/axis_dwidth_converter_v1_1_vl_rfs.v" \

vlog -work xil_defaultlib  -v2k5 "+incdir+../../../ipstatic/hdl" "+incdir+../../../ipstatic/hdl" \
"../../../../axis_dwidth_converter_1_ex.srcs/sources_1/ip/axis_dwidth_converter_1/sim/axis_dwidth_converter_1.v" \

vlog -work xil_defaultlib \
"glbl.v"

