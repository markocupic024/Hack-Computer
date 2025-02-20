#include "parser.h"

parsed_line_ts parser_parseLine(char *line)
{
    parsed_line_ts parsed_line = {0};

    line[strcspn(line, "\n")] = '\0';

    char *trimmed_line = line;
    while (isspace((unsigned char)*trimmed_line))
    {
        trimmed_line++;
    }

    if (*trimmed_line == '\0' || (*trimmed_line == '/' && *(trimmed_line + 1) == '/'))
    {
        return parsed_line;
    }

    char *ptr = trimmed_line;

    if (*ptr == A_INSTRUCTION_SYMBOL)
    {
        parsed_line.instruction_type = A_INSTRUCTION;
        parsed_line.instruction.a_value = (uint16_t)strtoul(ptr + 1, NULL, 10);
    }
    else
    {
        parsed_line.instruction_type = C_INSTRUCTION;
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
            strncpy(parsed_line.instruction.instruction_fields.dest, dest, MAX_FIELD_BUFFER_LEN - 1);
            parsed_line.instruction.instruction_fields.dest[MAX_FIELD_BUFFER_LEN - 1] = '\0';
        }
        if (comp)
        {
            strncpy(parsed_line.instruction.instruction_fields.comp, comp, MAX_FIELD_BUFFER_LEN - 1);
            parsed_line.instruction.instruction_fields.comp[MAX_FIELD_BUFFER_LEN - 1] = '\0';
        }
        if (jump)
        {
            strncpy(parsed_line.instruction.instruction_fields.jump, jump, MAX_FIELD_BUFFER_LEN - 1);
            parsed_line.instruction.instruction_fields.jump[MAX_FIELD_BUFFER_LEN - 1] = '\0';
        }
    }

    return parsed_line;
}
