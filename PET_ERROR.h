//====include Guards (or #pragma once) Prevent the header from being included multiple times.====
#ifndef PET_ERROR_H_
#define PET_ERROR_H_ 

//Compile options
#define IS_A_TEST_VERSION



//====Other Header Includes====
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>


//====Macro Definitions (#define)====
#define max_description_len 50
#define max_timestamp_len 27
#define ERROR_FIFO_Size 1024


//====Type Definitions (typedef)====


//====Struct, Union, and Enum Declarations====
typedef struct Error_Structure {   // Structure declaration
  uint8_t Timestamp[max_timestamp_len];       // Member (str variable)
  uint32_t Err_num;           // Member (int variable)
  uint8_t Err_description[max_description_len];       // Member (str variable)
} Error_Structure;
// End the structure with a semicolon

typedef struct Error_FIFO {   // Structure declaration
  struct Error_Structure* Read_P;
  struct Error_Structure* Write_P;
  bool Full;
  bool Empty;
  uint32_t Lenght;
  struct Error_Structure Data[ERROR_FIFO_Size];
} Error_FIFO;
// End the structure with a semicolon

//====Global Variable Declarations (with extern)====
uint32_t Error_num;
struct Error_FIFO Error_FIFO_1;


//====Function Declarations (Prototypes)====
void PET_ERROR_init(void);
uint8_t Error_Event(struct Error_FIFO *FIFO, uint8_t* string);
void Init_Error_led(void);
void Set_Error_led(void);
void Error_FIFO_Init(struct Error_FIFO *FIFO);
uint8_t Error_FIFO_Write( struct Error_FIFO *FIFO, struct Error_Structure *Data );
uint8_t Error_FIFO_Read( struct Error_FIFO *FIFO, struct Error_Structure *Data );
void Error_copy(struct Error_Structure* dest, struct Error_Structure* src);

//====include Guards (or #pragma once) Prevent the header from being included multiple times.====
#endif





