#include "code_writer.h"
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <string>

CodeWriter::CodeWriter(const std::filesystem::path &outputFilePath)
{
    asmFile.open(outputFilePath, std::ios::out);
    if (!asmFile)
    {
        throw std::runtime_error("Error: Cannot open output file: " + outputFilePath.string() + "\n");
    }
}

std::string CodeWriter::generateLabel(const std::string &base)
{
    return base + std::to_string(labelCounter++);
}

void CodeWriter::writeArithmetic(const std::string &command)
{
    if (!asmFile.is_open())
        return;

    if (command == "add")
    {
        asmFile << "@SP\n"
                   "AM=M-1\n"
                   "D=M\n"
                   "A=A-1\n"
                   "M=D+M\n";
    }
    else if (command == "sub")
    {
        asmFile << "@SP\n"
                   "AM=M-1\n"
                   "D=M\n"
                   "A=A-1\n"
                   "M=M-D\n";
    }
    else if (command == "neg")
    {
        asmFile << "@SP\n"
                   "A=M-1\n"
                   "M=-M\n";
    }
    else if (command == "eq" || command == "gt" || command == "lt")
    {
        std::string trueLabel = generateLabel("TRUE");
        std::string endLabel = generateLabel("END");

        asmFile << "@SP\n"
                   "AM=M-1\n"
                   "D=M\n"
                   "A=A-1\n"
                   "D=M-D\n";

        if (command == "eq")
            asmFile << "@" << trueLabel << "\n"
                    << "D;JEQ\n";
        else if (command == "gt")
            asmFile << "@" << trueLabel << "\n"
                    << "D;JGT\n";
        else if (command == "lt")
            asmFile << "@" << trueLabel << "\n"
                    << "D;JLT\n";

        asmFile << "@SP\n"
                   "A=M-1\n"
                   "M=0\n"
                   "@"
                << endLabel << "\n"
                << "0;JMP\n";

        asmFile << "(" << trueLabel << ")\n"
                                       "@SP\n"
                                       "A=M-1\n"
                                       "M=-1\n";

        asmFile << "(" << endLabel << ")\n";
    }
    else if (command == "and")
    {
        asmFile << "@SP\n"
                   "AM=M-1\n"
                   "D=M\n"
                   "A=A-1\n"
                   "M=D&M\n";
    }
    else if (command == "or")
    {
        asmFile << "@SP\n"
                   "AM=M-1\n"
                   "D=M\n"
                   "A=A-1\n"
                   "M=D|M\n";
    }
    else if (command == "not")
    {
        asmFile << "@SP\n"
                   "A=M-1\n"
                   "M=!M\n";
    }
    else
    {
        throw std::runtime_error("Invalid arithmetic command: " + command);
    }
}

void CodeWriter::writePushPop(CommandType command, const std::string &segment, const std::string &index)
{
    if (!asmFile.is_open())
        return;

    if (command == CommandType::PUSH)
    {
        if (segment == "constant")
        {
            asmFile << "@" << index << "\n"
                    << "D=A\n"
                    << "@SP\n"
                    << "A=M\n"
                    << "M=D\n"
                    << "@SP\n"
                    << "M=M+1\n";
        }
        else if (segment == "local" || segment == "argument" ||
                 segment == "this" || segment == "that")
        {
            std::string base;
            if (segment == "local")
                base = "LCL";
            else if (segment == "argument")
                base = "ARG";
            else if (segment == "this")
                base = "THIS";
            else if (segment == "that")
                base = "THAT";

            asmFile << "@" << index << "\n"
                    << "D=A\n"
                    << "@" << base << "\n"
                    << "A=D+M\n"
                    << "D=M\n"
                    << "@SP\n"
                    << "A=M\n"
                    << "M=D\n"
                    << "@SP\n"
                    << "M=M+1\n";
        }
        else if (segment == "temp")
        {
            asmFile << "@" << index << "\n"
                    << "D=A\n"
                    << "@5\n"
                    << "A=A+D\n"
                    << "D=M\n"
                    << "@SP\n"
                    << "A=M\n"
                    << "M=D\n"
                    << "@SP\n"
                    << "M=M+1\n";
        }
        else if (segment == "pointer")
        {
            std::string addr = (index == "0") ? "THIS" : "THAT";
            asmFile << "@" << addr << "\n"
                    << "D=M\n"
                    << "@SP\n"
                    << "A=M\n"
                    << "M=D\n"
                    << "@SP\n"
                    << "M=M+1\n";
        }
        else if (segment == "static")
        {
            asmFile << "@" << "StaticVar." << index << "\n"
                    << "D=M\n"
                    << "@SP\n"
                    << "A=M\n"
                    << "M=D\n"
                    << "@SP\n"
                    << "M=M+1\n";
        }
        else
        {
            throw std::runtime_error("Unknown segment for push: " + segment);
        }
    }
    else if (command == CommandType::POP)
    {
        if (segment == "local" || segment == "argument" ||
            segment == "this" || segment == "that")
        {
            std::string base;
            if (segment == "local")
                base = "LCL";
            else if (segment == "argument")
                base = "ARG";
            else if (segment == "this")
                base = "THIS";
            else if (segment == "that")
                base = "THAT";

            asmFile << "@" << index << "\n"
                    << "D=A\n"
                    << "@" << base << "\n"
                    << "D=D+M\n"
                    << "@R13\n"
                    << "M=D\n"
                    << "@SP\n"
                    << "AM=M-1\n"
                    << "D=M\n"
                    << "@R13\n"
                    << "A=M\n"
                    << "M=D\n";
        }
        else if (segment == "temp")
        {
            asmFile << "@" << index << "\n"
                    << "D=A\n"
                    << "@5\n"
                    << "D=A+D\n"
                    << "@R13\n"
                    << "M=D\n"
                    << "@SP\n"
                    << "AM=M-1\n"
                    << "D=M\n"
                    << "@R13\n"
                    << "A=M\n"
                    << "M=D\n";
        }
        else if (segment == "pointer")
        {
            std::string addr = (index == "0") ? "THIS" : "THAT";
            asmFile << "@SP\n"
                    << "AM=M-1\n"
                    << "D=M\n"
                    << "@" << addr << "\n"
                    << "M=D\n";
        }
        else if (segment == "static")
        {
            asmFile << "@SP\n"
                    << "AM=M-1\n"
                    << "D=M\n"
                    << "@" << "StaticVar." << index << "\n"
                    << "M=D\n";
        }
        else
        {
            throw std::runtime_error("Unknown segment for pop: " + segment);
        }
    }
    else
    {
        throw std::runtime_error("Invalid command type in writePushPop");
    }
}
