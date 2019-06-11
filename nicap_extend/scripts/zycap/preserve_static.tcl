
#open_checkpoint ./checkpoints/top_mode1_routed.dcp

update_design -cell design_1_wrapper/design_1_i/partial_led_test_0/inst/partial_led_test_v1_0_S00_AXI_inst -black_box

lock_design -level routing

write_checkpoint -force ./checkpoints/static_routed.dcp

close_design