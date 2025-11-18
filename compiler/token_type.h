#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

enum class TokenType
{
    KEYWORD,
    SYMBOL,
    IDENTIFIER,
    INT_CONST,
    STRING_CONST,
    NONE
};

enum class Keyword
{
    CLASS,
    METHOD,
    FUNCTION,
    CONSTRUCTOR,
    INT,
    BOOLEAN,
    CHAR,
    VOID,
    VAR,
    STATIC,
    FIELD,
    LET,
    DO,
    IF,
    ELSE,
    WHILE,
    RETURN,
    TRUE,
    FALSE,
    NULL_VAL,
    THIS,
    NONE
};

#endif

