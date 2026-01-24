#include "PET_ERROR.h"


void PET_ERROR_init(void)
{
    Error_num = 0;
    Error_FIFO_Init(&Error_FIFO_1);
    Init_Error_led();
}


uint8_t Error_Event( struct Error_FIFO *FIFO, uint8_t* description)
{
    struct Error_Structure new_error;

	//printf(description);


    // Get current timestamp
    snprintf((char*)new_error.Timestamp, sizeof(new_error.Timestamp), "2024-01-01 12:00:00"); // Placeholder timestamp
    new_error.Err_num = Error_num++;
    strncpy((char*)new_error.Err_description, (char*)description, max_description_len - 1);
    new_error.Err_description[max_description_len - 1] = '\0'; // Ensure null-termination
    if (Error_FIFO_Write(FIFO, &new_error) != 0) {
        // Handle FIFO full error
        return 1;
    }
    return 0;
}

void Init_Error_led(void)
{

}

void Set_Error_led(void)
{
	
}



void Error_FIFO_Init(struct Error_FIFO *FIFO)
{
	FIFO->Write_P = FIFO->Data;
	FIFO->Read_P = FIFO->Data;
	FIFO->Empty = true;
	FIFO->Full = false;
	FIFO->Lenght = ERROR_FIFO_Size;
}

uint8_t Error_FIFO_Write( struct Error_FIFO *FIFO, struct Error_Structure* struct_pointer )
{
	if(!FIFO->Full)
	{
		Error_copy(FIFO->Write_P, struct_pointer);
		if(FIFO->Write_P<( FIFO->Data+FIFO->Lenght-1))
		{
			FIFO->Write_P++;
		}
		else
		{
			FIFO->Write_P = FIFO->Data;
		}
		FIFO->Empty = false;
		if(FIFO->Write_P == FIFO->Read_P)
		{
			FIFO->Full = true;
		}
		return 0;
	}
	return 1;
}

uint8_t Error_FIFO_Read( struct Error_FIFO* FIFO, struct Error_Structure* struct_pointer )
{
	   
	if(!FIFO->Empty)
	{
		Error_copy(struct_pointer, FIFO->Read_P);
		if(FIFO->Read_P<(FIFO->Data+FIFO->Lenght-1))
		{
			FIFO->Read_P++;
		}
		else
		{
			FIFO->Read_P = FIFO->Data;
		}
		FIFO->Full = false;
		if(FIFO->Write_P == FIFO->Read_P)
		{
			FIFO->Empty = true;
		}
		return 0;
	}
	return 1;
}

void Error_copy(struct Error_Structure* dest, struct Error_Structure* src)
{
    memcpy(&(dest->Timestamp), &(src->Timestamp), sizeof(src->Timestamp));
    dest->Err_num = src->Err_num;
    memcpy(dest->Err_description, src->Err_description, sizeof(src->Err_description));
}
