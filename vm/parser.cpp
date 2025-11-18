#include "parser.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

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

        size_t first = currentLine.find_first_not_of(" \t\r\n");
        if (first == std::string::npos)
        {
            currentLine = "";
            return;
        }
        size_t last = currentLine.find_last_not_of(" \t\r\n");
        currentLine = currentLine.substr(first, (last - first + 1));
    }
}

CommandType Parser::commandType() const
{
    if (currentLine.empty())
    {
        return CommandType::EMPTY;
    }
    
    std::string command;
    std::istringstream stream(currentLine);
    stream >> command;

    if (command == "add" || command == "sub" || command == "neg" ||
        command == "eq" || command == "gt" || command == "lt" ||
        command == "and" || command == "or" || command == "not")
    {
        return CommandType::ARITHMETIC;
    }
    else if (command == "push")
    {
        return CommandType::PUSH;
    }
    else if (command == "pop")
    {
        return CommandType::POP;
    }
    else if (command == "label")
    {
        return CommandType::LABEL;
    }
    else if (command == "goto")
    {
        return CommandType::GOTO;
    }
    else if (command == "if-goto")
    {
        return CommandType::IF;
    }
    else if (command == "function")
    {
        return CommandType::FUNCTION;
    }
    else if (command == "return")
    {
        return CommandType::RETURN;
    }
    else if (command == "call")
    {
        return CommandType::CALL;
    }
    else
    {
        throw std::runtime_error("Unknown command: " + currentLine);
    }
}

std::string Parser::arg1() const
{
    CommandType type = commandType();
    if (type == CommandType::RETURN) {
        throw std::runtime_error("arg1() called on RETURN command");
    }

    if (type == CommandType::ARITHMETIC) {
        std::istringstream stream(currentLine);
        std::string command;
        stream >> command;
        return command;
    }

    std::istringstream stream(currentLine);
    std::string command, arg1;
    stream >> command >> arg1;
    return arg1;
}

std::string Parser::arg2() const
{
    CommandType type = commandType();
    if (type == CommandType::PUSH || type == CommandType::POP || 
        type == CommandType::FUNCTION || type == CommandType::CALL)
    {
        std::istringstream stream(currentLine);
        std::string command, arg1, arg2;
        stream >> command >> arg1 >> arg2;
        return arg2;
    }
    else 
    {
        throw std::runtime_error("arg2() called on invalid command type");
    }
}