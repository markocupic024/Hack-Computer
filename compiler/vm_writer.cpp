#include "vm_writer.h"
#include <stdexcept>

VMWriter::VMWriter(const std::string &outputFile)
{
    vmFile.open(outputFile, std::ios::out);
    if (!vmFile)
    {
        throw std::runtime_error("Error: Cannot open output file: " + outputFile);
    }
}

void VMWriter::writePush(Segment segment, int index)
{
    vmFile << "push " << segmentToString(segment) << " " << index << "\n";
}

void VMWriter::writePop(Segment segment, int index)
{
    vmFile << "pop " << segmentToString(segment) << " " << index << "\n";
}

void VMWriter::writeArithmetic(Command command)
{
    vmFile << commandToString(command) << "\n";
}

void VMWriter::writeLabel(const std::string &label)
{
    vmFile << "label " << label << "\n";
}

void VMWriter::writeGoto(const std::string &label)
{
    vmFile << "goto " << label << "\n";
}

void VMWriter::writeIf(const std::string &label)
{
    vmFile << "if-goto " << label << "\n";
}

void VMWriter::writeCall(const std::string &name, int nArgs)
{
    vmFile << "call " << name << " " << nArgs << "\n";
}

void VMWriter::writeFunction(const std::string &name, int nLocals)
{
    vmFile << "function " << name << " " << nLocals << "\n";
}

void VMWriter::writeReturn()
{
    vmFile << "return\n";
}

void VMWriter::close()
{
    if (vmFile.is_open())
    {
        vmFile.close();
    }
}

std::string VMWriter::segmentToString(Segment segment)
{
    switch (segment)
    {
    case Segment::CONST: return "constant";
    case Segment::ARG: return "argument";
    case Segment::LOCAL: return "local";
    case Segment::STATIC: return "static";
    case Segment::THIS: return "this";
    case Segment::THAT: return "that";
    case Segment::POINTER: return "pointer";
    case Segment::TEMP: return "temp";
    default: throw std::runtime_error("Unknown segment");
    }
}

std::string VMWriter::commandToString(Command command)
{
    switch (command)
    {
    case Command::ADD: return "add";
    case Command::SUB: return "sub";
    case Command::NEG: return "neg";
    case Command::EQ: return "eq";
    case Command::GT: return "gt";
    case Command::LT: return "lt";
    case Command::AND: return "and";
    case Command::OR: return "or";
    case Command::NOT: return "not";
    default: throw std::runtime_error("Unknown command");
    }
}

