//====include Guards (or #pragma once) Prevent the header from being included multiple times.====
#ifndef PET_4882_H_
#define PET_4882_H_

//Compile options
//#define IS_A_TEST_VERSION
//#define ENABLE_ERROR_HANDLING

//====Other Header Includes====
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
//#include <math.h>

#include "xgpio.h"
#include "xparameters.h"
#include "xil_printf.h"

#ifdef ENABLE_ERROR_HANDLING
  #include "PET_ERROR.h"
  #include "PET_ERROR.c"
#endif

//====Macro Definitions (#define)====
//#define input_buffer_size 1024
//#define output_buffer_size 1024
//#define function_buffer_size 256
#define Input_buffer_Size 1024
#define Output_buffer_Size 1024

//====Type Definitions (typedef)====

//====Type Definitions (typedef)====
union data_union { // needs to be above function_ptr_4882 typedef
    uint32_t i;
    float f;
    char string[256];
};

typedef uint8_t (*function_ptr_4882)(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5);

typedef enum {
    NON,
    CHARACTER_PROGRAM_DATA,
    DECIMAL_NUMERIC_PROGRAM_DATA,
    SUFFIX_PROGRAM_DATA,
    NONDECIMAL_NUMERIC_PROGRAM_DATA,
    STRING_PROGRAM_DATA,
    ARBITRARY_BLOCK_PROGRAM_DATA,
    EXPRESSION_PROGRAM_DATA
} PROGRAM_DATA;


//====Struct, Union, and Enum Declarations====


struct program_mnemonic {   // Structure declaration
  uint8_t mnemonic_name[13];
  struct program_mnemonic* parent;
  PROGRAM_DATA data_types[10];
  function_ptr_4882 function;
  bool is_end_mnemonic;
}; // End the structure with a semicolon


/*
d.i = 10;
printf("%d\n", d.i);

d.f = 3.14;
printf("%f\n", d.f);  // reading i now is undefined behavior
*/


//====Struct, Union, and Enum Declarations====
typedef struct program_message_terminator_FIFO {   // Structure declaration
  uint8_t** Read_P;
  uint8_t** Write_P;
  bool Full;
  bool Empty;
  uint32_t Lenght;
  uint8_t* Data[32];
}program_message_terminator_FIFO; // End the structure with a semicolon

typedef struct Input_FIFO {   // Structure declaration
  uint8_t* Read_P;
  uint8_t* Write_P;
  bool Full;
  bool Empty;
  uint32_t Lenght;
  uint8_t Data[Input_buffer_Size];
}Input_FIFO; // End the structure with a semicolon

typedef struct Output_FIFO {   // Structure declaration
  uint8_t* Read_P;
  uint8_t* Write_P;
  bool Full;
  bool Empty;
  uint32_t Lenght;
  uint8_t Data[Output_buffer_Size];
}Output_FIFO; // End the structure with a semicolon


typedef struct PET_488_2
{
  struct program_message_terminator_FIFO program_message_terminator_buffer;
  struct Input_FIFO input_buffer;
  struct Output_FIFO output_buffer;
  struct program_mnemonic* last_command_root;
  struct program_mnemonic* function_buffer[8];
  uint8_t function_buffer_index;
  union data_union data_buffer[8][5];
  uint8_t* read_input_buffer_until;
  struct program_mnemonic* common_command_mnemonics[10];
  struct program_mnemonic* simple_command_mnemonics[10];
  struct program_mnemonic* compound_command_mnemonics[10];
  struct program_mnemonic root_mnemonic;
  struct program_mnemonic IDN_mnemonic;
  struct program_mnemonic RST_mnemonic;
  struct program_mnemonic SYSTem_mnemonic;
  struct program_mnemonic SYSTem_Ld_mnemonic;
} PET_488_2;


//====Global Variable Declarations (with extern)====



//====Function Declarations (Prototypes)====
void PET_4882_Init(PET_488_2* pet_488_2_instance);
void PET_4882_Recive_character(PET_488_2* pet_488_2_instance, uint8_t character);
void PET_4882_Send_response(PET_488_2* pet_488_2_instance, uint8_t* string);
void PET_4882_Process(PET_488_2* pet_488_2_instance);
uint8_t add_command_function_to_buffer(PET_488_2* pet_488_2_instance, struct program_mnemonic** found_command);
uint8_t exacute_command_function(PET_488_2* pet_488_2_instance);
uint8_t Decode_simple_command_program_header(PET_488_2* pet_488_2_instance, struct program_mnemonic** current_command_root, uint8_t** ptr);
uint8_t Decode_common_command_program_header(PET_488_2* pet_488_2_instance, struct program_mnemonic** current_command_root, uint8_t** ptr);
uint8_t Decode_compound_command_program_header(PET_488_2* pet_488_2_instance, struct program_mnemonic** current_command_root, uint8_t** ptr);
void Make_string_uppercase(uint8_t* string);
uint8_t Find_maching_mnemonic(struct program_mnemonic** mnemonics_list_ptr, struct program_mnemonic** current_command_root, uint8_t* input_mnemonic);
uint8_t Decode_program_data(PET_488_2* pet_488_2_instance, struct program_mnemonic** current_command_root, uint8_t** ptr);
uint8_t Decode_character_program_data(uint8_t** ptr, uint8_t* output_string);
uint8_t Decode_decimal_numeric_program_data(uint8_t** ptr, float* output_value);
uint8_t Decode_suffix_program_data(uint8_t** ptr, float* output_value);
uint8_t Decode_nondecmial_numeric_program_data(uint8_t** ptr, int32_t* output_value);
uint8_t Decode_string_program_data(PET_488_2* pet_488_2_instance, uint8_t** ptr, uint8_t* output_string);
uint8_t Decode_arbitrary_block_program_data(PET_488_2* pet_488_2_instance, uint8_t** ptr, uint8_t* output_string);
uint8_t Decode_expression_program_data(uint8_t** ptr, float* output_value);
// Utility Functions
void Make_string_uppercase(uint8_t* string);
bool Is_mnemonic_character(uint8_t ptr);
bool Is_digit(uint8_t ptr);
void Step_thru_white_space(uint8_t** ptr);
bool Is_white_space(uint8_t** ptr);
bool Is_program_mnemonic_separator(uint8_t** ptr);
void Step_thru_program_mnemonic_separator( uint8_t** ptr );
void Step_thru_program_message_terminator( uint8_t** ptr );
bool Is_program_message_terminator( uint8_t** ptr );
bool Is_program_header_separator( uint8_t** ptr );
void Step_thru_program_header_separator( uint8_t** ptr );
bool Is_program_data_separator( uint8_t** ptr );
void Step_thru_program_data_separator( uint8_t** ptr );
bool Is_program_message_unit_separator( uint8_t** ptr );
void Step_thru_program_message_unit_separator( uint8_t** ptr );
// FIFO Functions
void Program_Message_Terminator_FIFO_Init( struct program_message_terminator_FIFO *FIFO);
uint8_t Program_Message_Terminator_FIFO_Write( struct program_message_terminator_FIFO *FIFO, uint8_t **Data );
uint8_t Program_Message_Terminator_FIFO_Read( struct program_message_terminator_FIFO *FIFO, uint8_t **Data );
void Input_FIFO_Init( struct Input_FIFO *FIFO);
uint8_t Input_FIFO_Write( struct Input_FIFO *FIFO, uint8_t *Data );
uint8_t Input_FIFO_Read( struct Input_FIFO *FIFO, uint8_t *Data );
uint8_t Input_FIFO_Move_pointer( PET_488_2* pet_488_2_instance, struct Input_FIFO *FIFO, uint8_t **ptr );
void Output_FIFO_Init( struct Output_FIFO *FIFO);
uint8_t Output_FIFO_Write( struct Output_FIFO *FIFO, uint8_t *Data );
uint8_t Output_FIFO_Read( struct Output_FIFO *FIFO, uint8_t *Data );









//====Function Declarations (Prototypes)====
// Common Commands
uint8_t PET_4882_IDN_query(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5);
uint8_t PET_4882_RST_command(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5);

//Simple Commands

//Compound Commands
uint8_t PET_4882_SYSTem_Ld(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5);

void PET_4882_function_init(PET_488_2* pet_488_2_instance);
//====Global Variable Declarations (with extern)====



//struct program_mnemonic root_mnemonic;

// Common Commands

//struct program_mnemonic IDN_mnemonic;
//struct program_mnemonic RST_mnemonic;




//Simple Commands




//Compound Commands

//struct program_mnemonic SYSTem_mnemonic;

//struct program_mnemonic SYSTem_Ld_mnemonic;



double power(double base, int exp);


//====include Guards (or #pragma once) Prevent the header from being included multiple times.====
#endif