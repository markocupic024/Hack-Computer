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
    
    // Inform the code writer that the translation of a new VM file is started.
    void setFileName(const std::string &fileName);
    
    // Writes the assembly instructions that effect the bootstrap code.
    void writeInit();
    
    void writeArithmetic(const std::string &command);
    void writePushPop(CommandType command, const std::string &segment, const int index);
    
    void writeLabel(const std::string &label);
    void writeGoto(const std::string &label);
    void writeIf(const std::string &label);
    
    void writeCall(const std::string &functionName, int numArgs);
    void writeReturn();
    void writeFunction(const std::string &functionName, int numLocals);
    
    void close();

private:
    std::ofstream asmFile;
    std::string currentFileName;
    std::string currentFunctionName;
    int labelCounter = 0;
    int callCounter = 0;

    std::string generateLabel(const std::string &base);
};

#endif