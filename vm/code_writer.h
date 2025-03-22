#ifndef CODE_WRITER_H
#define CODE_WRITER_H

#include "command_type.h"
#include <filesystem>
#include <fstream>
#include <string>

class CodeWriter
{
public:
    CodeWriter(const std::filesystem::path &outputFilePath);
    void writeArithmetic(const std::string &command);
    void writePushPop(CommandType command, const std::string &segment, const std::string &index);

private:
    std::ofstream asmFile;
    int labelCounter = 0;

    std::string generateLabel(const std::string &base);
};

#endif