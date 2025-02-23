#define _GNU_SOURCE
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

#include "parser.h"
#include "code.h"

#define A_INSTRUCTION_LSB 15
#define A_INSTRUCTION_MSB 0

#define C_INSTRUCTION_MSB 111

int main(int argc, char **argv)
{

    if (argc != 2)
    {
        fprintf(stderr, "usage: assembler <input>.asm\n");
        return EXIT_FAILURE;
    }

    const char *asm_file_name = argv[1];

    char *dot = strrchr(asm_file_name, '.');

    if (!dot || dot == asm_file_name || strcmp(dot + 1, "asm") != 0) // file extension
    {
        fprintf(stderr, "assembler: input file does not have a correct file extension or name\n");
        return EXIT_FAILURE;
    }

    size_t base_len = dot - asm_file_name;
    char hack_file_name[base_len + 6];

    snprintf(hack_file_name, sizeof(hack_file_name), "%.*s.hack", (int)base_len, asm_file_name);

    FILE *asm_file = fopen(asm_file_name, "r");
    if (!asm_file)
    {
        perror("assembler: error opening .asm input file");
        return EXIT_FAILURE;
    }

    FILE *hack_file = fopen(hack_file_name, "w");
    if (!hack_file)
    {
        perror("assembler: error opening output .hack file\n");
        return EXIT_FAILURE;
    }

    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;

    while ((read = getline(&line, &line_len, asm_file)) != -1)
    {
        parsed_line_ts parsed_line = parser_parseLine(line);

        if (parsed_line.instruction_type == A_INSTRUCTION)
        {
            fprintf(hack_file, "%d", A_INSTRUCTION_MSB);
            for (size_t i = A_INSTRUCTION_LSB - 1; i >= 0; i--)
            {
                fprintf(hack_file, "%d", (parsed_line.instruction.a_value >> i) & 1); // Shift each bit right and then mask LSB
            }
            fprintf(hack_file, "\n");
        }
        else if (parsed_line.instruction_type == C_INSTRUCTION)
        {
            const char *dest = code_dest(parsed_line.instruction.instruction_fields.dest);
            const char *comp = code_comp(parsed_line.instruction.instruction_fields.comp);
            const char *jump = code_jump(parsed_line.instruction.instruction_fields.jump);

            if (!dest && !comp && !jump)
            {
                fprintf(stderr, "assembler: invalid instructions\n");
                return EXIT_FAILURE;
            }

            fprintf(hack_file, "%d%s%s%s\n", C_INSTRUCTION_MSB, dest, comp, jump);
        }
    }

    free(line);
    line = NULL;
    fclose(asm_file);
    fclose(hack_file);

    return EXIT_SUCCESS;
}
