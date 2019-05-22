-makelib ies_lib/xil_defaultlib -sv \
  "/tools/Xilinx/Vivado/2017.4/data/ip/xpm/xpm_cdc/hdl/xpm_cdc.sv" \
  "/tools/Xilinx/Vivado/2017.4/data/ip/xpm/xpm_fifo/hdl/xpm_fifo.sv" \
  "/tools/Xilinx/Vivado/2017.4/data/ip/xpm/xpm_memory/hdl/xpm_memory.sv" \
-endlib
-makelib ies_lib/xpm \
  "/tools/Xilinx/Vivado/2017.4/data/ip/xpm/xpm_VCOMP.vhd" \
-endlib
-makelib ies_lib/lib_cdc_v1_0_2 \
  "../../../ipstatic/hdl/lib_cdc_v1_0_rfs.vhd" \
-endlib
-makelib ies_lib/proc_sys_reset_v5_0_12 \
  "../../../ipstatic/hdl/proc_sys_reset_v5_0_vh_rfs.vhd" \
-endlib
-makelib ies_lib/xil_defaultlib \
  "../../../../axis_dwidth_converter_1_ex.srcs/sources_1/ip/proc_sys_reset_0/sim/proc_sys_reset_0.vhd" \
-endlib
-makelib ies_lib/xil_defaultlib \
  glbl.v
-endlib

