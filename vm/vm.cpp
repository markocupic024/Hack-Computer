#include "parser.h"
#include "code_writer.h"
#include "command_type.h"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <memory>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

void processFile(const fs::path &vmPath, CodeWriter &codeWriter)
{
    std::ifstream vmFile(vmPath, std::ios::in);
    if (!vmFile)
    {
        throw std::runtime_error("Error: Cannot open file " + vmPath.string());
    }

    codeWriter.setFileName(vmPath.stem().string());
    Parser parser(vmFile);

    while (parser.hasMoreLines())
    {
        parser.advance();
        CommandType commandType = parser.commandType();

        switch (commandType)
        {
        case CommandType::ARITHMETIC:
            codeWriter.writeArithmetic(parser.arg1());
            break;
        case CommandType::PUSH:
            codeWriter.writePushPop(CommandType::PUSH, parser.arg1(), std::stoi(parser.arg2()));
            break;
        case CommandType::POP:
            codeWriter.writePushPop(CommandType::POP, parser.arg1(), std::stoi(parser.arg2()));
            break;
        case CommandType::LABEL:
            codeWriter.writeLabel(parser.arg1());
            break;
        case CommandType::GOTO:
            codeWriter.writeGoto(parser.arg1());
            break;
        case CommandType::IF:
            codeWriter.writeIf(parser.arg1());
            break;
        case CommandType::CALL:
            codeWriter.writeCall(parser.arg1(), std::stoi(parser.arg2()));
            break;
        case CommandType::FUNCTION:
            codeWriter.writeFunction(parser.arg1(), std::stoi(parser.arg2()));
            break;
        case CommandType::RETURN:
            codeWriter.writeReturn();
            break;
        default:
            break;
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: vm <input.vm|directory>\n";
        return EXIT_FAILURE;
    }

    fs::path inputPath(argv[1]);
    fs::path asmPath;
    std::vector<fs::path> vmFiles;
    bool isDirectory = false;

    if (fs::is_directory(inputPath))
    {
        isDirectory = true;
        asmPath = inputPath / (inputPath.filename().string() + ".asm");
        
        for (const auto &entry : fs::directory_iterator(inputPath))
        {
            if (entry.path().extension() == ".vm")
            {
                vmFiles.push_back(entry.path());
            }
        }
        
        if (vmFiles.empty()) {
            std::cerr << "Error: No .vm files found in directory.\n";
            return EXIT_FAILURE;
        }
    }
    else
    {
        if (inputPath.extension() != ".vm")
        {
            std::cerr << "Error: Input file must have a .vm extension.\n";
            return EXIT_FAILURE;
        }
        asmPath = inputPath;
        asmPath.replace_extension(".asm");
        vmFiles.push_back(inputPath);
    }

    try
    {
        CodeWriter codeWriter(asmPath);

        if (isDirectory)
        {
            codeWriter.writeInit();
        }

        for (const auto &vmFile : vmFiles)
        {
            processFile(vmFile, codeWriter);
        }
        
        codeWriter.close();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
