#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "symbol_table.h"

#define MAX_FIELD_BUFFER_LEN 5
#define A_INSTRUCTION_VALUE_MAX 32767
#define L_INSTRUCTION_SYMBOL '('
#define L_INSTRUCTION_CLOSE_SYMBOL ")"
#define A_INSTRUCTION_SYMBOL '@'
#define C_INSTRUCTION_DEST_SYMBOL '='
#define C_INSTRUCTION_JMP_SYMBOL ';'

typedef enum
{
    INVALID_INSTRUCTION,
    L_INSTRUCTION,
    A_INSTRUCTION,
    C_INSTRUCTION
} instruction_types_te;

typedef struct
{
    char dest[MAX_FIELD_BUFFER_LEN];
    char comp[MAX_FIELD_BUFFER_LEN];
    char jump[MAX_FIELD_BUFFER_LEN];
} instruction_fields_ts;

typedef union
{
    instruction_fields_ts instruction_fields;
    uint16_t a_value;
} instruction_tu;

typedef struct
{
    instruction_tu instruction;
    instruction_types_te instruction_type;
} parsed_line_ts;

bool parser_firstPassLine(symbol_table_ts *table, char *line, size_t *rom_line);

bool parser_secondPassLine(parsed_line_ts *parsed_line, symbol_table_ts *table, char *line);

#endif
