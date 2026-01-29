/******************************************************************************
* Copyright (C) 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#include "time.h"
#include "xgpio.h"
#include "xparameters.h"

#include "PET.h"
XGpio Gpio;

int main()
{
    init_platform();

    PET_488_2 PET_488_2_1;
    print("Hello World\n\r");
    print("Successfully ran Hello World application");
    
    XGpio_Initialize(&Gpio, XPAR_AXI_GPIO_0_BASEADDR);
    XGpio_SetDataDirection(&Gpio, 1, 0xFFFFFFFD);

    print("Hello World\n\r");
    print("Successfully ran Hello World application");
    char data;
    PET_4882_Init(&PET_488_2_1);
    while(1)
    {
        data = inbyte();
        PET_4882_Recive_character(&PET_488_2_1, data);
        PET_4882_Process(&PET_488_2_1);
        
    }
    cleanup_platform();
    return 0;
}