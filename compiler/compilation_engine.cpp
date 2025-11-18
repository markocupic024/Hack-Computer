#include "compilation_engine.h"
#include <stdexcept>
#include <iostream>

CompilationEngine::CompilationEngine(JackTokenizer &tokenizer, VMWriter &vmWriter)
    : tokenizer(tokenizer), vmWriter(vmWriter)
{
    if (tokenizer.hasMoreTokens())
    {
        tokenizer.advance();
    }
}

void CompilationEngine::eat(TokenType type)
{
    if (tokenizer.tokenType() == type)
    {
        if (tokenizer.hasMoreTokens())
        {
            tokenizer.advance();
        }
    }
    else
    {
        throw std::runtime_error("Expected token type " + std::to_string((int)type));
    }
}

void CompilationEngine::eat(Keyword keyword)
{
    if (tokenizer.tokenType() == TokenType::KEYWORD && tokenizer.keyword() == keyword)
    {
        if (tokenizer.hasMoreTokens())
        {
            tokenizer.advance();
        }
    }
    else
    {
        throw std::runtime_error("Expected keyword");
    }
}

void CompilationEngine::eat(char symbol)
{
    if (tokenizer.tokenType() == TokenType::SYMBOL && tokenizer.symbol() == symbol)
    {
        if (tokenizer.hasMoreTokens())
        {
            tokenizer.advance();
        }
    }
    else
    {
        throw std::runtime_error("Expected symbol " + std::string(1, symbol));
    }
}

std::string CompilationEngine::generateLabel()
{
    return "L" + std::to_string(labelCounter++);
}

void CompilationEngine::compileClass()
{
    eat(Keyword::CLASS);
    className = tokenizer.identifier();
    eat(TokenType::IDENTIFIER);
    eat('{');

    while (tokenizer.tokenType() == TokenType::KEYWORD && 
          (tokenizer.keyword() == Keyword::STATIC || tokenizer.keyword() == Keyword::FIELD))
    {
        compileClassVarDec();
    }

    while (tokenizer.tokenType() == TokenType::KEYWORD &&
          (tokenizer.keyword() == Keyword::CONSTRUCTOR || tokenizer.keyword() == Keyword::FUNCTION || tokenizer.keyword() == Keyword::METHOD))
    {
        compileSubroutine();
    }

    eat('}');
}

void CompilationEngine::compileClassVarDec()
{
    SymbolKind kind = (tokenizer.keyword() == Keyword::STATIC) ? SymbolKind::STATIC : SymbolKind::FIELD;
    eat(TokenType::KEYWORD); // static or field
    
    std::string type;
    if (tokenizer.tokenType() == TokenType::KEYWORD)
    {
        // int, char, boolean
        if (tokenizer.keyword() == Keyword::INT) type = "int";
        else if (tokenizer.keyword() == Keyword::CHAR) type = "char";
        else if (tokenizer.keyword() == Keyword::BOOLEAN) type = "boolean";
        eat(TokenType::KEYWORD);
    }
    else
    {
        type = tokenizer.identifier();
        eat(TokenType::IDENTIFIER);
    }
    
    std::string name = tokenizer.identifier();
    symbolTable.define(name, type, kind);
    eat(TokenType::IDENTIFIER);
    
    while (tokenizer.tokenType() == TokenType::SYMBOL && tokenizer.symbol() == ',')
    {
        eat(',');
        name = tokenizer.identifier();
        symbolTable.define(name, type, kind);
        eat(TokenType::IDENTIFIER);
    }
    eat(';');
}

void CompilationEngine::compileSubroutine()
{
    symbolTable.startSubroutine();
    Keyword subroutineType = tokenizer.keyword();
    eat(TokenType::KEYWORD); // constructor, function, method
    
    if (subroutineType == Keyword::METHOD)
    {
        symbolTable.define("this", className, SymbolKind::ARG);
    }
    
    // Return type
    if (tokenizer.tokenType() == TokenType::KEYWORD && tokenizer.keyword() == Keyword::VOID)
    {
        eat(TokenType::KEYWORD);
    }
    else if (tokenizer.tokenType() == TokenType::KEYWORD)
    {
        eat(TokenType::KEYWORD);
    }
    else
    {
        eat(TokenType::IDENTIFIER);
    }
    
    std::string subroutineName = tokenizer.identifier();
    eat(TokenType::IDENTIFIER);
    
    eat('(');
    compileParameterList();
    eat(')');
    
    eat('{');
    while (tokenizer.tokenType() == TokenType::KEYWORD && tokenizer.keyword() == Keyword::VAR)
    {
        compileVarDec();
    }
    
    int nLocals = symbolTable.varCount(SymbolKind::VAR);
    vmWriter.writeFunction(className + "." + subroutineName, nLocals);
    
    if (subroutineType == Keyword::CONSTRUCTOR)
    {
        int nFields = symbolTable.varCount(SymbolKind::FIELD);
        vmWriter.writePush(Segment::CONST, nFields);
        vmWriter.writeCall("Memory.alloc", 1);
        vmWriter.writePop(Segment::POINTER, 0);
    }
    else if (subroutineType == Keyword::METHOD)
    {
        vmWriter.writePush(Segment::ARG, 0);
        vmWriter.writePop(Segment::POINTER, 0);
    }
    
    compileStatements();
    eat('}');
}

void CompilationEngine::compileParameterList()
{
    if (tokenizer.tokenType() == TokenType::SYMBOL && tokenizer.symbol() == ')')
    {
        return;
    }
    
    std::string type;
    if (tokenizer.tokenType() == TokenType::KEYWORD)
    {
        if (tokenizer.keyword() == Keyword::INT) type = "int";
        else if (tokenizer.keyword() == Keyword::CHAR) type = "char";
        else if (tokenizer.keyword() == Keyword::BOOLEAN) type = "boolean";
        eat(TokenType::KEYWORD);
    }
    else
    {
        type = tokenizer.identifier();
        eat(TokenType::IDENTIFIER);
    }
    
    std::string name = tokenizer.identifier();
    symbolTable.define(name, type, SymbolKind::ARG);
    eat(TokenType::IDENTIFIER);
    
    while (tokenizer.tokenType() == TokenType::SYMBOL && tokenizer.symbol() == ',')
    {
        eat(',');
        if (tokenizer.tokenType() == TokenType::KEYWORD)
        {
             if (tokenizer.keyword() == Keyword::INT) type = "int";
            else if (tokenizer.keyword() == Keyword::CHAR) type = "char";
            else if (tokenizer.keyword() == Keyword::BOOLEAN) type = "boolean";
            eat(TokenType::KEYWORD);
        }
        else
        {
            type = tokenizer.identifier();
            eat(TokenType::IDENTIFIER);
        }
        
        name = tokenizer.identifier();
        symbolTable.define(name, type, SymbolKind::ARG);
        eat(TokenType::IDENTIFIER);
    }
}

void CompilationEngine::compileVarDec()
{
    eat(Keyword::VAR);
    std::string type;
    if (tokenizer.tokenType() == TokenType::KEYWORD)
    {
        if (tokenizer.keyword() == Keyword::INT) type = "int";
        else if (tokenizer.keyword() == Keyword::CHAR) type = "char";
        else if (tokenizer.keyword() == Keyword::BOOLEAN) type = "boolean";
        eat(TokenType::KEYWORD);
    }
    else
    {
        type = tokenizer.identifier();
        eat(TokenType::IDENTIFIER);
    }
    
    std::string name = tokenizer.identifier();
    symbolTable.define(name, type, SymbolKind::VAR);
    eat(TokenType::IDENTIFIER);
    
    while (tokenizer.tokenType() == TokenType::SYMBOL && tokenizer.symbol() == ',')
    {
        eat(',');
        name = tokenizer.identifier();
        symbolTable.define(name, type, SymbolKind::VAR);
        eat(TokenType::IDENTIFIER);
    }
    eat(';');
}

void CompilationEngine::compileStatements()
{
    while (tokenizer.tokenType() == TokenType::KEYWORD)
    {
        Keyword k = tokenizer.keyword();
        if (k == Keyword::LET) compileLet();
        else if (k == Keyword::IF) compileIf();
        else if (k == Keyword::WHILE) compileWhile();
        else if (k == Keyword::DO) compileDo();
        else if (k == Keyword::RETURN) compileReturn();
        else break;
    }
}

void CompilationEngine::compileLet()
{
    eat(Keyword::LET);
    std::string varName = tokenizer.identifier();
    eat(TokenType::IDENTIFIER);
    
    bool isArray = false;
    if (tokenizer.tokenType() == TokenType::SYMBOL && tokenizer.symbol() == '[')
    {
        isArray = true;
        eat('[');
        
        Segment seg;
        int index = symbolTable.indexOf(varName);
        SymbolKind kind = symbolTable.kindOf(varName);
        if (kind == SymbolKind::FIELD) seg = Segment::THIS;
        else if (kind == SymbolKind::STATIC) seg = Segment::STATIC;
        else if (kind == SymbolKind::VAR) seg = Segment::LOCAL;
        else seg = Segment::ARG;
        
        vmWriter.writePush(seg, index);
        
        compileExpression();
        eat(']');
        
        vmWriter.writeArithmetic(Command::ADD);
    }
    
    eat('=');
    compileExpression();
    eat(';');
    
    if (isArray)
    {
        vmWriter.writePop(Segment::TEMP, 0);
        vmWriter.writePop(Segment::POINTER, 1);
        vmWriter.writePush(Segment::TEMP, 0);
        vmWriter.writePop(Segment::THAT, 0);
    }
    else
    {
        Segment seg;
        int index = symbolTable.indexOf(varName);
        SymbolKind kind = symbolTable.kindOf(varName);
        if (kind == SymbolKind::FIELD) seg = Segment::THIS;
        else if (kind == SymbolKind::STATIC) seg = Segment::STATIC;
        else if (kind == SymbolKind::VAR) seg = Segment::LOCAL;
        else seg = Segment::ARG;
        
        vmWriter.writePop(seg, index);
    }
}

void CompilationEngine::compileIf()
{
    std::string l1 = generateLabel();
    std::string l2 = generateLabel();
    
    eat(Keyword::IF);
    eat('(');
    compileExpression();
    eat(')');
    
    vmWriter.writeArithmetic(Command::NOT);
    vmWriter.writeIf(l1);
    
    eat('{');
    compileStatements();
    eat('}');
    
    vmWriter.writeGoto(l2);
    vmWriter.writeLabel(l1);
    
    if (tokenizer.tokenType() == TokenType::KEYWORD && tokenizer.keyword() == Keyword::ELSE)
    {
        eat(Keyword::ELSE);
        eat('{');
        compileStatements();
        eat('}');
    }
    
    vmWriter.writeLabel(l2);
}

void CompilationEngine::compileWhile()
{
    std::string l1 = generateLabel();
    std::string l2 = generateLabel();
    
    vmWriter.writeLabel(l1);
    eat(Keyword::WHILE);
    eat('(');
    compileExpression();
    eat(')');
    
    vmWriter.writeArithmetic(Command::NOT);
    vmWriter.writeIf(l2);
    
    eat('{');
    compileStatements();
    eat('}');
    
    vmWriter.writeGoto(l1);
    vmWriter.writeLabel(l2);
}

void CompilationEngine::compileDo()
{
    eat(Keyword::DO);
    compileTerm(); // This handles the subroutine call
    eat(';');
    vmWriter.writePop(Segment::TEMP, 0); // Discard void return value
}

void CompilationEngine::compileReturn()
{
    eat(Keyword::RETURN);
    if (tokenizer.tokenType() != TokenType::SYMBOL || tokenizer.symbol() != ';')
    {
        compileExpression();
    }
    else
    {
        vmWriter.writePush(Segment::CONST, 0);
    }
    eat(';');
    vmWriter.writeReturn();
}

void CompilationEngine::compileExpression()
{
    compileTerm();
    while (tokenizer.tokenType() == TokenType::SYMBOL && 
           std::string("+-*/&|<>=").find(tokenizer.symbol()) != std::string::npos)
    {
        char op = tokenizer.symbol();
        eat(TokenType::SYMBOL);
        compileTerm();
        
        switch (op)
        {
        case '+': vmWriter.writeArithmetic(Command::ADD); break;
        case '-': vmWriter.writeArithmetic(Command::SUB); break;
        case '*': vmWriter.writeCall("Math.multiply", 2); break;
        case '/': vmWriter.writeCall("Math.divide", 2); break;
        case '&': vmWriter.writeArithmetic(Command::AND); break;
        case '|': vmWriter.writeArithmetic(Command::OR); break;
        case '<': vmWriter.writeArithmetic(Command::LT); break;
        case '>': vmWriter.writeArithmetic(Command::GT); break;
        case '=': vmWriter.writeArithmetic(Command::EQ); break;
        }
    }
}

void CompilationEngine::compileTerm()
{
    TokenType type = tokenizer.tokenType();
    if (type == TokenType::INT_CONST)
    {
        vmWriter.writePush(Segment::CONST, tokenizer.intVal());
        eat(TokenType::INT_CONST);
    }
    else if (type == TokenType::STRING_CONST)
    {
        std::string s = tokenizer.stringVal();
        vmWriter.writePush(Segment::CONST, s.length());
        vmWriter.writeCall("String.new", 1);
        for (char c : s)
        {
            vmWriter.writePush(Segment::CONST, (int)c);
            vmWriter.writeCall("String.appendChar", 2);
        }
        eat(TokenType::STRING_CONST);
    }
    else if (type == TokenType::KEYWORD)
    {
        Keyword k = tokenizer.keyword();
        if (k == Keyword::TRUE)
        {
            vmWriter.writePush(Segment::CONST, 0); // This is 0
            vmWriter.writeArithmetic(Command::NOT); // !0 = -1 (11...1)
        }
        else if (k == Keyword::FALSE || k == Keyword::NULL_VAL)
        {
            vmWriter.writePush(Segment::CONST, 0);
        }
        else if (k == Keyword::THIS)
        {
            vmWriter.writePush(Segment::POINTER, 0);
        }
        eat(TokenType::KEYWORD);
    }
    else if (type == TokenType::SYMBOL)
    {
        if (tokenizer.symbol() == '(')
        {
            eat('(');
            compileExpression();
            eat(')');
        }
        else if (tokenizer.symbol() == '-' || tokenizer.symbol() == '~')
        {
            char op = tokenizer.symbol();
            eat(TokenType::SYMBOL);
            compileTerm();
            if (op == '-') vmWriter.writeArithmetic(Command::NEG);
            else vmWriter.writeArithmetic(Command::NOT);
        }
    }
    else if (type == TokenType::IDENTIFIER)
    {
        std::string name = tokenizer.identifier();
        eat(TokenType::IDENTIFIER);
        
        if (tokenizer.tokenType() == TokenType::SYMBOL && tokenizer.symbol() == '[')
        {
            // Array access
            eat('[');
            
            Segment seg;
            int index = symbolTable.indexOf(name);
            SymbolKind kind = symbolTable.kindOf(name);
            if (kind == SymbolKind::FIELD) seg = Segment::THIS;
            else if (kind == SymbolKind::STATIC) seg = Segment::STATIC;
            else if (kind == SymbolKind::VAR) seg = Segment::LOCAL;
            else seg = Segment::ARG;
            
            vmWriter.writePush(seg, index);
            
            compileExpression();
            eat(']');
            
            vmWriter.writeArithmetic(Command::ADD);
            vmWriter.writePop(Segment::POINTER, 1);
            vmWriter.writePush(Segment::THAT, 0);
        }
        else if (tokenizer.tokenType() == TokenType::SYMBOL && (tokenizer.symbol() == '(' || tokenizer.symbol() == '.'))
        {
            // Subroutine call
            std::string funcName = name;
            int nArgs = 0;
            
            if (tokenizer.symbol() == '.')
            {
                eat('.');
                std::string subName = tokenizer.identifier();
                eat(TokenType::IDENTIFIER);
                
                // Check if name is a variable class type or a class name
                SymbolKind kind = symbolTable.kindOf(name);
                if (kind != SymbolKind::NONE)
                {
                    // Method call on object instance
                    Segment seg;
                    int index = symbolTable.indexOf(name);
                    if (kind == SymbolKind::FIELD) seg = Segment::THIS;
                    else if (kind == SymbolKind::STATIC) seg = Segment::STATIC;
                    else if (kind == SymbolKind::VAR) seg = Segment::LOCAL;
                    else seg = Segment::ARG;
                    
                    vmWriter.writePush(seg, index);
                    funcName = symbolTable.typeOf(name) + "." + subName;
                    nArgs = 1;
                }
                else
                {
                    // Static call
                    funcName = name + "." + subName;
                }
            }
            else
            {
                // Method call on this
                vmWriter.writePush(Segment::POINTER, 0);
                funcName = className + "." + name;
                nArgs = 1;
            }
            
            eat('(');
            nArgs += compileExpressionList();
            eat(')');
            
            vmWriter.writeCall(funcName, nArgs);
        }
        else
        {
            // Variable access
            Segment seg;
            int index = symbolTable.indexOf(name);
            SymbolKind kind = symbolTable.kindOf(name);
            
            if (kind == SymbolKind::FIELD) seg = Segment::THIS;
            else if (kind == SymbolKind::STATIC) seg = Segment::STATIC;
            else if (kind == SymbolKind::VAR) seg = Segment::LOCAL;
            else seg = Segment::ARG;
            
            vmWriter.writePush(seg, index);
        }
    }
}

int CompilationEngine::compileExpressionList()
{
    int nArgs = 0;
    if (tokenizer.tokenType() != TokenType::SYMBOL || tokenizer.symbol() != ')')
    {
        compileExpression();
        nArgs++;
        while (tokenizer.tokenType() == TokenType::SYMBOL && tokenizer.symbol() == ',')
        {
            eat(',');
            compileExpression();
            nArgs++;
        }
    }
    return nArgs;
}

