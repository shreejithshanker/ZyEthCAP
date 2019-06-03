# 
# Synthesis run script generated by Vivado
# 

proc create_report { reportName command } {
  set status "."
  append status $reportName ".fail"
  if { [file exists $status] } {
    eval file delete [glob $status]
  }
  send_msg_id runtcl-4 info "Executing : $command"
  set retval [eval catch { $command } msg]
  if { $retval != 0 } {
    set fp [open $status w]
    close $fp
    send_msg_id runtcl-5 warning "$msg"
  }
}
create_project -in_memory -part xc7z020clg400-1

set_param project.singleFileAddWarning.threshold 0
set_param project.compositeFile.enableAutoGeneration 0
set_param synth.vivado.isSynthRun true
set_msg_config -source 4 -id {IP_Flow 19-2162} -severity warning -new_severity info
set_property webtalk.parent_dir /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.cache/wt [current_project]
set_property parent.project_path /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.xpr [current_project]
set_property XPM_LIBRARIES {XPM_CDC XPM_FIFO XPM_MEMORY} [current_project]
set_property default_lib xil_defaultlib [current_project]
set_property target_language Verilog [current_project]
set_property board_part www.digilentinc.com:pynq-z1:part0:1.0 [current_project]
set_property ip_repo_paths {
  /home/alex/GitHub/ZyEthCAP/zycap_core
  /home/alex/GitHub/zynq-ip-cores
} [current_project]
set_property ip_output_repo /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.cache/ip [current_project]
set_property ip_cache_permissions {read write} [current_project]
read_verilog -library xil_defaultlib {
  /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/hdl/design_1_wrapper.v
  /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/imports/nicap_extend/sources_1/imports/sources_1/imports/src/enet_bridge.v
  /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/imports/nicap_extend/sources_1/imports/sources_1/imports/src/framePassThru.v
  /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/imports/nicap_extend/sources_1/imports/sources_1/imports/src/framedecrementer.v
  /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/imports/nicap_extend/sources_1/new/framedecrementer_wrap.v
  /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/imports/nicap_extend/sources_1/imports/sources_1/imports/src/frameincrementer.v
  /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/imports/nicap_extend/sources_1/imports/sources_1/imports/src/frameincrementer_wrap.v
  /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/imports/nicap_extend/sources_1/imports/sources_1/imports/src/rx_arb.v
  /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/imports/nicap_extend/sources_1/imports/sources_1/imports/src/tx_arb.v
  /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/imports/nicap_extend/sources_1/imports/sources_1/new/sys_top.v
}
read_ip -quiet /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/ip/blk_mem_gen_0/blk_mem_gen_0.xci
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/ip/blk_mem_gen_0/blk_mem_gen_0_ooc.xdc]

read_ip -quiet /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/ip/fifo_generator_1/fifo_generator_1.xci
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/ip/fifo_generator_1/fifo_generator_1.xdc]

read_ip -quiet /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/ip/fifo_generator_0/fifo_generator_0.xci
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/ip/fifo_generator_0/fifo_generator_0.xdc]

add_files /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/design_1.bd
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_blk_mem_gen_0_0/design_1_blk_mem_gen_0_0_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_blk_mem_gen_0_1/design_1_blk_mem_gen_0_1_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_axi_bram_ctrl_0_0/design_1_axi_bram_ctrl_0_0_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_axi_bram_ctrl_0_1/design_1_axi_bram_ctrl_0_1_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_rst_processing_system7_0_50M_0/design_1_rst_processing_system7_0_50M_0_board.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_rst_processing_system7_0_50M_0/design_1_rst_processing_system7_0_50M_0.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_rst_processing_system7_0_50M_0/design_1_rst_processing_system7_0_50M_0_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_processing_system7_0_2/design_1_processing_system7_0_2.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_axi_bram_ctrl_2_0/design_1_axi_bram_ctrl_2_0_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_axi_bram_ctrl_3_0/design_1_axi_bram_ctrl_3_0_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_zycap_0_0/src/zycap_axi_dma_0_0/zycap_axi_dma_0_0_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_zycap_0_0/src/zycap_axi_dma_0_0/zycap_axi_dma_0_0.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_zycap_0_0/src/zycap_axi_dma_0_0/zycap_axi_dma_0_0_clocks.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_axi_smc_0/bd_0/ip/ip_5/bd_afc3_s00a2s_0_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_axi_smc_0/bd_0/ip/ip_8/bd_afc3_m00s2a_0_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_axi_smc_0/bd_0/ip/ip_6/bd_afc3_sarn_0_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_axi_smc_0/bd_0/ip/ip_7/bd_afc3_srn_0_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_axi_smc_0/bd_0/ip/ip_1/bd_afc3_psr_aclk_0_board.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_axi_smc_0/bd_0/ip/ip_1/bd_afc3_psr_aclk_0.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_axi_smc_0/ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_xbar_1/design_1_xbar_1_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_xbar_0/design_1_xbar_0_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_axi_dma_0_1/design_1_axi_dma_0_1_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_axi_dma_0_1/design_1_axi_dma_0_1.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_axi_dma_0_1/design_1_axi_dma_0_1_clocks.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_auto_pc_4/design_1_auto_pc_4_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_auto_pc_5/design_1_auto_pc_5_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_auto_pc_2/design_1_auto_pc_2_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_auto_pc_1/design_1_auto_pc_1_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_auto_pc_0/design_1_auto_pc_0_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/ip/design_1_auto_pc_3/design_1_auto_pc_3_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/bd/design_1/design_1_ooc.xdc]

read_ip -quiet /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/ip/bit_buffer_fifo/bit_buffer_fifo.xci
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/ip/bit_buffer_fifo/bit_buffer_fifo_ooc.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/ip/bit_buffer_fifo/bit_buffer_fifo/bit_buffer_fifo.xdc]
set_property used_in_implementation false [get_files -all /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/sources_1/ip/bit_buffer_fifo/bit_buffer_fifo/bit_buffer_fifo_clocks.xdc]

# Mark all dcp files as not used in implementation to prevent them from being
# stitched into the results of this synthesis run. Any black boxes in the
# design are intentionally left as such for best results. Dcp files will be
# stitched into the design at a later time, either when this synthesis run is
# opened, or when it is stitched into a dependent implementation run.
foreach dcp [get_files -quiet -all -filter file_type=="Design\ Checkpoint"] {
  set_property used_in_implementation false $dcp
}
read_xdc /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/constrs_1/new/top_io.xdc
set_property used_in_implementation false [get_files /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.srcs/constrs_1/new/top_io.xdc]

read_xdc dont_touch.xdc
set_property used_in_implementation false [get_files dont_touch.xdc]

synth_design -top sys_top -part xc7z020clg400-1


# disable binary constraint mode for synth run checkpoints
set_param constraints.enableBinaryConstraints false
write_checkpoint -force -noxdef sys_top.dcp
create_report "synth_1_synth_report_utilization_0" "report_utilization -file sys_top_utilization_synth.rpt -pb sys_top_utilization_synth.pb"
