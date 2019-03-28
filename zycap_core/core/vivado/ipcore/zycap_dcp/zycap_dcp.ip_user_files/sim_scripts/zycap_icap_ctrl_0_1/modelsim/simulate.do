onbreak {quit -f}
onerror {quit -f}

vsim -voptargs="+acc" -t 1ps -L xil_defaultlib -L xpm -L unisims_ver -L unimacro_ver -L secureip -lib xil_defaultlib xil_defaultlib.zycap_icap_ctrl_0_1 xil_defaultlib.glbl

do {wave.do}

view wave
view structure
view signals

do {zycap_icap_ctrl_0_1.udo}

run -all

quit -force
