#include "parser.h"

bool parser_firstPassLine(symbol_table_ts *table, char *line, size_t *rom_line)
{
    line[strcspn(line, "\n")] = '\0';

    char *trimmed_line = line;
    while (isspace((unsigned char)*trimmed_line))
    {
        trimmed_line++;
    }

    if (*trimmed_line == '\0' || (*trimmed_line == '/' && *(trimmed_line + 1) == '/'))
        return false;

    char *ptr = trimmed_line;

    if (*ptr == L_INSTRUCTION_SYMBOL)
    {
        char *l_instruction = ptr + 1;
        char *label = strtok(l_instruction, L_INSTRUCTION_CLOSE_SYMBOL);
        if (label)
        {
            symbol_table_set(table, label, *rom_line);
        }
    }
    else
    {
        (*rom_line)++;
    }
    return true;
}

bool parser_secondPassLine(parsed_line_ts *parsed_line, symbol_table_ts *table, char *line)
{
    parsed_line->instruction_type = INVALID_INSTRUCTION;

    line[strcspn(line, "\n")] = '\0';

    char *trimmed_line = line;
    while (isspace((unsigned char)*trimmed_line))
    {
        trimmed_line++;
    }

    if (*trimmed_line == '\0' || (*trimmed_line == '/' && *(trimmed_line + 1) == '/'))
        return false;

    char *ptr = trimmed_line;

    static size_t value_address = 16;

    if (*ptr == L_INSTRUCTION_SYMBOL)
    {
        return false;
    }
    else if (*ptr == A_INSTRUCTION_SYMBOL)
    {
        parsed_line->instruction_type = A_INSTRUCTION;

        char *a_instruction = ptr + 1;
        size_t parsed_a;

        if (isalpha(*a_instruction))
        {
            if (!symbol_table_get(table, a_instruction, &parsed_a))
            {
                parsed_a = value_address;
                symbol_table_set(table, a_instruction, value_address);
                value_address++;
            }
        }
        else
        {
            parsed_a = strtoul(a_instruction, NULL, 10);
        }
        if (parsed_a > A_INSTRUCTION_VALUE_MAX)
        {
            fprintf(stderr, "assembler: A-instruction value out of range: %zu\n", parsed_a);
            exit(EXIT_FAILURE);
        }
        parsed_line->instruction.a_value = (uint16_t)parsed_a;
    }
    else
    {
        parsed_line->instruction_type = C_INSTRUCTION;

        parsed_line->instruction.instruction_fields = (instruction_fields_ts){
            .dest = "null",
            .comp = "null",
            .jump = "null"};

        char *dest = NULL, *comp = NULL, *jump = NULL;

        char *dest_symbol = strchr(ptr, C_INSTRUCTION_DEST_SYMBOL);
        char *jmp_symbol = strchr(ptr, C_INSTRUCTION_JMP_SYMBOL);

        if (dest_symbol)
        {
            *dest_symbol = '\0';
            dest = ptr;
            comp = dest_symbol + 1;
        }
        else
        {
            comp = ptr;
        }
        if (jmp_symbol)
        {
            *jmp_symbol = '\0';
            jump = jmp_symbol + 1;
        }

        if (dest)
        {
            strncpy(parsed_line->instruction.instruction_fields.dest, dest, MAX_FIELD_BUFFER_LEN - 1);
            parsed_line->instruction.instruction_fields.dest[MAX_FIELD_BUFFER_LEN - 1] = '\0';
        }
        if (comp)
        {
            strncpy(parsed_line->instruction.instruction_fields.comp, comp, MAX_FIELD_BUFFER_LEN - 1);
            parsed_line->instruction.instruction_fields.comp[MAX_FIELD_BUFFER_LEN - 1] = '\0';
        }
        if (jump)
        {
            strncpy(parsed_line->instruction.instruction_fields.jump, jump, MAX_FIELD_BUFFER_LEN - 1);
            parsed_line->instruction.instruction_fields.jump[MAX_FIELD_BUFFER_LEN - 1] = '\0';
        }
    }

    return true;
}
