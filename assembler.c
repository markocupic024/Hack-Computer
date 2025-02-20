#define _GNU_SOURCE
#include <sys/types.h>

#include "parser.h"

int main(int argc, char **argv)
{

    if (argc != 2)
    {
        fprintf(stderr, "usage: assembler <input>.asm\n");
        return EXIT_FAILURE;
    }

    const char *file_name = argv[1];

    char *dot = strrchr(file_name, '.');

    if (!dot || dot == file_name || strcmp(dot + 1, "asm") != 0) // file extension
    {
        fprintf(stderr, "assembler: input file does not have a correct file extension or name\n");
        return EXIT_FAILURE;
    }

    FILE *asm_file = fopen(file_name, "r");

    if (!asm_file)
    {
        perror("assembler: error opening file\n");
        return EXIT_FAILURE;
    }

    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;

    while ((read = getline(&line, &line_len, asm_file)) != -1)
    {
        parsed_line_ts parsed_line = parser_parseLine(line);

        if (parsed_line.instruction_type == C_INSTRUCTION)
        {
            printf("%s\n", parsed_line.instruction.instruction_fields.comp);
        }
    }

    free(line);
    line = NULL;
    fclose(asm_file);

    return EXIT_SUCCESS;
}