open_project ../nicap_test.xpr
synth_design -top sys_top -part xc7z020clg400-1
set_property HD.RECONFIGURABLE true [get_cells design_1_wrapper/design_1_i/crypto_top_0]
update_design -cell design_1_wrapper/design_1_i/crypto_top_0 -black_box
