#include "jack_tokenizer.h"
#include <stdexcept>
#include <iostream>
#include <regex>

std::unordered_map<std::string, Keyword> JackTokenizer::keywordMap = {
    {"class", Keyword::CLASS}, {"constructor", Keyword::CONSTRUCTOR}, {"function", Keyword::FUNCTION},
    {"method", Keyword::METHOD}, {"field", Keyword::FIELD}, {"static", Keyword::STATIC},
    {"var", Keyword::VAR}, {"int", Keyword::INT}, {"char", Keyword::CHAR},
    {"boolean", Keyword::BOOLEAN}, {"void", Keyword::VOID}, {"true", Keyword::TRUE},
    {"false", Keyword::FALSE}, {"null", Keyword::NULL_VAL}, {"this", Keyword::THIS},
    {"let", Keyword::LET}, {"do", Keyword::DO}, {"if", Keyword::IF},
    {"else", Keyword::ELSE}, {"while", Keyword::WHILE}, {"return", Keyword::RETURN}
};

JackTokenizer::JackTokenizer(std::ifstream &inputFile) : file(inputFile)
{
    // Read entire file into string
    std::string line;
    while (std::getline(file, line))
    {
        fileContent += line + "\n";
    }
    
    removeComments();
    initTokens();
}

void JackTokenizer::removeComments()
{
    std::string clean;
    bool inString = false;
    bool inBlockComment = false;
    bool inLineComment = false;
    
    for (size_t i = 0; i < fileContent.length(); ++i)
    {
        char c = fileContent[i];
        char next = (i + 1 < fileContent.length()) ? fileContent[i + 1] : '\0';
        
        if (inString)
        {
            clean += c;
            if (c == '"') inString = false;
        }
        else if (inBlockComment)
        {
            if (c == '*' && next == '/')
            {
                inBlockComment = false;
                i++; // skip /
            }
        }
        else if (inLineComment)
        {
            if (c == '\n')
            {
                inLineComment = false;
                clean += c;
            }
        }
        else
        {
            if (c == '"')
            {
                inString = true;
                clean += c;
            }
            else if (c == '/' && next == '*')
            {
                inBlockComment = true;
                i++; // skip *
            }
            else if (c == '/' && next == '/')
            {
                inLineComment = true;
                i++; // skip /
            }
            else
            {
                clean += c;
            }
        }
    }
    fileContent = clean;
}

void JackTokenizer::initTokens()
{
    std::string token;
    bool inString = false;
    
    for (size_t i = 0; i < fileContent.length(); ++i)
    {
        char c = fileContent[i];
        
        if (inString)
        {
            token += c;
            if (c == '"')
            {
                tokens.push_back(token);
                token.clear();
                inString = false;
            }
        }
        else
        {
            if (isspace(c))
            {
                if (!token.empty())
                {
                    tokens.push_back(token);
                    token.clear();
                }
            }
            else if (isSymbol(c))
            {
                if (!token.empty())
                {
                    tokens.push_back(token);
                    token.clear();
                }
                tokens.push_back(std::string(1, c));
            }
            else if (c == '"')
            {
                if (!token.empty())
                {
                    tokens.push_back(token);
                    token.clear();
                }
                token += c;
                inString = true;
            }
            else
            {
                token += c;
            }
        }
    }
    if (!token.empty())
    {
        tokens.push_back(token);
    }
}

bool JackTokenizer::isSymbol(char c)
{
    return std::string("{}()[].,;+-*/&|<>=~").find(c) != std::string::npos;
}

bool JackTokenizer::hasMoreTokens() const
{
    return tokenIndex < tokens.size();
}

void JackTokenizer::advance()
{
    if (hasMoreTokens())
    {
        currentToken = tokens[tokenIndex++];
        
        if (currentToken.length() == 1 && isSymbol(currentToken[0]))
        {
            currentTokenType = TokenType::SYMBOL;
        }
        else if (keywordMap.count(currentToken))
        {
            currentTokenType = TokenType::KEYWORD;
        }
        else if (isdigit(currentToken[0]))
        {
            currentTokenType = TokenType::INT_CONST;
        }
        else if (currentToken[0] == '"')
        {
            currentTokenType = TokenType::STRING_CONST;
        }
        else
        {
            currentTokenType = TokenType::IDENTIFIER;
        }
    }
}

TokenType JackTokenizer::tokenType() const
{
    return currentTokenType;
}

Keyword JackTokenizer::keyword() const
{
    if (currentTokenType == TokenType::KEYWORD)
    {
        return keywordMap.at(currentToken);
    }
    throw std::runtime_error("Not a keyword: " + currentToken);
}

char JackTokenizer::symbol() const
{
    if (currentTokenType == TokenType::SYMBOL)
    {
        return currentToken[0];
    }
    throw std::runtime_error("Not a symbol: " + currentToken);
}

std::string JackTokenizer::identifier() const
{
    if (currentTokenType == TokenType::IDENTIFIER)
    {
        return currentToken;
    }
    throw std::runtime_error("Not an identifier: " + currentToken);
}

int JackTokenizer::intVal() const
{
    if (currentTokenType == TokenType::INT_CONST)
    {
        return std::stoi(currentToken);
    }
    throw std::runtime_error("Not an integer constant: " + currentToken);
}

std::string JackTokenizer::stringVal() const
{
    if (currentTokenType == TokenType::STRING_CONST)
    {
        // Remove quotes
        return currentToken.substr(1, currentToken.length() - 2);
    }
    throw std::runtime_error("Not a string constant: " + currentToken);
}

