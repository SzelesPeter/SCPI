#ifndef INC_PETER_FIFO_H_
#define INC_PETER_FIFO_H_

#include "stdint.h"
#include "string.h"
#include "stdio.h"
#include "stdbool.h"

#define FIFO_Size 1024
#define String_Size 51

struct uint8_t_FIFO {   // Structure declaration
  uint8_t* Read_P;
  uint8_t* Write_P;
  bool Full;
  bool Empty;
  uint32_t Lenght;
  uint8_t Data[FIFO_Size];
}; // End the structure with a semicolon

struct String_FIFO {   // Structure declaration
  uint8_t* Read_P;
  uint8_t* Write_P;
  bool Full;
  bool Empty;
  uint32_t Lenght;
  uint8_t Data[FIFO_Size][String_Size];
}; // End the structure with a semicolon

#endif /* INC_PETER_FIFO_H_ */


void uint8_t_FIFO_Init( struct uint8_t_FIFO *FIFO );
uint8_t uint8_t_FIFO_Write( struct uint8_t_FIFO *FIFO, uint8_t *Data );
uint8_t uint8_t_FIFO_Read( struct uint8_t_FIFO *FIFO, uint8_t *Data );
void String_FIFO_Init( struct String_FIFO *FIFO );
uint8_t String_FIFO_Write( struct String_FIFO *FIFO, uint8_t *Data );
uint8_t String_FIFO_Read( struct String_FIFO *FIFO, uint8_t *Data );