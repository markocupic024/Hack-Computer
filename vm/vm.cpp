#include "parser.h"
#include "code_writer.h"
#include "command_type.h"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <memory>

namespace fs = std::filesystem;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: vm <input>.vm\n";
        return EXIT_FAILURE;
    }

    std::string vmFileName = argv[1];
    fs::path inputPath(vmFileName);

    if (inputPath.extension() != ".vm")
    {
        std::cerr << "Error: Input file must have a .vm extension.\n";
        return EXIT_FAILURE;
    }

    if (!fs::exists(inputPath))
    {
        std::cerr << "Error: File does not exist: " << vmFileName << '\n';
        return EXIT_FAILURE;
    }

    fs::path asmFileName = inputPath;
    asmFileName.replace_extension(".asm");

    std::ifstream vmFile(vmFileName, std::ios::in);
    if (!vmFile)
    {
        std::cerr << "Error: Cannot open file " << vmFileName << '\n';
        return EXIT_FAILURE;
    }

    std::unique_ptr<Parser> parser = std::make_unique<Parser>(vmFile);
    std::unique_ptr<CodeWriter> codeWriter = std::make_unique<CodeWriter>(asmFileName);

    while (parser->hasMoreLines())
    {
        parser->advance();
        CommandType commandType = parser->commandType();

        switch (commandType)
        {
        case CommandType::ARITHMETIC:
            codeWriter->writeArithmetic(parser->arg1());
            break;
        case CommandType::POP:
            codeWriter->writePushPop(CommandType::POP, parser->arg1(), parser->arg2());
            break;
        case CommandType::PUSH:
            codeWriter->writePushPop(CommandType::PUSH, parser->arg1(), parser->arg2());
            break;
        default:
            break;
        }
    }

    return EXIT_SUCCESS;
}
