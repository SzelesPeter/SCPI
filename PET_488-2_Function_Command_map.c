#include "PET_488-2_Function_Command_map.h"


// Common Commands
uint8_t PET_4882_IDN_query(void *var_1, void *var_2, void *var_3, void *var_4, void *var_5)
{
    uint8_t response[] = "MyInstrument,Model1234,Serial0001,1.0\n";
    //PET_4882_Send_response(response);
    printf("PET_4882_IDN_query");
    return 0;
}

uint8_t PET_4882_RST_command(void *var_1, void *var_2, void *var_3, void *var_4, void *var_5)
{
    //PET_4882_Init();
    printf("PET_4882_RST_command");
    return 0;
}

uint8_t PET_4882_TST_query(void *var_1, void *var_2, void *var_3, void *var_4, void *var_5)
{
    uint8_t response[] = "0\n"; // Self-test passed
    //PET_4882_Send_response(response);
    printf("PET_4882_TST_query");
    return 0;
}

//Simple Commands
uint8_t PET_4882_LOL_query(void *var_1, void *var_2, void *var_3, void *var_4, void *var_5)
{
    uint8_t response[] = "0\n"; // Self-test passed
    //PET_4882_Send_response(response);
    printf("PET_4882_LOL_query");
    return 0;
}

//Compound Commands

uint8_t PET_4882_SYSTem_ERRor_query(void *var_1, void *var_2, void *var_3, void *var_4, void *var_5)
{
    uint8_t response[] = "0,No error\n";
    //PET_4882_Send_response(response);
    printf("PET_4882_SYSTem_ERRor_query");
    return 0;
}

uint8_t PET_4882_SYSTem_VERsion_query(void *var_1, void *var_2, void *var_3, void *var_4, void *var_5)
{
    uint8_t response[] = "Firmware Version 1.0\n";
    //PET_4882_Send_response(response);
    printf("PET_4882_SYSTem_VERsion_query");
    return 0;
}

