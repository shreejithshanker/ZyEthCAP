open_checkpoint ../checkpoints/top_aes_routed.dcp
write_bitstream -force ../bitstreams/aes.bit
close_design
open_checkpoint ../checkpoints/top_pres_routed.dcp
write_bitstream -force ../bitstreams/pres.bit
close_design
