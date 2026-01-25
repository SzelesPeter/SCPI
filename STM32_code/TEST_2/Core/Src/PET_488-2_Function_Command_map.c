#include "PET_488-2_Function_Command_map.h"
#include "stm32f7xx_hal.h"
#include "main.h"

extern UART_HandleTypeDef huart3;

void PET_4882_function_init()
{
	strcpy(root_mnemonic.mnemonic_name, "ROOT");
	root_mnemonic.parent = 0;
	memset(root_mnemonic.data_types, NON, 10);
	root_mnemonic.function = 0;
	root_mnemonic.is_end_mnemonic = false;

	strcpy(IDN_mnemonic.mnemonic_name, "IDN?");
	IDN_mnemonic.parent = &root_mnemonic;
	memset(IDN_mnemonic.data_types, NON, 10);
	IDN_mnemonic.function = &PET_4882_IDN_query;
	IDN_mnemonic.is_end_mnemonic = true;

	strcpy(RST_mnemonic.mnemonic_name, "RST");
	RST_mnemonic.parent = &root_mnemonic;
	memset(IDN_mnemonic.data_types, NON, 10);
	RST_mnemonic.function = &PET_4882_RST_command;
	RST_mnemonic.is_end_mnemonic = true;

	strcpy(SYSTem_mnemonic.mnemonic_name, "SYSTem");
	SYSTem_mnemonic.parent = &root_mnemonic;
	memset(SYSTem_mnemonic.data_types, NON, 10);
	SYSTem_mnemonic.function = 0;
	SYSTem_mnemonic.is_end_mnemonic = false;

	strcpy(SYSTem_Ld_mnemonic.mnemonic_name, "Ld");
	SYSTem_Ld_mnemonic.parent = &SYSTem_mnemonic;
	memset(SYSTem_Ld_mnemonic.data_types, NON, 10);
	SYSTem_Ld_mnemonic.data_types[0] = CHARACTER_PROGRAM_DATA;
	SYSTem_Ld_mnemonic.data_types[1] = CHARACTER_PROGRAM_DATA;
	SYSTem_Ld_mnemonic.function = &PET_4882_SYSTem_Ld;
	SYSTem_Ld_mnemonic.is_end_mnemonic = true;

	common_command_mnemonics[0] = &IDN_mnemonic;
	common_command_mnemonics[1] = &RST_mnemonic;
	common_command_mnemonics[2] = 0;

	simple_command_mnemonics[0] = 0;

	compound_command_mnemonics[0] = &SYSTem_mnemonic;
	compound_command_mnemonics[1] = &SYSTem_Ld_mnemonic;
	compound_command_mnemonics[2] = 0;
}






// Common Commands
uint8_t PET_4882_IDN_query(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5)
{
    uint8_t response[] = "MyInstrument,Model1234,Serial0001,1.0\n";
    //PET_4882_Send_response(response);
    HAL_UART_Transmit(&huart3, response, 40, 1000);
    //printf("PET_4882_IDN_query\n");
    return 0;
}

uint8_t PET_4882_RST_command(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5)
{
    //PET_4882_Init();
    //printf("PET_4882_RST_command\n");
    return 0;
}



//Simple Commands


//Compound Commands

uint8_t PET_4882_SYSTem_Ld(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5)
{
    //uint8_t response[] = "Firmware Version 1.0\n";
    //PET_4882_Send_response(response);
    //uint8_t message[256];
    //sprintf((char*)message, "ET_4882_SYSTem_NUMber_query %f\n", var_1->f);
    //printf("%s", message);
	if(var_1->i == '1')
	{
		if(var_2->i == '0')
		{
			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 0);
		}
		if(var_2->i == '1')
		{
			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
		}
	}
	if(var_1->i == '2')
		{
			if(var_2->i == '0')
			{
				HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);
			}
			if(var_2->i == '1')
			{
				HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
			}
		}
	if(var_1->i == '3')
		{
			if(var_2->i == '0')
			{
				HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, 0);
			}
			if(var_2->i == '1')
			{
				HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, 1);
			}
		}




    return 0;
}
