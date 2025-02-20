#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_FIELD_BUFFER_LEN 4

#define A_INSTRUCTION_SYMBOL '@'

#define C_INSTRUCTION_DEST_SYMBOL '='

#define C_INSTRUCTION_JMP_SYMBOL ';'

#ifndef PARSER_H
#define PARSER_H

typedef enum
{
    A_INSTRUCTION,
    C_INSTRUCTION,
} instruction_types_te;

typedef struct
{
    char dest[MAX_FIELD_BUFFER_LEN];
    char comp[MAX_FIELD_BUFFER_LEN];
    char jump[MAX_FIELD_BUFFER_LEN];
} instruction_fields_ts;

typedef union
{
    instruction_fields_ts instruction_fields; // For C-instruction
    uint16_t a_value;                         // For A-instruction (stores @value)
} instruction_tu;

typedef struct
{
    instruction_tu instruction;
    instruction_types_te instruction_type;
} parsed_line_ts;

parsed_line_ts parser_parseLine(char *line);

#endif