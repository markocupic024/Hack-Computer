#define _GNU_SOURCE
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "code.h"
#include "symbol_table.h"
#include "hack_symbols.h"

#define A_INSTRUCTION_LSB 15
#define A_INSTRUCTION_MSB 0
#define C_INSTRUCTION_MSB 111

static void add_predefined_symbols(symbol_table_ts *table);
static FILE *open_file(const char *filename, const char *mode);
static void first_pass(FILE *file, symbol_table_ts *table);
static void second_pass(FILE *file, symbol_table_ts *table, const char *hack_file_name);

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: assembler <input>.asm\n");
        return EXIT_FAILURE;
    }

    const char *asm_file_name = argv[1];
    char *dot = strrchr(asm_file_name, '.');

    if (!dot || dot == asm_file_name || strcmp(dot + 1, "asm") != 0)
    {
        fprintf(stderr, "assembler: input file does not have a correct file extension or name\n");
        return EXIT_FAILURE;
    }

    symbol_table_ts *symbol_table = malloc(sizeof(symbol_table_ts));

    if (!symbol_table)
    {
        perror("assembler: malloc failed");
        return EXIT_FAILURE;
    }

    symbol_table_init(symbol_table);

    add_predefined_symbols(symbol_table);

    FILE *asm_file = open_file(asm_file_name, "r");

    first_pass(asm_file, symbol_table);

    fclose(asm_file);

    size_t base_len = dot - asm_file_name;
    char hack_file_name[base_len + 6];

    snprintf(hack_file_name, sizeof(hack_file_name), "%.*s.hack", (int)base_len, asm_file_name);

    asm_file = open_file(asm_file_name, "r");

    second_pass(asm_file, symbol_table, hack_file_name);

    fclose(asm_file);
    symbol_table_free(symbol_table);
    free(symbol_table);

    return EXIT_SUCCESS;
}
static void first_pass(FILE *file, symbol_table_ts *table)
{
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    size_t rom_line = 0;

    while ((read = getline(&line, &line_len, file)) != -1)
    {
        parser_firstPassLine(table, line, &rom_line);
    }

    free(line);
}

static void second_pass(FILE *file, symbol_table_ts *table, const char *hack_file_name)
{
    FILE *hack_file = open_file(hack_file_name, "w");

    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    parsed_line_ts parsed_line;

    while ((read = getline(&line, &line_len, file)) != -1)
    {
        if (parser_secondPassLine(&parsed_line, table, line))
        {
            if (parsed_line.instruction_type == A_INSTRUCTION)
            {
                fprintf(hack_file, "%d", A_INSTRUCTION_MSB);
                for (int i = A_INSTRUCTION_LSB - 1; i >= 0; i--)
                {
                    fprintf(hack_file, "%d", (parsed_line.instruction.a_value >> i) & 1);
                }
                fprintf(hack_file, "\n");
            }
            else if (parsed_line.instruction_type == C_INSTRUCTION)
            {
                const char *dest = code_dest(parsed_line.instruction.instruction_fields.dest);
                const char *comp = code_comp(parsed_line.instruction.instruction_fields.comp);
                const char *jump = code_jump(parsed_line.instruction.instruction_fields.jump);

                if (!dest || !comp || !jump)
                {
                    fprintf(stderr, "assembler: invalid instruction encountered\n");
                    exit(EXIT_FAILURE);
                }
                fprintf(hack_file, "%d%s%s%s\n", C_INSTRUCTION_MSB, comp, dest, jump);
            }
        }
    }

    free(line);
    fclose(hack_file);
}

static void add_predefined_symbols(symbol_table_ts *table)
{
    symbol_table_set(table, "R0", R0);
    symbol_table_set(table, "R1", R1);
    symbol_table_set(table, "R2", R2);
    symbol_table_set(table, "R3", R3);
    symbol_table_set(table, "R4", R4);
    symbol_table_set(table, "R5", R5);
    symbol_table_set(table, "R6", R6);
    symbol_table_set(table, "R7", R7);
    symbol_table_set(table, "R8", R8);
    symbol_table_set(table, "R9", R9);
    symbol_table_set(table, "R10", R10);
    symbol_table_set(table, "R11", R11);
    symbol_table_set(table, "R12", R12);
    symbol_table_set(table, "R13", R13);
    symbol_table_set(table, "R14", R14);
    symbol_table_set(table, "R15", R15);
    symbol_table_set(table, "SCREEN", SCREEN);
    symbol_table_set(table, "KBD", KBD);
    symbol_table_set(table, "SP", SP);
    symbol_table_set(table, "LCL", LCL);
    symbol_table_set(table, "ARG", ARG);
    symbol_table_set(table, "THIS", THIS);
    symbol_table_set(table, "THAT", THAT);
}

static FILE *open_file(const char *filename, const char *mode)
{
    FILE *file = fopen(filename, mode);
    if (!file)
    {
        perror("assembler: error opening file");
        exit(EXIT_FAILURE);
    }
    return file;
}
