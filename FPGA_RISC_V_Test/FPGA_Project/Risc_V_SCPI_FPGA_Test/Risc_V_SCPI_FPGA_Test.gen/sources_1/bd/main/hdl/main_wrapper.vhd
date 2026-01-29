--Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
--Copyright 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.
----------------------------------------------------------------------------------
--Tool Version: Vivado v.2024.2 (win64) Build 5239630 Fri Nov 08 22:35:27 MST 2024
--Date        : Thu Jan 29 07:02:41 2026
--Host        : DESKTOP-I3BS7O3 running 64-bit major release  (build 9200)
--Command     : generate_target main_wrapper.bd
--Design      : main_wrapper
--Purpose     : IP block netlist
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity main_wrapper is
  port (
    KEY1 : in STD_LOGIC;
    KEY2 : in STD_LOGIC;
    LED1 : out STD_LOGIC_VECTOR ( 0 to 0 );
    LED2 : out STD_LOGIC_VECTOR ( 0 to 0 );
    RX : in STD_LOGIC;
    TX : out STD_LOGIC;
    clk_50MHz : in STD_LOGIC
  );
end main_wrapper;

architecture STRUCTURE of main_wrapper is
  component main is
  port (
    clk_50MHz : in STD_LOGIC;
    KEY2 : in STD_LOGIC;
    LED1 : out STD_LOGIC_VECTOR ( 0 to 0 );
    LED2 : out STD_LOGIC_VECTOR ( 0 to 0 );
    KEY1 : in STD_LOGIC;
    RX : in STD_LOGIC;
    TX : out STD_LOGIC
  );
  end component main;
begin
main_i: component main
     port map (
      KEY1 => KEY1,
      KEY2 => KEY2,
      LED1(0) => LED1(0),
      LED2(0) => LED2(0),
      RX => RX,
      TX => TX,
      clk_50MHz => clk_50MHz
    );
end STRUCTURE;
