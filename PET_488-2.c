#include "PET_488-2.h"

//TODO:
//  \r not handled correctly as message terminator
//when incrementing (*ptr)++ check for buffer overflow
// Implement Input_FIFO_Move_pointer function to move pointer within FIFO buffer safely
// Handle New_line_received_during_string properly
//                  -Save the adress of the New_line_received.   
//                  -Only decode untill that adress

#ifdef IS_A_TEST_VERSION
    uint8_t main(void) //ONLY FOR TESTING
    {
        PET_488_2 PET_488_2_1;
        uint8_t test_sting[] = "*IDN?\n";
        printf("START\n");
        PET_4882_Init(&PET_488_2_1);
        #ifdef ENABLE_ERROR_HANDLING
        PET_ERROR_init();
        #endif

        for(int i = 0; test_sting[i] != '\0'; i++)
            {
                PET_4882_Recive_character(&PET_488_2_1, test_sting[i]);
                PET_4882_Process(&PET_488_2_1);
            }

        
        return 0;
    }
#endif

void PET_4882_Init(PET_488_2* pet_488_2_instance)
{
    Program_Message_Terminator_FIFO_Init(&pet_488_2_instance->program_message_terminator_buffer);
    Input_FIFO_Init(&pet_488_2_instance->input_buffer);
    Output_FIFO_Init(&pet_488_2_instance->output_buffer);
    PET_4882_function_init(pet_488_2_instance);
    pet_488_2_instance->last_command_root = &root_mnemonic;
}

void PET_4882_Recive_character(PET_488_2* pet_488_2_instance, uint8_t character)
{
    if (Input_FIFO_Write(&pet_488_2_instance->input_buffer, &character) != 0)
    {
        #ifdef ENABLE_ERROR_HANDLING
        Error_Event(&Error_FIFO_1, (uint8_t *)"Input buffer overflow");
        #endif
        
    }
    if((character == '\n') || (character == '\r'))
    {
        Program_Message_Terminator_FIFO_Write(&pet_488_2_instance->program_message_terminator_buffer, &pet_488_2_instance->input_buffer.Write_P);
    } 
}

void PET_4882_Send_response(PET_488_2* pet_488_2_instance, uint8_t* string)
{
    printf(string);
    while(*string)
    {
        if (Output_FIFO_Write(&pet_488_2_instance->output_buffer, string) != 0)
        {
            #ifdef ENABLE_ERROR_HANDLING
            Error_Event(&Error_FIFO_1, (uint8_t *)"Output buffer overflow");
            #endif
            return;
        }
        string++;
    }
}

void PET_4882_Process(PET_488_2* pet_488_2_instance)
{
    if(0 == pet_488_2_instance->program_message_terminator_buffer.Empty)
    {
        uint8_t* FIFO_ptr = pet_488_2_instance->input_buffer.Read_P;
        bool more_message_units = false;
        struct program_mnemonic* current_command_root = pet_488_2_instance->last_command_root;
        pet_488_2_instance->function_buffer_index = 0;
        if (Program_Message_Terminator_FIFO_Read(&pet_488_2_instance->program_message_terminator_buffer, &pet_488_2_instance->read_input_buffer_until) != 0)
        {
            #ifdef ENABLE_ERROR_HANDLING
            Error_Event(&Error_FIFO_1, (uint8_t *)"Program message terminator buffer underflow");
            #endif
            return;
        }

        do // Process all received message units
        {
            Step_thru_white_space(&FIFO_ptr);
            
            if((*FIFO_ptr == '\n') || (*FIFO_ptr == '\r'))
            {
                #ifdef ENABLE_ERROR_HANDLING
                Error_Event(&Error_FIFO_1, (uint8_t *)"Empty command received");
                #endif
                pet_488_2_instance->input_buffer.Read_P = FIFO_ptr + 1;   // Move read pointer past the termination character(s)
                return;
            }
            else if ((*FIFO_ptr >= 'A' && *FIFO_ptr <= 'Z') || (*FIFO_ptr >= 'a' && *FIFO_ptr <= 'z'))
            {
                if(Decode_simple_command_program_header(pet_488_2_instance, &current_command_root, &FIFO_ptr) != 0)
                {
                    #ifdef ENABLE_ERROR_HANDLING
                    Error_Event(&Error_FIFO_1, (uint8_t *)"Invalid simple command received");
                    #endif
                    PET_4882_Init(pet_488_2_instance);
                    return;
                }
            }
            else if((*FIFO_ptr == '*'))
            {
                if(Decode_common_command_program_header(pet_488_2_instance, &current_command_root, &FIFO_ptr) != 0)
                {
                    #ifdef ENABLE_ERROR_HANDLING
                    Error_Event(&Error_FIFO_1, (uint8_t *)"Invalid common command received");
                    #endif
                    PET_4882_Init(pet_488_2_instance);
                    return;
                }
            }
            else if((*FIFO_ptr == ':'))
            {
                if(Decode_compound_command_program_header(pet_488_2_instance, &current_command_root, &FIFO_ptr) != 0)
                {
                    #ifdef ENABLE_ERROR_HANDLING
                    Error_Event(&Error_FIFO_1, (uint8_t *)"Invalid compound command received");
                    #endif
                    PET_4882_Init(pet_488_2_instance);
                    return;
                }
            }
            else
            {
                #ifdef ENABLE_ERROR_HANDLING
                Error_Event(&Error_FIFO_1, (uint8_t *)"Invalid command received");
                #endif
                PET_4882_Init(pet_488_2_instance);
                return;
            }
            uint8_t tmp_error = Decode_program_data(pet_488_2_instance, &current_command_root, &FIFO_ptr);
            if(tmp_error != 0)
            {
                if(tmp_error == 2)
                {
                    return; // Wait for more data
                }
                #ifdef ENABLE_ERROR_HANDLING
                Error_Event(&Error_FIFO_1, (uint8_t *)"Invalid or insufficient data provided");
                #endif
                PET_4882_Init(pet_488_2_instance);
                return;
            }
            if(Is_white_space(&FIFO_ptr))
            {
                Step_thru_white_space(&FIFO_ptr);
            }
            if(Is_program_message_terminator(&FIFO_ptr))
            {
                Step_thru_program_message_terminator(&FIFO_ptr);
                if(add_command_function_to_buffer(pet_488_2_instance, &current_command_root))
                {
                    #ifdef ENABLE_ERROR_HANDLING
                    Error_Event(&Error_FIFO_1, (uint8_t *)"Function buffer overflow");
                    #endif
                    PET_4882_Init(pet_488_2_instance);
                    return;
                }
                exacute_command_function(pet_488_2_instance);
                more_message_units = false;
                pet_488_2_instance->input_buffer.Read_P = FIFO_ptr;   // Move read pointer past the termination character(s)
            }
            else if(Is_program_message_unit_separator(&FIFO_ptr))
            {
                Step_thru_program_message_unit_separator(&FIFO_ptr);
                if(add_command_function_to_buffer(pet_488_2_instance, &current_command_root))
                {
                    #ifdef ENABLE_ERROR_HANDLING
                    Error_Event(&Error_FIFO_1, (uint8_t *)"Function buffer overflow");
                    #endif
                    PET_4882_Init(pet_488_2_instance);
                    return;
                }
                more_message_units = true;
            }
            else
            {
                #ifdef ENABLE_ERROR_HANDLING
                Error_Event(&Error_FIFO_1, (uint8_t *)"Invalid termination of command");
                #endif
                PET_4882_Init(pet_488_2_instance);
            }
            
        }
        while(more_message_units); // Is there a PROGRAM MESSAGE UNIT SEPARATOR (';') present?
    }
}

uint8_t add_command_function_to_buffer(PET_488_2* pet_488_2_instance, struct program_mnemonic** found_command)
{
    if(pet_488_2_instance->function_buffer_index < 8)
    {
        pet_488_2_instance->last_command_root = *found_command;
        pet_488_2_instance->function_buffer[pet_488_2_instance->function_buffer_index] = *found_command;
        pet_488_2_instance->function_buffer_index++;
        return 0;
    }
    else
    {
        return 1; // Function buffer full
    }
}

uint8_t exacute_command_function(PET_488_2* pet_488_2_instance)
{
    for(uint8_t i = 0; i < pet_488_2_instance->function_buffer_index; i++)
    {
        if(pet_488_2_instance->function_buffer[i]->function(&pet_488_2_instance->data_buffer[i][0],&pet_488_2_instance->data_buffer[i][1],&pet_488_2_instance->data_buffer[i][2],&pet_488_2_instance->data_buffer[i][3],&pet_488_2_instance->data_buffer[i][4]))
        {
            #ifdef ENABLE_ERROR_HANDLING
            Error_Event(&Error_FIFO_1, (uint8_t *)"Cant process command");
            #endif
        }
    }
    pet_488_2_instance->function_buffer_index = 0; // Reset function buffer index after command execution
    return 0;
}

uint8_t Decode_simple_command_program_header(PET_488_2* pet_488_2_instance, struct program_mnemonic** current_command_root, uint8_t** ptr)
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
    if(Find_maching_mnemonic(pet_488_2_instance->simple_command_mnemonics, current_command_root, tmp_mnemonic) != 0)
    {
        return 1; // No match found
    }

    return 0;
}



uint8_t Decode_common_command_program_header(PET_488_2* pet_488_2_instance, struct program_mnemonic** current_command_root, uint8_t** ptr)
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
    if(Find_maching_mnemonic(pet_488_2_instance->common_command_mnemonics, current_command_root, tmp_mnemonic) != 0)
    {
        return 1; // No match found
    }

    return 0;
}

uint8_t Decode_compound_command_program_header(PET_488_2* pet_488_2_instance, struct program_mnemonic** current_command_root, uint8_t** ptr)
{
    uint8_t tmp_mnemonic[13] = {0};
    uint8_t index = 0;
    uint8_t* saved_ptr = *ptr;

    *current_command_root = (*current_command_root)->parent; // Move up one level in the command hierarchy


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
        if(Find_maching_mnemonic(pet_488_2_instance->compound_command_mnemonics, current_command_root, tmp_mnemonic) != 0)
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
                if(Find_maching_mnemonic(pet_488_2_instance->compound_command_mnemonics, current_command_root, tmp_mnemonic) != 0)
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



uint8_t Find_maching_mnemonic(struct program_mnemonic** mnemonics_list_ptr, struct program_mnemonic** current_command_root, uint8_t* input_mnemonic)
{
    uint8_t input_mnemonic_upper_case[13];
    strcpy((char*)input_mnemonic_upper_case, (char*)input_mnemonic);
    Make_string_uppercase(input_mnemonic_upper_case);
    struct program_mnemonic** mnemonic_ptr = mnemonics_list_ptr;
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

uint8_t Decode_program_data(PET_488_2* pet_488_2_instance, struct program_mnemonic** current_command_root, uint8_t** ptr)
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
            {
                uint8_t tmp_character_program_data[13] = {0};
                if (Decode_character_program_data(ptr, tmp_character_program_data) != 0)
                {
                    return 1; // Error decoding character data
                }
                strcpy(pet_488_2_instance->data_buffer[pet_488_2_instance->function_buffer_index][i].string, (char*)tmp_character_program_data);
            }
            break;
        case DECIMAL_NUMERIC_PROGRAM_DATA:
            {
                float tmp_decimal_numeric_program_data = 0;
                if (Decode_decimal_numeric_program_data(ptr, (float*)&tmp_decimal_numeric_program_data) != 0)
                {
                    return 1; // Error decoding decimal numeric data
                }
                pet_488_2_instance->data_buffer[pet_488_2_instance->function_buffer_index][i].f = tmp_decimal_numeric_program_data;
            }
            break;
        case SUFFIX_PROGRAM_DATA:
            return 1; // Unsupported data type
            break;
        case NONDECIMAL_NUMERIC_PROGRAM_DATA:
            return 1; // Unsupported data type
            break;
        case STRING_PROGRAM_DATA:
            {
                uint8_t tmp_string_program_data[256] = {0};
                uint8_t tmp_error = 1;
                tmp_error = Decode_string_program_data(pet_488_2_instance, ptr, tmp_string_program_data);
                if (tmp_error == 2)
                {
                    return 2; // String not completed before buffer end
                }
                else if (tmp_error != 0)
                {
                    return 1; // Error decoding string data
                }
                strcpy(pet_488_2_instance->data_buffer[pet_488_2_instance->function_buffer_index][i].string, (char*)tmp_string_program_data);  
            }
            break;
        case ARBITRARY_BLOCK_PROGRAM_DATA:
            {
                uint8_t tmp_string_program_data[256] = {0};
                uint8_t tmp_error = 1;
                tmp_error = Decode_arbitrary_block_program_data(pet_488_2_instance, ptr, tmp_string_program_data);
                if (tmp_error == 2)
                {
                    return 2; // String not completed before buffer end
                }
                else if (tmp_error != 0)
                {
                    return 1; // Error decoding string data
                }
                strcpy(pet_488_2_instance->data_buffer[pet_488_2_instance->function_buffer_index][i].string, (char*)tmp_string_program_data);
            }
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

uint8_t Decode_string_program_data(PET_488_2* pet_488_2_instance, uint8_t** ptr, uint8_t* output_string)
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
                if (Input_FIFO_Move_pointer(pet_488_2_instance, &pet_488_2_instance->input_buffer, ptr ) != 0) // Move to the next character
                {
                    #ifdef IS_A_TEST_VERSION
                    printf("Buffer end reached while decoding string\n");
                    #endif
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
                if (Input_FIFO_Move_pointer(pet_488_2_instance, &pet_488_2_instance->input_buffer, ptr ) != 0) // Move to the next character
                {
                    #ifdef IS_A_TEST_VERSION
                    printf("Buffer end reached while decoding string\n");
                    #endif
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

uint8_t Decode_arbitrary_block_program_data(PET_488_2* pet_488_2_instance, uint8_t** ptr, uint8_t* output_string)
{
    uint8_t index = 0;
    if(**ptr != '#')
    {
        return 1; // Invalid arbitrary block format
    }
    (*ptr)++; // Move past the '#'
    if(!Is_digit(**ptr))
    {
        return 1; // Invalid arbitrary block format
    }
    if(**ptr == '0')
    {
        (*ptr)++; // Move past the '0'
        while(**ptr != '\n')
        {
            if(index < 255) // Prevent buffer overflow
            {
                output_string[index++] = **ptr; // Copy character to output string
            }
            else
            {
                return 1; // Output string buffer overflow
            }
            if (Input_FIFO_Move_pointer(pet_488_2_instance, &pet_488_2_instance->input_buffer, ptr ) != 0) // Move to the next character
            {
                #ifdef IS_A_TEST_VERSION
                printf("Buffer end reached while decoding arbitrary block\n");
                #endif
                return 2; // String not completed before buffer end
            }
        }
        if (Input_FIFO_Move_pointer(pet_488_2_instance, &pet_488_2_instance->input_buffer, ptr ) != 0) // Move past the newline character
            {
                #ifdef IS_A_TEST_VERSION
                printf("Buffer end reached while decoding arbitrary block\n");
                #endif
                return 2; // String not completed before buffer end
            }
        if(**ptr == '\r')
        {
            if (Input_FIFO_Move_pointer(pet_488_2_instance, &pet_488_2_instance->input_buffer, ptr ) != 0) // Move past the carriage return character
            {
                #ifdef IS_A_TEST_VERSION
                printf("Buffer end reached while decoding arbitrary block\n");
                #endif
                return 2; // String not completed before buffer end
            } 
        }
        else
        {
            return 1; // Invalid arbitrary block format
        }
    }
    else
    {
        uint8_t length_digits = **ptr - '0';
        (*ptr)++; // Move past the length digit
        uint32_t data_length = 0;
        for(uint8_t i = 0; i < length_digits; i++)
        {
            if(!Is_digit(**ptr))
            {
                return 1; // Invalid arbitrary block format
            }
            data_length = data_length *10 + (**ptr - '0');
            (*ptr)++; // Move past the length digit
        }
        for(uint32_t i = 0; i < data_length; i++)
        {
            if(index < 255) // Prevent buffer overflow
            {
                output_string[index++] = **ptr; // Copy character to output string
            }
            else
            {
                return 1; // Output string buffer overflow
            }
            if (Input_FIFO_Move_pointer(pet_488_2_instance, &pet_488_2_instance->input_buffer, ptr ) != 0) // Move to the next character
            {
                #ifdef IS_A_TEST_VERSION
                printf("Buffer end reached while decoding arbitrary block\n");
                #endif
                return 2; // String not completed before buffer end
            }
        }
    }
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

uint8_t Input_FIFO_Move_pointer(PET_488_2* pet_488_2_instance, struct Input_FIFO *FIFO, uint8_t **ptr )
{
	if((*ptr != FIFO->Write_P) && (*ptr != pet_488_2_instance->read_input_buffer_until-1))
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

void Program_Message_Terminator_FIFO_Init( struct program_message_terminator_FIFO *FIFO)
{
    FIFO->Write_P = (uint8_t**)&FIFO->Data[0];
    FIFO->Read_P = (uint8_t**)&FIFO->Data[0];
    FIFO->Empty = true;
    FIFO->Full = false;
    FIFO->Lenght = 32;
}

uint8_t Program_Message_Terminator_FIFO_Write( struct program_message_terminator_FIFO *FIFO, uint8_t **Data )
{
    if(!FIFO->Full)
    {
        *(FIFO->Write_P) = *Data;
        if(FIFO->Write_P < (uint8_t**)&FIFO->Data[FIFO->Lenght - 1])
        {
            FIFO->Write_P++;
        }
        else
        {
            FIFO->Write_P = (uint8_t**)&FIFO->Data[0];
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

uint8_t Program_Message_Terminator_FIFO_Read( struct program_message_terminator_FIFO *FIFO, uint8_t **Data )
{
    if(!FIFO->Empty)
    {
        *Data = *(FIFO->Read_P);
        if(FIFO->Read_P < (uint8_t**)&FIFO->Data[FIFO->Lenght - 1])
        {
            FIFO->Read_P++;
        }
        else
        {
            FIFO->Read_P = (uint8_t**)&FIFO->Data[0];
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



//===========================================================================================================

void PET_4882_function_init(PET_488_2* pet_488_2_instance)
{
	strcpy(root_mnemonic.mnemonic_name, "ROOT");
	root_mnemonic.parent = 0;
	memset(root_mnemonic.data_types, NON, 10);
	root_mnemonic.function = 0;
	root_mnemonic.is_end_mnemonic = false;

	strcpy(IDN_mnemonic.mnemonic_name, "IDN?");
	IDN_mnemonic.parent = &root_mnemonic;
	memset(IDN_mnemonic.data_types, NON, 10);
	IDN_mnemonic.function = &PET_4882_IDN_query;
	IDN_mnemonic.is_end_mnemonic = true;

	strcpy(RST_mnemonic.mnemonic_name, "RST");
	RST_mnemonic.parent = &root_mnemonic;
	memset(IDN_mnemonic.data_types, NON, 10);
	RST_mnemonic.function = &PET_4882_RST_command;
	RST_mnemonic.is_end_mnemonic = true;

	strcpy(SYSTem_mnemonic.mnemonic_name, "SYSTem");
	SYSTem_mnemonic.parent = &root_mnemonic;
	memset(SYSTem_mnemonic.data_types, NON, 10);
	SYSTem_mnemonic.function = 0;
	SYSTem_mnemonic.is_end_mnemonic = false;

	strcpy(SYSTem_Ld_mnemonic.mnemonic_name, "Ld");
	SYSTem_Ld_mnemonic.parent = &SYSTem_mnemonic;
	memset(SYSTem_Ld_mnemonic.data_types, NON, 10);
	SYSTem_Ld_mnemonic.data_types[0] = CHARACTER_PROGRAM_DATA;
	SYSTem_Ld_mnemonic.data_types[1] = CHARACTER_PROGRAM_DATA;
	SYSTem_Ld_mnemonic.function = &PET_4882_SYSTem_Ld;
	SYSTem_Ld_mnemonic.is_end_mnemonic = true;

	pet_488_2_instance->common_command_mnemonics[0] = &IDN_mnemonic;
	pet_488_2_instance->common_command_mnemonics[1] = &RST_mnemonic;
	pet_488_2_instance->common_command_mnemonics[2] = 0;

	pet_488_2_instance->simple_command_mnemonics[0] = 0;

	pet_488_2_instance->compound_command_mnemonics[0] = &SYSTem_mnemonic;
	pet_488_2_instance->compound_command_mnemonics[1] = &SYSTem_Ld_mnemonic;
	pet_488_2_instance->compound_command_mnemonics[2] = 0;
}


// Common Commands
uint8_t PET_4882_IDN_query(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5)
{
    uint8_t response[] = "MyInstrument,Model1234,Serial0001,1.0\n";
    //PET_4882_Send_response(response);
    //HAL_UART_Transmit(&huart3, response, 40, 1000);
    printf(response);
    return 0;
}

uint8_t PET_4882_RST_command(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5)
{
    //PET_4882_Init();
    //printf("PET_4882_RST_command\n");
    return 0;
}



//Simple Commands


//Compound Commands

uint8_t PET_4882_SYSTem_Ld(union data_union *var_1, union data_union *var_2, union data_union *var_3, union data_union *var_4, union data_union *var_5)
{
    //uint8_t response[] = "Firmware Version 1.0\n";
    //PET_4882_Send_response(response);
    //uint8_t message[256];
    //sprintf((char*)message, "ET_4882_SYSTem_NUMber_query %f\n", var_1->f);
    //printf("%s", message);
	if(var_1->i == '1')
	{
		if(var_2->i == '0')
		{
			//HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 0);
		}
		if(var_2->i == '1')
		{
			//HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
		}
	}
	if(var_1->i == '2')
	{
		if(var_2->i == '0')
		{
			//HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);
		}
		if(var_2->i == '1')
		{
			//HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
		}
	}
	if(var_1->i == '3')
	{
		if(var_2->i == '0')
		{
			//HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, 0);
		}
		if(var_2->i == '1')
		{
			//HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, 1);
		}
	}
    return 0;
}
