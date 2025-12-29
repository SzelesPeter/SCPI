#include "PET_488-2.h"


uint8_t main(void) //ONLY FOR TESTING
{
    printf("START\n");
    PET_4882_Init();
    Error_FIFO_Init(&Error_FIFO_1);
    PET_4882_Recive_character(' ');
    PET_4882_Recive_character('l');
    PET_4882_Recive_character('o');
    PET_4882_Recive_character('L');
    PET_4882_Recive_character('?');
    PET_4882_Recive_character('\r');
    PET_4882_Recive_character('\n');
    PET_4882_Process();
    return 0;
}


void PET_4882_Init(void)
{
    Input_FIFO_Init(&input_buffer);
    Output_FIFO_Init(&output_buffer);
    New_line_received = 0;
    last_command_root = &root_mnemonic;
}

void PET_4882_Recive_character(uint8_t character)
{
    if (Input_FIFO_Write(&input_buffer, &character) != 0) {
        Error_Event(&Error_FIFO_1, (uint8_t *)"Input buffer overflow");
    }
    if((character == '\n') || (character == '\r'))
    {
        New_line_received++;
    } 
} 


void PET_4882_Send_response(uint8_t* string)
{
    printf(string);
    while(*string)
    {
        if (Output_FIFO_Write(&output_buffer, string) != 0) {
            Error_Event(&Error_FIFO_1, (uint8_t *)"Output buffer overflow");
            return;
        }
        string++;
    }
}

void PET_4882_Process(void)
{
    if(New_line_received)
    {
        uint8_t* FIFO_ptr = input_buffer.Read_P;
        bool more_message_units = false;
        struct program_mnemonic* current_command_root = &root_mnemonic;

        do // Process all received message units
        {
            current_command_root = &root_mnemonic; //reset current cooand pointer to root mnemonic

            Step_thru_white_space(&FIFO_ptr);
            
            if((*FIFO_ptr == '\n') || (*FIFO_ptr == '\r'))
            {
                Error_Event(&Error_FIFO_1, (uint8_t *)"Empty command received");
                New_line_received--;
                input_buffer.Read_P = FIFO_ptr + 1;   // Move read pointer past the termination character(s)
                return;
            }
            else if ((*FIFO_ptr >= 'A' && *FIFO_ptr <= 'Z') || (*FIFO_ptr >= 'a' && *FIFO_ptr <= 'z'))
            {
                if(Decode_simple_command_program_header(&current_command_root, &FIFO_ptr) != 0)
                {
                    Error_Event(&Error_FIFO_1, (uint8_t *)"Invalid simple command received");
                    PET_4882_Init();
                    return;
                }
            }
            else if((*FIFO_ptr == '*'))
            {
                if(Decode_common_command_program_header(&current_command_root, &FIFO_ptr) != 0)
                {
                    Error_Event(&Error_FIFO_1, (uint8_t *)"Invalid common command received");
                    PET_4882_Init();
                    return;
                }
            }
            else if((*FIFO_ptr == ':'))
            {
                if(Decode_compound_command_program_header(&current_command_root, &FIFO_ptr) != 0)
                {
                    Error_Event(&Error_FIFO_1, (uint8_t *)"Invalid compound command received");
                    PET_4882_Init();
                    return;
                }
            }
            else
            {
                Error_Event(&Error_FIFO_1, (uint8_t *)"Invalid command received");
                PET_4882_Init();
                return;
            }
            if(Is_white_space(&FIFO_ptr))
            {
                Step_thru_white_space(&FIFO_ptr);
            }
            if(Is_program_message_terminator(&FIFO_ptr))
            {
                Step_thru_program_message_terminator(&FIFO_ptr);
                exacute_command_function(current_command_root);
                more_message_units = false;
                New_line_received--;
                input_buffer.Read_P = FIFO_ptr + 1;   // Move read pointer past the termination character(s)
            }
            else if(Is_program_message_unit_separator(&FIFO_ptr))
            {
                Step_thru_program_message_unit_separator(&FIFO_ptr);
                exacute_command_function(current_command_root);
                more_message_units = true;
            }
            else
            {
                Error_Event(&Error_FIFO_1, (uint8_t *)"Invalid termination of command");
                PET_4882_Init();
            }
            
        }
        while(more_message_units); // Is there a PROGRAM MESSAGE UNIT SEPARATOR (';') present?
    }
}

uint8_t exacute_command_function(struct program_mnemonic* found_command)
{
    if(found_command->function(0,0,0,0,0))
    {
        Error_Event(&Error_FIFO_1, (uint8_t *)"Cant process command");
    }
}

uint8_t Decode_simple_command_program_header(struct program_mnemonic** current_command_root, uint8_t** ptr)
{
    uint8_t tmp_mnemonic[13] = {0};
    uint8_t index = 0;

    while(Is_mnemonic_character(**ptr))
    {
        if(index < 12) // Prevent buffer overflow
        {
            tmp_mnemonic[index++] = **ptr; // Copy character to temporary mnemonic
        }
        *ptr = *ptr +1;
    }
    tmp_mnemonic[index] = '\0'; // Null-terminate the string

    // Compare with known common command mnemonics
    if(Find_maching_mnemonic(simple_command_mnemonics, current_command_root, tmp_mnemonic) != 0)
    {
        return 1; // No match found
    }

    return 0;
}



uint8_t Decode_common_command_program_header(struct program_mnemonic** current_command_root, uint8_t** ptr)
{
    uint8_t tmp_mnemonic[13] = {0};
    uint8_t index = 0;

    *ptr = *ptr +1; // Skip the '*'

    while(Is_mnemonic_character(**ptr))
    {
        if(index < 12) // Prevent buffer overflow
        {
            tmp_mnemonic[index++] = **ptr; // Copy character to temporary mnemonic
        }
        *ptr = *ptr +1;
    }
    tmp_mnemonic[index] = '\0'; // Null-terminate the string

    // Compare with known common command mnemonics
    if(Find_maching_mnemonic(common_command_mnemonics, current_command_root, tmp_mnemonic) != 0)
    {
        return 1; // No match found
    }

    return 0;
}

uint8_t Decode_compound_command_program_header(struct program_mnemonic** current_command_root, uint8_t** ptr)
{
    return 0;
}



uint8_t Find_maching_mnemonic(const struct program_mnemonic** mnemonics_list_ptr, struct program_mnemonic** current_command_root, uint8_t* input_mnemonic)
{
    uint8_t input_mnemonic_upper_case[13];
    strcpy((char*)input_mnemonic_upper_case, (char*)input_mnemonic);
    Make_string_uppercase(input_mnemonic_upper_case);
    const struct program_mnemonic** mnemonic_ptr = mnemonics_list_ptr;
    while(*mnemonic_ptr)
    {
        uint8_t mnemonic_name_upper_case[13];
        strcpy((char*)mnemonic_name_upper_case, (char*)(*mnemonic_ptr)->mnemonic_name);
        Make_string_uppercase(mnemonic_name_upper_case);
        if((*mnemonic_ptr)->parent == *current_command_root)
        {
            if(strcmp((char*)mnemonic_name_upper_case, (char*)input_mnemonic_upper_case) == 0)
            {
                *current_command_root = *mnemonic_ptr;
                return 0; // Match found
            }
        }
        mnemonic_ptr++;
    }
    return 1; // No match found
}

// Utility Functions

void Make_string_uppercase(uint8_t* string)
{
    while(*string)
    {
        if((*string >= 'a') && (*string <= 'z'))
        {
            *string = *string - ('a' - 'A');
        }
        string++;
    }
}

bool Is_mnemonic_character(uint8_t ptr)
{
    if(((ptr >= 'A') && (ptr <= 'Z')) || ((ptr >= 'a') && (ptr <= 'z')) || ((ptr >= '0') && (ptr <= '9')) || (ptr == '_') || (ptr == '?'))
    {
        return true;
    }
    return false;
}

void Step_thru_white_space(uint8_t** ptr)
{
    while(((**ptr >= 0x0) && (**ptr <= 0x09)) || ((**ptr >= 0x0B) && (**ptr <= 0x20)))
    {
        (*ptr)++;
    }
}

bool Is_white_space(uint8_t** ptr)
{
    if((((**ptr) >= 0x0) && ((**ptr) <= 0x09)) || (((**ptr) >= 0x0B) && ((**ptr) <= 0x20)))
    {
        return true;
    }
    return false;
}

void Step_thru_program_message_terminator(uint8_t** ptr)
{
    while((**ptr == '\n') || (**ptr == '\r'))
    {
        (*ptr)++;
    }
}

bool Is_program_message_terminator(uint8_t** ptr)
{
    if((**ptr == '\n') || (**ptr == '\r'))
    {
        return true;
    }
    return false;
}

bool Is_program_header_separator(uint8_t** ptr)
{
    return Is_white_space(ptr);
}

bool Is_program_data_separator(uint8_t** ptr)
{
    if(**ptr == ',')
    {
        return true;
    }
    return false;
}

void Step_thru_program_data_separator(uint8_t** ptr)
{
    (*ptr)++; // Move past the comma
}

bool Is_program_message_unit_separator(uint8_t** ptr)
{
    if(**ptr == ';')
    {
        return true;
    }
    return false;
}

void Step_thru_program_message_unit_separator(uint8_t** ptr)
{
    (*ptr)++; // Move past the semicolon
}

// FIFO Functions

void Input_FIFO_Init( struct Input_FIFO *FIFO)
{
	FIFO->Write_P = FIFO->Data;
	FIFO->Read_P = FIFO->Data;
	FIFO->Empty = true;
	FIFO->Full = false;
	FIFO->Lenght = FIFO_Size;
}

uint8_t Input_FIFO_Write( struct Input_FIFO *FIFO, uint8_t *Data )
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

uint8_t Input_FIFO_Read( struct Input_FIFO *FIFO, uint8_t *Data )
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

void Output_FIFO_Init( struct Output_FIFO *FIFO)
{
	FIFO->Write_P = FIFO->Data;
	FIFO->Read_P = FIFO->Data;
	FIFO->Empty = true;
	FIFO->Full = false;
	FIFO->Lenght = FIFO_Size;
}

uint8_t Output_FIFO_Write( struct Output_FIFO *FIFO, uint8_t *Data )
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

uint8_t Output_FIFO_Read( struct Output_FIFO *FIFO, uint8_t *Data )
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