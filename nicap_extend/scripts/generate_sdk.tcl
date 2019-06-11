open_project ../nicap_test.xpr
write_hwdef -file ../software/system_wrapper.hdf -force
launch_sdk -workspace ../software -hwspec ../software/system_wrapper.hdf
