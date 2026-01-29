set_property SRC_FILE_INFO {cfile:C:/Users/win10/Documents/C/SCPI/FPGA_RISC_V_Test/FPGA_Project/Risc_V_SCPI_FPGA_Test/Risc_V_SCPI_FPGA_Test.srcs/constrs_1/new/A7_lite_constrains.xdc rfile:../../../Risc_V_SCPI_FPGA_Test.srcs/constrs_1/new/A7_lite_constrains.xdc id:1} [current_design]
set_property src_info {type:XDC file:1 line:2 export:INPUT save:INPUT read:READ} [current_design]
set_property -dict { PACKAGE_PIN J19 IOSTANDARD LVCMOS33 } [get_ports { clk_50MHz }];
set_property src_info {type:XDC file:1 line:3 export:INPUT save:INPUT read:READ} [current_design]
create_clock -add-name sys_clk-period 20-waveform {0 10} [get_ports { clk_50MHz }];
set_property src_info {type:XDC file:1 line:5 export:INPUT save:INPUT read:READ} [current_design]
set_property -dict { PACKAGE_PIN M18 IOSTANDARD LVCMOS33 } [get_ports { LED1 }];
set_property src_info {type:XDC file:1 line:6 export:INPUT save:INPUT read:READ} [current_design]
set_property -dict { PACKAGE_PIN N18 IOSTANDARD LVCMOS33 } [get_ports { LED2 }];
set_property src_info {type:XDC file:1 line:8 export:INPUT save:INPUT read:READ} [current_design]
set_property -dict { PACKAGE_PIN AA1 IOSTANDARD LVCMOS33 } [get_ports { KEY1 }];
set_property src_info {type:XDC file:1 line:9 export:INPUT save:INPUT read:READ} [current_design]
set_property -dict { PACKAGE_PIN W1 IOSTANDARD LVCMOS33 } [get_ports { KEY2 }];
set_property src_info {type:XDC file:1 line:11 export:INPUT save:INPUT read:READ} [current_design]
set_property -dict { PACKAGE_PIN U2 IOSTANDARD LVCMOS33 } [get_ports { RX }];
set_property src_info {type:XDC file:1 line:12 export:INPUT save:INPUT read:READ} [current_design]
set_property -dict { PACKAGE_PIN V2 IOSTANDARD LVCMOS33 } [get_ports { TX }];
