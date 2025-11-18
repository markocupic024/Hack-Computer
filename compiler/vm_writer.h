#ifndef VM_WRITER_H
#define VM_WRITER_H

#include <string>
#include <fstream>

enum class Segment
{
    CONST,
    ARG,
    LOCAL,
    STATIC,
    THIS,
    THAT,
    POINTER,
    TEMP
};

enum class Command
{
    ADD,
    SUB,
    NEG,
    EQ,
    GT,
    LT,
    AND,
    OR,
    NOT
};

class VMWriter
{
public:
    VMWriter(const std::string &outputFile);
    
    void writePush(Segment segment, int index);
    void writePop(Segment segment, int index);
    void writeArithmetic(Command command);
    void writeLabel(const std::string &label);
    void writeGoto(const std::string &label);
    void writeIf(const std::string &label);
    void writeCall(const std::string &name, int nArgs);
    void writeFunction(const std::string &name, int nLocals);
    void writeReturn();
    void close();

private:
    std::ofstream vmFile;
    
    std::string segmentToString(Segment segment);
    std::string commandToString(Command command);
};

#endif

