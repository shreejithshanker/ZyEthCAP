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
set_msg_config -id {HDL-1065} -limit 10000
set_param project.vivado.isBlockSynthRun true
create_project -in_memory -part xc7z020clg400-1

set_param project.singleFileAddWarning.threshold 0
set_param project.compositeFile.enableAutoGeneration 0
set_param synth.vivado.isSynthRun true
set_property webtalk.parent_dir /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.tmp/partial_led_test_v1_0_project/partial_led_test_v1_0_project.cache/wt [current_project]
set_property parent.project_path /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.tmp/partial_led_test_v1_0_project/partial_led_test_v1_0_project.xpr [current_project]
set_property default_lib xil_defaultlib [current_project]
set_property target_language Verilog [current_project]
set_property board_part www.digilentinc.com:pynq-z1:part0:1.0 [current_project]
set_property ip_repo_paths {
  /home/alex/GitHub/ip_repo/aes_1.0
  /home/alex/GitHub/ip_repo/aes_1.0
  /home/alex/GitHub/ZyEthCAP/zycap_core
  /home/alex/GitHub/zynq-ip-cores
} [current_project]
set_property ip_output_repo /home/alex/GitHub/ZyEthCAP/nicap_extend/nicap_test.tmp/partial_led_test_v1_0_project/partial_led_test_v1_0_project.cache/ip [current_project]
set_property ip_cache_permissions {read write} [current_project]
read_verilog -library xil_defaultlib /home/alex/GitHub/ZyEthCAP/zycap_core/core/vivado/ipcore/partial_led_test_1.0/src/partial_led_test_v1_0_S00_AXI.v
# Mark all dcp files as not used in implementation to prevent them from being
# stitched into the results of this synthesis run. Any black boxes in the
# design are intentionally left as such for best results. Dcp files will be
# stitched into the design at a later time, either when this synthesis run is
# opened, or when it is stitched into a dependent implementation run.
foreach dcp [get_files -quiet -all -filter file_type=="Design\ Checkpoint"] {
  set_property used_in_implementation false $dcp
}

synth_design -top partial_led_test_v1_0_S00_AXI -part xc7z020clg400-1 -mode out_of_context

rename_ref -prefix_all partial_led_test_v1_0_S00_AXI_

# disable binary constraint mode for synth run checkpoints
set_param constraints.enableBinaryConstraints false
write_checkpoint -force -noxdef partial_led_test_v1_0_S00_AXI.dcp
create_report "up_synth_1_synth_report_utilization_0" "report_utilization -file partial_led_test_v1_0_S00_AXI_utilization_synth.rpt -pb partial_led_test_v1_0_S00_AXI_utilization_synth.pb"