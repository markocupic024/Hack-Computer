#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include "jack_tokenizer.h"
#include "symbol_table.h"
#include "vm_writer.h"
#include <string>

class CompilationEngine
{
public:
    CompilationEngine(JackTokenizer &tokenizer, VMWriter &vmWriter);
    
    void compileClass();
    
private:
    JackTokenizer &tokenizer;
    VMWriter &vmWriter;
    SymbolTable symbolTable;
    
    std::string className;
    int labelCounter = 0;
    
    void compileClassVarDec();
    void compileSubroutine();
    void compileParameterList();
    void compileVarDec();
    void compileStatements();
    void compileDo();
    void compileLet();
    void compileWhile();
    void compileReturn();
    void compileIf();
    void compileExpression();
    void compileTerm();
    int compileExpressionList();
    
    void eat(TokenType type);
    void eat(Keyword keyword);
    void eat(char symbol);
    
    std::string generateLabel();
};

#endif

