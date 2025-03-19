#include "parser.h"
#include <iostream>
#include <sstream>

Parser::Parser(std::ifstream &inputFile) : vmFile(inputFile)
{
}

bool Parser::hasMoreLines() const
{
    return !vmFile.eof();
}

void Parser::advance()
{
    if (std::getline(vmFile, currentLine))
    {
        size_t comment_pos = currentLine.find("//");
        if (comment_pos != std::string::npos)
        {
            currentLine = currentLine.substr(0, comment_pos);
        }

        currentLine.erase(0, currentLine.find_first_not_of(" \t"));
        currentLine.erase(currentLine.find_last_not_of(" \t") + 1);
    }
}

CommandType Parser::commandType() const
{
    if (currentLine.empty())
    {
        return CommandType::EMPTY;
    }
    if (currentLine == "add" || currentLine == "sub" || currentLine == "neg" ||
        currentLine == "eq" || currentLine == "gt" || currentLine == "lt" ||
        currentLine == "and" || currentLine == "or" || currentLine == "not")
    {
        return CommandType::ARITHMETIC;
    }
    else if (currentLine.find("push") == 0)
    {
        return CommandType::PUSH;
    }
    else if (currentLine.find("pop") == 0)
    {
        return CommandType::POP;
    }
    else
    {
        throw std::runtime_error("Unknown command: " + currentLine);
    }
}

std::string Parser::arg1() const
{
    std::istringstream stream(currentLine);
    std::string command, arg1;
    stream >> command >> arg1;

    if (command == "push" || command == "pop")
    {
        return arg1;
    }
    else
    {
        return currentLine;
    }
}

std::string Parser::arg2() const
{
    std::istringstream stream(currentLine);
    std::string command, arg1, arg2;
    stream >> command >> arg1 >> arg2;

    if (command == "push" || command == "pop")
    {
        return arg2;
    }
    else
    {
        return "";
    }
}