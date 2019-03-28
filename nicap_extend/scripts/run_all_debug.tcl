
# MODE 1

open_checkpoint ./checkpoints/top_synth_debug.dcp
#open_checkpoint ./checkpoints/top_synth.dcp
set_property HD.RECONFIGURABLE true [get_cells design_1_wrapper/design_1_i/partial_led_test_0/inst/partial_led_test_v1_0_S00_AXI_inst]

read_xdc ./constraints/top_io.xdc

read_checkpoint ./modes/mode1.dcp -cell design_1_wrapper/design_1_i/partial_led_test_0/inst/partial_led_test_v1_0_S00_AXI_inst
# Floor plan
startgroup
create_pblock pblock_partial_led_test_0
resize_pblock pblock_partial_led_test_0 -add SLICE_X64Y100:SLICE_X67Y149
add_cells_to_pblock pblock_partial_led_test_0 [get_cells [list design_1_wrapper/design_1_i/partial_led_test_0/inst/partial_led_test_v1_0_S00_AXI_inst]] -clear_locs
endgroup
set_property RESET_AFTER_RECONFIG 1 [get_pblocks pblock_partial_led_test_0]
set_property SNAPPING_MODE ON [get_pblocks pblock_partial_led_test_0]

write_xdc ./constraints/top_fplan.xdc -force
write_checkpoint ./checkpoints/top_link_mode1.dcp -force

# DRC for PR
create_drc_ruledeck ruledeck_1
add_drc_checks -ruledeck ruledeck_1 [get_drc_checks {HDPRA-62 HDPRA-60 HDPRA-58 HDPRA-57 HDPRA-56 HDPRA-55 HDPRA-54 HDPRA-53 HDPRA-52 HDPRA-51 HDPRA-21 HDPR-43 HDPR-20 HDPR-41 HDPR-40 HDPR-30 HDPR-87 HDPR-86 HDPR-85 HDPR-84 HDPR-83 HDPR-74 HDPR-73 HDPR-72 HDPR-71 HDPR-70 HDPR-69 HDPR-68 HDPR-67 HDPR-66 HDPR-65 HDPR-64 HDPR-63 HDPR-62 HDPR-61 HDPR-60 HDPR-59 HDPR-58 HDPR-57 HDPR-54 HDPR-51 HDPR-50 HDPR-49 HDPR-48 HDPR-47 HDPR-46 HDPR-45 HDPR-44 HDPR-42 HDPR-38 HDPR-37 HDPR-36 HDPR-35 HDPR-34 HDPR-33 HDPR-32 HDPR-29 HDPR-28 HDPR-27 HDPR-26 HDPR-25 HDPR-23 HDPR-22 HDPR-18 HDPR-17 HDPR-16 HDPR-15 HDPR-14 HDPR-13 HDPR-12 HDPR-11 HDPR-10 HDPR-9 HDPR-8 HDPR-7 HDPR-6 HDPR-5 HDPR-4 HDPR-3 HDPR-2 HDPR-1}]
report_drc -name drc_1 -ruledeck ruledeck_1

# Errors should appear now, if any
delete_drc_ruledeck ruledeck_1

# Proceed if there are no errors 

opt_design

place_design 

route_design 

write_checkpoint ./checkpoints/top_mode1_routed.dcp -force 


# PRESERVE STATIC
#open_checkpoint ./checkpoints/top_mode1_routed.dcp

update_design -cell design_1_wrapper/design_1_i/partial_led_test_0/inst/partial_led_test_v1_0_S00_AXI_inst -black_box

lock_design -level routing

write_checkpoint -force ./checkpoints/static_routed.dcp

close_design


# MODE 2


open_checkpoint ./checkpoints/static_routed.dcp

read_checkpoint -cell design_1_wrapper/design_1_i/partial_led_test_0/inst/partial_led_test_v1_0_S00_AXI_inst ./modes/mode2.dcp

write_checkpoint ./checkpoints/top_link_mode2.dcp -force

opt_design

place_design

route_design

write_checkpoint ./checkpoints/top_mode2_routed.dcp -force

write_debug_probes ./bitstreams/probes.ltx -force

close_design

# GENERATE BITSTREAM 
open_checkpoint ./checkpoints/top_mode1_routed.dcp

write_bitstream -force ./bitstreams/mode1.bit

close_design

open_checkpoint ./checkpoints/top_mode2_routed.dcp

write_bitstream -force ./bitstreams/mode2.bit

close_design