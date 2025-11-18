#include "code_writer.h"
#include <stdexcept>
#include <iostream>
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

void CodeWriter::setFileName(const std::string &fileName)
{
    currentFileName = fileName;
}

void CodeWriter::writeInit()
{
    asmFile << "// Bootstrap code\n"
            << "@256\n"
            << "D=A\n"
            << "@SP\n"
            << "M=D\n";
    writeCall("Sys.init", 0);
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

void CodeWriter::writePushPop(CommandType command, const std::string &segment, const int index)
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
            std::string addr = (index == 0) ? "THIS" : "THAT";
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
            // Use currentFileName for static variables
            // Convention: FileName.index
            std::string staticLabel = currentFileName + "." + std::to_string(index);
            asmFile << "@" << staticLabel << "\n"
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
            std::string addr = (index == 0) ? "THIS" : "THAT";
            asmFile << "@SP\n"
                    << "AM=M-1\n"
                    << "D=M\n"
                    << "@" << addr << "\n"
                    << "M=D\n";
        }
        else if (segment == "static")
        {
             std::string staticLabel = currentFileName + "." + std::to_string(index);
            asmFile << "@SP\n"
                    << "AM=M-1\n"
                    << "D=M\n"
                    << "@" << staticLabel << "\n"
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

void CodeWriter::writeLabel(const std::string &label)
{
    std::string labelName = currentFunctionName.empty() ? label : currentFunctionName + "$" + label;
    asmFile << "(" << labelName << ")\n";
}

void CodeWriter::writeGoto(const std::string &label)
{
    std::string labelName = currentFunctionName.empty() ? label : currentFunctionName + "$" + label;
    asmFile << "@" << labelName << "\n"
            << "0;JMP\n";
}

void CodeWriter::writeIf(const std::string &label)
{
    std::string labelName = currentFunctionName.empty() ? label : currentFunctionName + "$" + label;
    asmFile << "@SP\n"
            << "AM=M-1\n"
            << "D=M\n"
            << "@" << labelName << "\n"
            << "D;JNE\n";
}

void CodeWriter::writeCall(const std::string &functionName, int numArgs)
{
    std::string returnLabel = functionName + "$ret." + std::to_string(callCounter++);
    
    // push returnAddress
    asmFile << "@" << returnLabel << "\n"
            << "D=A\n"
            << "@SP\n"
            << "A=M\n"
            << "M=D\n"
            << "@SP\n"
            << "M=M+1\n";
            
    // push LCL, ARG, THIS, THAT
    std::string segments[] = {"LCL", "ARG", "THIS", "THAT"};
    for (const auto& seg : segments) {
        asmFile << "@" << seg << "\n"
                << "D=M\n"
                << "@SP\n"
                << "A=M\n"
                << "M=D\n"
                << "@SP\n"
                << "M=M+1\n";
    }
    
    // ARG = SP - nArgs - 5
    asmFile << "@SP\n"
            << "D=M\n"
            << "@5\n"
            << "D=D-A\n"
            << "@" << numArgs << "\n"
            << "D=D-A\n"
            << "@ARG\n"
            << "M=D\n";
            
    // LCL = SP
    asmFile << "@SP\n"
            << "D=M\n"
            << "@LCL\n"
            << "M=D\n";
            
    // goto functionName
    asmFile << "@" << functionName << "\n"
            << "0;JMP\n";
            
    // (returnAddress)
    asmFile << "(" << returnLabel << ")\n";
}

void CodeWriter::writeReturn()
{
    // FRAME = LCL (stored in R13 for temp storage)
    asmFile << "@LCL\n"
            << "D=M\n"
            << "@R13\n"
            << "M=D\n";
            
    // RET = *(FRAME-5) (stored in R14)
    asmFile << "@5\n"
            << "A=D-A\n"
            << "D=M\n"
            << "@R14\n"
            << "M=D\n";
            
    // *ARG = pop()
    asmFile << "@SP\n"
            << "AM=M-1\n"
            << "D=M\n"
            << "@ARG\n"
            << "A=M\n"
            << "M=D\n";
            
    // SP = ARG + 1
    asmFile << "@ARG\n"
            << "D=M+1\n"
            << "@SP\n"
            << "M=D\n";
            
    // Restore THAT, THIS, ARG, LCL
    // THAT = *(FRAME-1)
    asmFile << "@R13\n"
            << "D=M\n" 
            << "@1\n"
            << "A=D-A\n"
            << "D=M\n"
            << "@THAT\n"
            << "M=D\n";
            
    // THIS = *(FRAME-2)
    asmFile << "@R13\n"
            << "D=M\n" 
            << "@2\n"
            << "A=D-A\n"
            << "D=M\n"
            << "@THIS\n"
            << "M=D\n";
            
    // ARG = *(FRAME-3)
    asmFile << "@R13\n"
            << "D=M\n" 
            << "@3\n"
            << "A=D-A\n"
            << "D=M\n"
            << "@ARG\n"
            << "M=D\n";
            
    // LCL = *(FRAME-4)
    asmFile << "@R13\n"
            << "D=M\n" 
            << "@4\n"
            << "A=D-A\n"
            << "D=M\n"
            << "@LCL\n"
            << "M=D\n";
            
    // goto RET
    asmFile << "@R14\n"
            << "A=M\n"
            << "0;JMP\n";
}

void CodeWriter::writeFunction(const std::string &functionName, int numLocals)
{
    currentFunctionName = functionName;
    asmFile << "(" << functionName << ")\n";
    for (int i = 0; i < numLocals; ++i)
    {
        asmFile << "@SP\n"
                << "A=M\n"
                << "M=0\n"
                << "@SP\n"
                << "M=M+1\n";
    }
}

void CodeWriter::close()
{
    if (asmFile.is_open())
    {
        asmFile.close();
    }
}
