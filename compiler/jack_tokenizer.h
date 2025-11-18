#ifndef JACK_TOKENIZER_H
#define JACK_TOKENIZER_H

#include "token_type.h"
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>

class JackTokenizer
{
public:
    JackTokenizer(std::ifstream &inputFile);
    
    bool hasMoreTokens() const;
    void advance();
    
    TokenType tokenType() const;
    Keyword keyword() const;
    char symbol() const;
    std::string identifier() const;
    int intVal() const;
    std::string stringVal() const;

private:
    std::string currentToken;
    TokenType currentTokenType;
    std::ifstream &file;
    std::string fileContent;
    size_t position = 0;
    
    void tokenize();
    std::vector<std::string> tokens;
    size_t tokenIndex = 0;
    
    bool isSymbol(char c);
    void removeComments();
    void initTokens();
    
    static std::unordered_map<std::string, Keyword> keywordMap;
};

#endif

