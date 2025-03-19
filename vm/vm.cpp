#include "parser.h"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

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

    fs::path asmFileName = inputPath.replace_extension(".asm");

    std::ifstream vmFile(vmFileName, std::ios::in);
    if (!vmFile)
    {
        std::cerr << "Error: Cannot open file " << vmFileName << '\n';
        return EXIT_FAILURE;
    }

    Parser parser(vmFile);

    while (parser.hasMoreLines())
    {
        parser.advance();
        CommandType commandType = parser.commandType();

        if (commandType == CommandType::EMPTY)
        {
            continue;
        }

        if (commandType == CommandType::ARITHMETIC)
        {
            std::cout << parser.arg1() << '\n';
        }
        else if (commandType == CommandType::POP)
        {
            std::cout << parser.arg1() << " " << parser.arg2() << '\n';
        }
        else if (commandType == CommandType::PUSH)
        {
            std::cout << parser.arg1() << " " << parser.arg2() << '\n';
        }
    }

    return EXIT_SUCCESS;
}
