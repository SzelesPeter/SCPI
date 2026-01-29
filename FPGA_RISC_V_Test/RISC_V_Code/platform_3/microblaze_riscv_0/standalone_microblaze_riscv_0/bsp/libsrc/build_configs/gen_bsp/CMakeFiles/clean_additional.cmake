# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "C:\\Users\\win10\\Documents\\C\\SCPI\\FPGA_RISC_V_Test\\RISC_V_Code\\platform_3\\microblaze_riscv_0\\standalone_microblaze_riscv_0\\bsp\\include\\sleep.h"
  "C:\\Users\\win10\\Documents\\C\\SCPI\\FPGA_RISC_V_Test\\RISC_V_Code\\platform_3\\microblaze_riscv_0\\standalone_microblaze_riscv_0\\bsp\\include\\xiltimer.h"
  "C:\\Users\\win10\\Documents\\C\\SCPI\\FPGA_RISC_V_Test\\RISC_V_Code\\platform_3\\microblaze_riscv_0\\standalone_microblaze_riscv_0\\bsp\\include\\xtimer_config.h"
  "C:\\Users\\win10\\Documents\\C\\SCPI\\FPGA_RISC_V_Test\\RISC_V_Code\\platform_3\\microblaze_riscv_0\\standalone_microblaze_riscv_0\\bsp\\lib\\libxiltimer.a"
  )
endif()
