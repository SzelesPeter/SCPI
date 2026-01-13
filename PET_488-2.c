#include "PET_488-2.h"


//TODO:
//  \r not handled correctly as message terminator
//when incrementing (*ptr)++ check for buffer overflow
// Implement Input_FIFO_Move_pointer function to move pointer within FIFO buffer safely
// Handle New_line_received_during_string properly
//                  -Save the adress of the New_line_received.   
//                  -Only decode untill that adress


uint8_t main(void) //ONLY FOR TESTING
{
    uint8_t test_sting[] = ":systEM:message \"\n\rTest string\n\r with 'new line' and carrage return \"\"and quotes\"\"\";:message \"\n\rTest string\n\r with 'new line' and carrage return \"\"and quotes\"\"\" \n";
    printf("START\n");
    PET_4882_Init();
    Error_FIFO_Init(&Error_FIFO_1);

for(int i = 0; test_sting[i] != '\0'; i++)
    {
        PET_4882_Recive_character(test_sting[i]);
        PET_4882_Process();
    }

    
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
        function_buffer_index = 0;

        do // Process all received message units
        {
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
            uint8_t tmp_error = Decode_program_data(&current_command_root, &FIFO_ptr);
            if(tmp_error != 0)
            {
                if(tmp_error == 2)
                {
                    return; // Wait for more data
                }
                Error_Event(&Error_FIFO_1, (uint8_t *)"Invalid or insufficient data provided");
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
                if(add_command_function_to_buffer(&current_command_root))
                {
                    Error_Event(&Error_FIFO_1, (uint8_t *)"Function buffer overflow");
                    PET_4882_Init();
                    return;
                }
                exacute_command_function();
                more_message_units = false;
                New_line_received--;
                input_buffer.Read_P = FIFO_ptr + 1;   // Move read pointer past the termination character(s)
            }
            else if(Is_program_message_unit_separator(&FIFO_ptr))
            {
                Step_thru_program_message_unit_separator(&FIFO_ptr);
                if(add_command_function_to_buffer(&current_command_root))
                {
                    Error_Event(&Error_FIFO_1, (uint8_t *)"Function buffer overflow");
                    PET_4882_Init();
                    return;
                }
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

uint8_t add_command_function_to_buffer(struct program_mnemonic** found_command)
{
    if(function_buffer_index < 8)
    {
        last_command_root = *found_command;
        function_buffer[function_buffer_index] = *found_command;
        function_buffer_index++;
        return 0;
    }
    else
    {
        return 1; // Function buffer full
    }
}

uint8_t exacute_command_function()
{
    for(uint8_t i = 0; i < function_buffer_index; i++)
    {
        if(function_buffer[i]->function(&data_buffer[i][0],&data_buffer[i][1],&data_buffer[i][2],&data_buffer[i][3],&data_buffer[i][4]))
        {
            Error_Event(&Error_FIFO_1, (uint8_t *)"Cant process command");
        }
    }
    function_buffer_index = 0; // Reset function buffer index after command execution
    return 0;
}

uint8_t Decode_simple_command_program_header(struct program_mnemonic** current_command_root, uint8_t** ptr)
{
    uint8_t tmp_mnemonic[13] = {0};
    uint8_t index = 0;
    *current_command_root = &root_mnemonic; //reset current cooand pointer to root mnemonic

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
    *current_command_root = &root_mnemonic; //reset current cooand pointer to root mnemonic
    
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
    uint8_t tmp_mnemonic[13] = {0};
    uint8_t index = 0;
    uint8_t* saved_ptr = *ptr;

    *current_command_root = last_command_root->parent; // Move up one level in the command hierarchy


    while(Is_program_mnemonic_separator(ptr)) 
    {
        Step_thru_program_mnemonic_separator(ptr); // Skip the ':'
        
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
        if(Find_maching_mnemonic(compound_command_mnemonics, current_command_root, tmp_mnemonic) != 0)
        {
            // No match found, reset pointer and try again
            *ptr = saved_ptr;
            index = 0;
            *current_command_root = &root_mnemonic; //reset current cooand pointer to root mnemonic

            while(Is_program_mnemonic_separator(ptr)) 
            {
                Step_thru_program_mnemonic_separator(ptr); // Skip the ':'
                
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
                if(Find_maching_mnemonic(compound_command_mnemonics, current_command_root, tmp_mnemonic) != 0)
                {
                    return 1; // No match found
                }

                index = 0; // Reset index for next mnemonic
            }
        }

        index = 0; // Reset index for next mnemonic
    }

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
        uint8_t mnemonic_shortname_upper_case[13];
        strcpy((char*)mnemonic_shortname_upper_case, (char*)(*mnemonic_ptr)->mnemonic_name);
        for(uint8_t i = 12; i > 0; i--)
        {
            if(mnemonic_shortname_upper_case[i] >= 'a' && mnemonic_shortname_upper_case[i] <= 'z')
            {
                mnemonic_shortname_upper_case[i] = 0;
            }
            else if(mnemonic_shortname_upper_case[i] == '?' && (mnemonic_shortname_upper_case[i-1] >= 'a' && mnemonic_shortname_upper_case[i-1] <= 'z'))
            {
                mnemonic_shortname_upper_case[i] = 0;
                mnemonic_shortname_upper_case[i-1] = '?';
            }
        }
                
        if((*mnemonic_ptr)->parent == *current_command_root)
        {
            if(strcmp((char*)mnemonic_name_upper_case, (char*)input_mnemonic_upper_case) == 0)
            {
                *current_command_root = *mnemonic_ptr;
                return 0; // Match found
            }
            else if(strcmp((char*)mnemonic_shortname_upper_case, (char*)input_mnemonic_upper_case) == 0)
            {
                *current_command_root = *mnemonic_ptr;
                return 0; // Match found
            }
        }
        mnemonic_ptr++;
    }
    return 1; // No match found
}

uint8_t Decode_program_data(struct program_mnemonic** current_command_root, uint8_t** ptr)
{
    Step_thru_program_header_separator(ptr);

    for(uint8_t i = 0; i < 5; i++)
    {
        if (0 ==i)
        {
            if (Is_program_header_separator(ptr))
            {
                Step_thru_program_header_separator(ptr);
            }
        }
        else
        {
            if((*current_command_root)->data_types[i] != NON)
            {
                Step_thru_white_space(ptr);
                if (Is_program_data_separator(ptr))
                {
                    Step_thru_program_data_separator(ptr);
                }
                else
                {
                    return 1; // Not enough data provided
                }
                Step_thru_white_space(ptr);
            }
        }
            
        switch ((*current_command_root)->data_types[i])
        {
        case NON:
            break;
        case CHARACTER_PROGRAM_DATA:
            uint8_t tmp_character_program_data[13] = {0};
            if (Decode_character_program_data(ptr, tmp_character_program_data) != 0)
            {
                return 1; // Error decoding character data
            }
            strcpy(data_buffer[function_buffer_index][i].string, (char*)tmp_character_program_data);
            break;
        case DECIMAL_NUMERIC_PROGRAM_DATA:
            float tmp_decimal_numeric_program_data = 0;
            if (Decode_decimal_numeric_program_data(ptr, (float*)&tmp_decimal_numeric_program_data) != 0)
            {
                return 1; // Error decoding decimal numeric data
            }
            data_buffer[function_buffer_index][i].f = tmp_decimal_numeric_program_data;
            break;
        case SUFFIX_PROGRAM_DATA:
            return 1; // Unsupported data type
            break;
        case NONDECIMAL_NUMERIC_PROGRAM_DATA:
            return 1; // Unsupported data type
            break;
        case STRING_PROGRAM_DATA:
            uint8_t tmp_string_program_data[256] = {0};
            uint8_t tmp_error = 1;
            tmp_error = Decode_string_program_data(ptr, tmp_string_program_data);
            if (tmp_error == 2)
            {
                return 2; // String not completed before buffer end
            }
            else if (tmp_error != 0)
            {
                return 1; // Error decoding string data
            }
            strcpy(data_buffer[function_buffer_index][i].string, (char*)tmp_string_program_data);
            break;
        case ARBITRARY_BLOCK_PROGRAM_DATA:
            return 1; // Unsupported data type
            break;
        case EXPRESSION_PROGRAM_DATA:
            return 1; // Unsupported data type
            break;

        default:
            return 1; // Unsupported data type
            break;
        }
    }
    return 0;
}

uint8_t Decode_character_program_data(uint8_t** ptr, uint8_t* output_string)
{
    uint8_t index = 0;
    while(Is_mnemonic_character(**ptr))
    {
        if(index < 12) // Prevent buffer overflow
        {
            output_string[index++] = **ptr; // Copy character to temporary mnemonic
        }
        *ptr = *ptr +1;
    }
    if (index == 0)
    {
        return 1; // No character data found
    }
    output_string[index] = '\0'; // Null-terminate the string
    return 0;
}

uint8_t Decode_decimal_numeric_program_data(uint8_t** ptr, float* output_value)
{
    float mantissa = 0;
    int32_t exponent = 0;
    uint32_t mantissa_integer = 0;
    float mantissa_fraction = 0;
    bool is_negative = false;
    bool integer_part_exists = false;
    bool fraction_part_exists = false;

    
    if(**ptr == '-')
    {
        is_negative = true;
        (*ptr)++; // Move past the negative sign
    }
    else if(**ptr == '+')
    {
        (*ptr)++; // Move past the positive sign
    }
    else if(Is_digit(**ptr)) // Read integer part
    {
        integer_part_exists = true;
        while (Is_digit(**ptr))
        {
            mantissa_integer = mantissa_integer *10 + (**ptr - '0');
            (*ptr)++;
        }
    }

    if(**ptr == '.') // Read fractional part
    {
        (*ptr)++; // Skip the decimal point
        if(Is_digit(**ptr)) // Read fractional digits
        {
            fraction_part_exists = true;
            uint8_t fraction_digit_count = 0;
            while (Is_digit(**ptr))
            {
                mantissa_fraction = mantissa_fraction + ( (**ptr - '0') / pow(10, fraction_digit_count + 1) );
                fraction_digit_count++;
                (*ptr)++;
            }
        }
    }

    if(!integer_part_exists && !fraction_part_exists)
    {
        return 1; // No numeric data found
    }

    // Combine integer and fractional parts and apply sign
    mantissa = (float)mantissa_integer+(float)mantissa_fraction;
    if(is_negative)
    {
        mantissa = -mantissa;
    }

    Step_thru_white_space(ptr);

    if((**ptr == 'E') || (**ptr == 'e')) // Read exponent part
    {
        (*ptr)++; // Skip the 'E' or 'e'
        Step_thru_white_space(ptr);

        bool exponent_negative = false;
        if(**ptr == '-')
        {
            exponent_negative = true;
            (*ptr)++; // Move past the negative sign
        }
        else if(**ptr == '+')
        {
            (*ptr)++; // Move past the positive sign
        }
        if(Is_digit(**ptr)) // Read exponent digits
        {
            while (Is_digit(**ptr))
            {
                exponent = exponent *10 + (**ptr - '0');
                (*ptr)++;
            }
            if(exponent_negative) // Apply exponent sign to exponent value
            {
                exponent = -((int32_t)exponent);
            }
        }
        else
        {
            return 1; // Invalid exponent format
        }
    }
    *output_value = mantissa * pow(10, exponent); // Apply exponent to mantissa
    return 0;
}

uint8_t Decode_suffix_program_data(uint8_t** ptr, float* output_value)
{
    // Implementation for decoding suffix data goes here
    return 0;
}

uint8_t Decode_nondecmial_numeric_program_data(uint8_t** ptr, int32_t* output_value)
{
    // Implementation for decoding non-decimal numeric data goes here
    return 0;
}

uint8_t Decode_string_program_data(uint8_t** ptr, uint8_t* output_string)
{
    uint8_t New_line_received_during_string = 0;
    if(**ptr == '\"') // Check for opening double quote
    {
        (*ptr)++; // Move past the opening double quote
        uint8_t index = 0;
        while(1)
        {
            if(**ptr == '\"') // Check for closing double quote
            {
                (*ptr)++; // Move past the closing double quote
                if(**ptr == '\"') // Check for inserted double quote
                {
                    if(index < 255) // Prevent buffer overflow
                    {
                        output_string[index++] = '\"'; // Add a single double quote to the output
                    }
                    (*ptr)++; // Move past the inserted double quote
                }
                else
                {
                    output_string[index] = '\0'; // Null-terminate the string
                    return 0; // Successfully decoded string
                }
            }
            else
            {
                if(index < 255) // Prevent buffer overflow
                {
                    output_string[index++] = **ptr; // Copy character to output string
                    if(**ptr == '\n' || **ptr == '\r')
                    {
                        New_line_received_during_string++;
                    }
                }
                else
                {
                    return 1; // Output string buffer overflow
                }
                if (Input_FIFO_Move_pointer( &input_buffer, ptr ) != 0) // Move to the next character
                {
                    printf("Buffer end reached while decoding string\n");
                    New_line_received--;
                    return 2; // String not completed before buffer end
                }
            }
        }
    }
    else if(**ptr == '\'')
    {
        (*ptr)++; // Move past the opening single quote
        uint8_t index = 0;
        while(1)
        {
            if(**ptr == '\'') // Check for closing single quote
            {
                (*ptr)++; // Move past the closing single quote
                if(**ptr == '\'') // Check for inserted single quote
                {
                    if(index < 255) // Prevent buffer overflow
                    {
                        output_string[index++] = '\''; // Add a single single quote to the output
                    }
                    (*ptr)++; // Move past the inserted single quote
                }
                else
                {
                    output_string[index] = '\0'; // Null-terminate the string
                    return 0; // Successfully decoded string
                }
            }
            else
            {
                if(index < 255) // Prevent buffer overflow
                {
                    output_string[index++] = **ptr; // Copy character to output string
                }
                else
                {
                    return 1; // Output string buffer overflow
                }
                if (Input_FIFO_Move_pointer( &input_buffer, ptr ) != 0) // Move to the next character
                {
                    printf("Buffer end reached while decoding string\n");
                    return 2; // String not completed before buffer end
                }
            }
        }
    }
    else
    {
        return 1; // Invalid string format
    }

    return 0;
}

uint8_t Decode_arbitrary_block_program_data(uint8_t** ptr, uint8_t* output_data, uint32_t* output_length)
{
    // Implementation for decoding arbitrary block data goes here
    return 0;
}

uint8_t Decode_expression_program_data(uint8_t** ptr, float* output_value)
{
    // Implementation for decoding expression data goes here
    return 0;
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

bool Is_digit(uint8_t ptr)
{
    if((ptr >= '0') && (ptr <= '9'))
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


bool Is_program_mnemonic_separator(uint8_t** ptr)
{
    if(**ptr == ':')
    {
        return true;
    }
    return false;
}

void Step_thru_program_mnemonic_separator(uint8_t** ptr)
{
    (*ptr)++; // Move past the semicolon
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

void Step_thru_program_header_separator(uint8_t** ptr)
{
    Step_thru_white_space(ptr);
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
	FIFO->Lenght = Input_buffer_Size;
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

uint8_t Input_FIFO_Move_pointer( struct Input_FIFO *FIFO, uint8_t **ptr )
{
	if(*ptr != FIFO->Write_P)
	{
		if(*ptr<(FIFO->Data+FIFO->Lenght-1))
		{
			(*ptr)++;
		}
		else
		{
			*ptr = FIFO->Data;
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
	FIFO->Lenght = Output_buffer_Size;
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