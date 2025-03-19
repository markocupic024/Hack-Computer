#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <fstream>

enum CommandType
{
    EMPTY,
    ARITHMETIC,
    PUSH,
    POP
};

class Parser
{
public:
    Parser(std::ifstream &inputFile);

    bool hasMoreLines() const;

    void advance();

    CommandType commandType() const;

    std::string arg1() const;

    std::string arg2() const;

private:
    std::ifstream &vmFile;
    std::string currentLine;
};

#endif