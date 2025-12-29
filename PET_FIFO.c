#include "PET_FIFO.h"

void uint8_t_FIFO_Init( struct uint8_t_FIFO *FIFO)
{
	FIFO->Write_P = FIFO->Data;
	FIFO->Read_P = FIFO->Data;
	FIFO->Empty = true;
	FIFO->Full = false;
	FIFO->Lenght = FIFO_Size;
}

uint8_t uint8_t_FIFO_Write( struct uint8_t_FIFO *FIFO, uint8_t *Data )
{
	if(!FIFO->Full)
	{
		*FIFO->Write_P = *Data;
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

uint8_t uint8_t_FIFO_Read( struct uint8_t_FIFO *FIFO, uint8_t *Data )
{
	if(!FIFO->Empty)
	{
		*Data = *FIFO->Read_P;
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

void String_FIFO_Init( struct String_FIFO *FIFO )
{
	FIFO->Write_P = FIFO->Data;
	FIFO->Read_P = FIFO->Data;
	FIFO->Empty = true;
	FIFO->Full = false;
	FIFO->Lenght = FIFO_Size;
}

uint8_t String_FIFO_Write( struct String_FIFO *FIFO, uint8_t *Data )
{
	if(!FIFO->Full)
	{
		for(uint32_t i=0; i<String_Size; i++)
		{
			*(FIFO->Write_P+i) = *(Data+i);
		}
		if(FIFO->Write_P<( FIFO->Data+(String_Size*(FIFO->Lenght-1))))
		{
			FIFO->Write_P = FIFO->Write_P + String_Size;
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

uint8_t String_FIFO_Read( struct String_FIFO *FIFO, uint8_t *Data )
{
	if(!FIFO->Empty)
	{
		for(uint32_t i=0; i<String_Size; i++)
		{
			*(Data+i) = *(FIFO->Read_P+i);
		}
		if(FIFO->Read_P<( FIFO->Data+(String_Size*(FIFO->Lenght-1))))
		{
			FIFO->Read_P = FIFO->Read_P + String_Size;
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