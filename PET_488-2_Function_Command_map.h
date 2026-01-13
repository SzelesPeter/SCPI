//====include Guards (or #pragma once) Prevent the header from being included multiple times.====
#ifndef PET_4882_FUNCTION_COMMAND_MAP_H_
#define PET_4882_FUNCTION_COMMAND_MAP_H_

//====Other Header Includes====
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "PET_ERROR.h"
#include "PET_488-2.h"

//====Macro Definitions (#define)====


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


//====Function Declarations (Prototypes)====
// Common Commands
uint8_t PET_4882_IDN_query(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5);
uint8_t PET_4882_RST_command(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5);
uint8_t PET_4882_TST_query(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5);

//Simple Commands
uint8_t PET_4882_LOL_query(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5);

//Compound Commands
uint8_t PET_4882_SYSTem_ERRor_query(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5);
uint8_t PET_4882_SYSTem_VERsion_query(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5);
uint8_t PET_4882_SYSTem_MESsage(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5);
uint8_t PET_4882_SYSTem_NUMber(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5);

//====Global Variable Declarations (with extern)====



const struct program_mnemonic root_mnemonic = {
    .mnemonic_name = "ROOT",
    .parent = 0,
    .data_types = {NON},
    .function = 0,
    .is_end_mnemonic = false
};

// Common Commands

const struct program_mnemonic IDN_mnemonic = {
    .mnemonic_name = "IDN?",
    .parent = &root_mnemonic,
    .data_types = {NON},
    .function = &PET_4882_IDN_query,
    .is_end_mnemonic = true
};

const struct program_mnemonic RST_mnemonic = {
    .mnemonic_name = "RST",
    .parent = &root_mnemonic,
    .data_types = {NON},
    .function = &PET_4882_RST_command,
    .is_end_mnemonic = true
};

const struct program_mnemonic TST_mnemonic = {
    .mnemonic_name = "TST?",
    .parent = &root_mnemonic,
    .data_types = {NON},
    .function = &PET_4882_TST_query,
    .is_end_mnemonic = true
};

const struct program_mnemonic *common_command_mnemonics[] = {
    &IDN_mnemonic,
    &RST_mnemonic,
    &TST_mnemonic,
    0
};

//Simple Commands
const struct program_mnemonic LOL_mnemonic = {
    .mnemonic_name = "LOL?",
    .parent = &root_mnemonic,
    .data_types = {NON},
    .function = &PET_4882_LOL_query,
    .is_end_mnemonic = true
};

const struct program_mnemonic *simple_command_mnemonics[] = {
    &LOL_mnemonic,
    0
};

//Compound Commands

const struct program_mnemonic SYSTem_mnemonic = {
    .mnemonic_name = "SYSTem",
    .parent = &root_mnemonic,
    .data_types = {NON},
    .function = 0,
    .is_end_mnemonic = false
};

const struct program_mnemonic SYSTem_ERRor_query_mnemonic = {
    .mnemonic_name = "ERRor?",
    .parent = &SYSTem_mnemonic,
    .data_types = {NON},
    .function = &PET_4882_SYSTem_ERRor_query,
    .is_end_mnemonic = true
};

const struct program_mnemonic SYSTem_VERsion_query_mnemonic = {
    .mnemonic_name = "VERsion?",
    .parent = &SYSTem_mnemonic,
    .data_types = {NON},
    .function = &PET_4882_SYSTem_VERsion_query,
    .is_end_mnemonic = true
};

const struct program_mnemonic SYSTem_MESsage_mnemonic = {
    .mnemonic_name = "MESsage",
    .parent = &SYSTem_mnemonic,
    .data_types = {STRING_PROGRAM_DATA, NON},
    .function = &PET_4882_SYSTem_MESsage,
    .is_end_mnemonic = true
};

const struct program_mnemonic SYSTem_NUMber_mnemonic = {
    .mnemonic_name = "NUMber",
    .parent = &SYSTem_mnemonic,
    .data_types = {DECIMAL_NUMERIC_PROGRAM_DATA, NON},
    .function = &PET_4882_SYSTem_NUMber,
    .is_end_mnemonic = true
};

const struct program_mnemonic *compound_command_mnemonics[] = {
    &SYSTem_mnemonic,
    &SYSTem_ERRor_query_mnemonic,
    &SYSTem_VERsion_query_mnemonic,
    &SYSTem_MESsage_mnemonic,
    &SYSTem_NUMber_mnemonic,
    0
};

//====include Guards (or #pragma once) Prevent the header from being included multiple times.====
#endif