# CLOCK
set_property -dict { PACKAGE_PIN J19 IOSTANDARD LVCMOS33 } [get_ports { clk_50MHz }];    
create_clock -add-name sys_clk-period 20-waveform {0 10} [get_ports { clk_50MHz }];
# LEDS
set_property -dict { PACKAGE_PIN M18 IOSTANDARD LVCMOS33 } [get_ports { LED1 }];
set_property -dict { PACKAGE_PIN N18 IOSTANDARD LVCMOS33 } [get_ports { LED2 }];
# BUTTONS
set_property -dict { PACKAGE_PIN AA1 IOSTANDARD LVCMOS33 } [get_ports { KEY1 }];
set_property -dict { PACKAGE_PIN W1 IOSTANDARD LVCMOS33 } [get_ports { KEY2 }];
# UART
set_property -dict { PACKAGE_PIN U2 IOSTANDARD LVCMOS33 } [get_ports { RX }];
set_property -dict { PACKAGE_PIN V2 IOSTANDARD LVCMOS33 } [get_ports { TX }];





