//====include Guards (or #pragma once) Prevent the header from being included multiple times.====
#ifndef PET_4882_H_
#define PET_4882_H_

//====Other Header Includes====
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "PET_ERROR.h"
#include "PET_ERROR.c"
#include "PET_488-2_Function_Command_map.h"
#include "PET_488-2_Function_Command_map.c"

//====Macro Definitions (#define)====
//#define input_buffer_size 1024
//#define output_buffer_size 1024
//#define function_buffer_size 256
#define Input_buffer_Size 1024
#define Output_buffer_Size 1024

//====Type Definitions (typedef)====

//====Struct, Union, and Enum Declarations====
struct Input_FIFO {   // Structure declaration
  uint8_t* Read_P;
  uint8_t* Write_P;
  bool Full;
  bool Empty;
  uint32_t Lenght;
  uint8_t Data[FIFO_Size];
}; // End the structure with a semicolon

struct Output_FIFO {   // Structure declaration
  uint8_t* Read_P;
  uint8_t* Write_P;
  bool Full;
  bool Empty;
  uint32_t Lenght;
  uint8_t Data[FIFO_Size];
}; // End the structure with a semicolon


//====Global Variable Declarations (with extern)====
    struct Input_FIFO input_buffer;
    struct Output_FIFO output_buffer;
    uint8_t New_line_received = 0;


//====Function Declarations (Prototypes)====
void PET_4882_Init(void);
void PET_4882_Recive_character(uint8_t character);
void PET_4882_Send_response(uint8_t* string);
void PET_4882_Process(void);
uint8_t exacute_command_function(struct program_mnemonic* found_command);
uint8_t Decode_simple_command_program_header(struct program_mnemonic** current_command_root, uint8_t** ptr);
uint8_t Decode_common_command_program_header(struct program_mnemonic** current_command_root, uint8_t** ptr);
uint8_t Decode_compound_command_program_header(struct program_mnemonic** current_command_root, uint8_t** ptr);
void Make_string_uppercase(uint8_t* string);
uint8_t Find_maching_mnemonic(const struct program_mnemonic** mnemonics_list_ptr, struct program_mnemonic** current_command_root, uint8_t* input_mnemonic);
// Utility Functions
void Make_string_uppercase(uint8_t* string);
bool Is_mnemonic_character(uint8_t ptr);
void Step_thru_white_space(uint8_t** ptr);
bool Is_white_space(uint8_t** ptr);
void Step_thru_program_message_terminator(uint8_t** ptr);
bool Is_program_message_terminator(uint8_t** ptr);
bool Is_program_header_separator(uint8_t** ptr);
bool Is_program_data_separator(uint8_t** ptr);
void Step_thru_program_data_separator(uint8_t** ptr);
bool Is_program_message_unit_separator(uint8_t** ptr);
void Step_thru_program_message_unit_separator(uint8_t** ptr);
// FIFO Functions
void Input_FIFO_Init( struct Input_FIFO *FIFO);
uint8_t Input_FIFO_Write( struct Input_FIFO *FIFO, uint8_t *Data );
uint8_t Input_FIFO_Read( struct Input_FIFO *FIFO, uint8_t *Data );
void Output_FIFO_Init( struct Output_FIFO *FIFO);
uint8_t Output_FIFO_Write( struct Output_FIFO *FIFO, uint8_t *Data );
uint8_t Output_FIFO_Read( struct Output_FIFO *FIFO, uint8_t *Data );



//====include Guards (or #pragma once) Prevent the header from being included multiple times.====
#endif