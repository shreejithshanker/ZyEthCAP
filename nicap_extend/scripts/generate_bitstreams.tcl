open_checkpoint ../checkpoints/top_mode1_routed.dcp
write_bitstream -force ../bitstreams/aes.bit
close_design
open_checkpoint ../checkpoints/top_mode2_routed.dcp
write_bitstream -force ../bitstreams/pres.bit
close_design
