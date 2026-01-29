// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2026 Advanced Micro Devices, Inc. All Rights Reserved.
// -------------------------------------------------------------------------------

`timescale 1 ps / 1 ps

(* BLOCK_STUB = "true" *)
module main (
  clk_50MHz,
  KEY2,
  LED1,
  LED2,
  KEY1,
  RX,
  TX
);

  (* X_INTERFACE_IGNORE = "true" *)
  input clk_50MHz;
  (* X_INTERFACE_IGNORE = "true" *)
  input KEY2;
  (* X_INTERFACE_IGNORE = "true" *)
  output [0:0]LED1;
  (* X_INTERFACE_IGNORE = "true" *)
  output [0:0]LED2;
  (* X_INTERFACE_IGNORE = "true" *)
  input KEY1;
  (* X_INTERFACE_IGNORE = "true" *)
  input RX;
  (* X_INTERFACE_IGNORE = "true" *)
  output TX;

  // stub module has no contents

endmodule
