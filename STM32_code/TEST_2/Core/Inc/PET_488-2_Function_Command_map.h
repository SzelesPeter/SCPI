//====include Guards (or #pragma once) Prevent the header from being included multiple times.====
#ifndef PET_4882_FUNCTION_COMMAND_MAP_H_
#define PET_4882_FUNCTION_COMMAND_MAP_H_




//====Other Header Includes====
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "PET_ERROR.h"


//====Macro Definitions (#define)====


//====Type Definitions (typedef)====
typedef union data_union { // needs to be above function_ptr_4882 typedef
    uint32_t i;
    float f;
    char string[256];
}data_union1;

#include "PET_488-2.h"

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


//====Function Declarations (Prototypes)====
// Common Commands
uint8_t PET_4882_IDN_query(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5);
uint8_t PET_4882_RST_command(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5);

//Simple Commands

//Compound Commands
uint8_t PET_4882_SYSTem_Ld(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5);

void PET_4882_function_init();
//====Global Variable Declarations (with extern)====



struct program_mnemonic root_mnemonic;

// Common Commands

 struct program_mnemonic IDN_mnemonic;
 struct program_mnemonic RST_mnemonic;


 struct program_mnemonic *common_command_mnemonics[10];

//Simple Commands


struct program_mnemonic *simple_command_mnemonics[10];

//Compound Commands

struct program_mnemonic SYSTem_mnemonic;

struct program_mnemonic SYSTem_Ld_mnemonic;

const struct program_mnemonic *compound_command_mnemonics[10];

//====include Guards (or #pragma once) Prevent the header from being included multiple times.====
#endif
