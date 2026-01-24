#include "PET_488-2_Function_Command_map.h"
#include "stm32f7xx_hal.h"

// Common Commands
uint8_t PET_4882_IDN_query(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5)
{
    uint8_t response[] = "MyInstrument,Model1234,Serial0001,1.0\n";
    //PET_4882_Send_response(response);
    //printf("PET_4882_IDN_query\n");
    return 0;
}

uint8_t PET_4882_RST_command(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5)
{
    //PET_4882_Init();
    //printf("PET_4882_RST_command\n");
    return 0;
}

uint8_t PET_4882_TST_query(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5)
{
    uint8_t response[] = "0\n"; // Self-test passed
    //PET_4882_Send_response(response);
    //printf("PET_4882_TST_query\n");
    return 0;
}

//Simple Commands
uint8_t PET_4882_LOL_query(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5)
{
    uint8_t response[] = "0\n"; // Self-test passed
    //PET_4882_Send_response(response);
    //printf("PET_4882_LOL_query\n");
    return 0;
}

//Compound Commands

uint8_t PET_4882_SYSTem_ERRor_query(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5)
{
    uint8_t response[] = "0,No error\n";
    //PET_4882_Send_response(response);
    //printf("PET_4882_SYSTem_ERRor_query\n");
    return 0;
}

uint8_t PET_4882_SYSTem_VERsion_query(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5)
{
    uint8_t response[] = "Firmware Version 1.0\n";
    //PET_4882_Send_response(response);
    //printf("PET_4882_SYSTem_VERsion_query\n");
    return 0;
}

uint8_t PET_4882_SYSTem_MESsage(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5)
{
    //uint8_t response[] = "Firmware Version 1.0\n";
    //PET_4882_Send_response(response);
    uint8_t message[500];
    sprintf((char*)message, "ET_4882_SYSTem_MESsage_query %s\n", var_1->string);
    //printf("%s", message);
    return 0;
}

uint8_t PET_4882_SYSTem_NUMber(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5)
{
    //uint8_t response[] = "Firmware Version 1.0\n";
    //PET_4882_Send_response(response);
    uint8_t message[256];
    sprintf((char*)message, "ET_4882_SYSTem_NUMber_query %f\n", var_1->f);
    //printf("%s", message);
    return 0;
}

uint8_t PET_4882_OUTput_LD1(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5)
{
    //uint8_t response[] = "Firmware Version 1.0\n";
    //PET_4882_Send_response(response);

	if(*var_1->f)
	{
		HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 0);
	}

    uint8_t message[256];
    sprintf((char*)message, "ET_4882_SYSTem_NUMber_query %f\n", var_1->f);
    //printf("%s", message);
    return 0;
}

uint8_t PET_4882_OUTput_LD2(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5)
{
    //uint8_t response[] = "Firmware Version 1.0\n";
    //PET_4882_Send_response(response);

	if(*var_1->f)
	{
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);
	}

    uint8_t message[256];
    sprintf((char*)message, "ET_4882_SYSTem_NUMber_query %f\n", var_1->f);
    //printf("%s", message);
    return 0;
}

uint8_t PET_4882_OUTput_LD3(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5)
{
    //uint8_t response[] = "Firmware Version 1.0\n";
    //PET_4882_Send_response(response);

	if(*var_1->f)
	{
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, 0);
	}

    uint8_t message[256];
    sprintf((char*)message, "ET_4882_SYSTem_NUMber_query %f\n", var_1->f);
    //printf("%s", message);
    return 0;
}
