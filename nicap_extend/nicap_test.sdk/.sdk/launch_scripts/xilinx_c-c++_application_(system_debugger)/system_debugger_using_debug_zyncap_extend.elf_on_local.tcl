connect -url tcp:127.0.0.1:3121
source C:/SHS/Zynq7000AP_SoC_Ethernet_Packet_Redirection_Design/HW/nicap_extend/nicap_test.sdk/sys_top_hw_platform_1/ps7_init.tcl
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Xilinx TUL 1234-tulA"} -index 0
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Xilinx TUL 1234-tulA" && level==0} -index 1
fpga -file C:/SHS/Zynq7000AP_SoC_Ethernet_Packet_Redirection_Design/HW/nicap_extend/bitstreams/mode1.bit
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Xilinx TUL 1234-tulA"} -index 0
loadhw -hw C:/SHS/Zynq7000AP_SoC_Ethernet_Packet_Redirection_Design/HW/nicap_extend/nicap_test.sdk/sys_top_hw_platform_1/system.hdf -mem-ranges [list {0x40000000 0xbfffffff}]
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Xilinx TUL 1234-tulA"} -index 0
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Xilinx TUL 1234-tulA"} -index 0
dow C:/SHS/Zynq7000AP_SoC_Ethernet_Packet_Redirection_Design/HW/nicap_extend/nicap_test.sdk/zyncap_extend/Debug/zyncap_extend.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Xilinx TUL 1234-tulA"} -index 0
con
