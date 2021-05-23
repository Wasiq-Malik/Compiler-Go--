// 17L-4125 Hamza Murad
// 17L-6315 Muhammad Wasiq

#include <iostream>
#include <streambuf>
#include <fstream>
#include <vector>
#include <string>
#include <ctype.h>
#include <iterator>
#include <map>
#include "lexer.h"

using namespace std;

// CFG functions prototypes
void Program();
void RetStatement();
void IfStatement();
void InputStatement();
void ExpPrime();
void Term();
void F();
void TermPrime();
void FuncDec();
void Variable();
void DecParam();
void AnyCode();
void DataType();
void String();
void RelationOP();
void Character();
void Num();
void Identifier();
void Statement();
void MultiDec(string);
void Operand();
void LogicalOperand();
void ConditionPrime();
void PrintType();
void PrintPrime();
void IF();
void Elif();
void Else();
void VariableDec();
void VarAssign();
void WhileLoop();
void PrintStatement();
void FunctionCall();
void CallParams();
void MultiCallParams();

// helper function
void throw_exception(string caller_error)
{

    cout << "[ln: " << Token::look.curr_line << ", col: " << Token::look.col << "] " << caller_error << endl;
    exit(1);
}

// parse tree generation functions
int tabs = 0;
ofstream parse_tree("parsetree.txt");   // global file pointer

void PrintTabs(string s)
{
    // print tabs for each branch in the parse tree
    if (tabs >= 1)
    {
        parse_tree << "|";
        for (int i = 0; i < tabs - 1; i++)
            parse_tree << "-";

        parse_tree << "|";
    }

    parse_tree << s << endl;
}

// global hash table for symbols
map<string, string> symbols;

// symbol table generation
void write_symbol_table()
{
    ofstream symbol_table("parser-symboltable.txt");

    // write symbols in file
    map<string, string>::iterator itr;
    for (itr = symbols.begin(); itr != symbols.end(); ++itr)
    {
        symbol_table << itr->first << " : " << itr->second << '\n';
    }

    symbol_table.close();
}

void add_to_symbol_table(pair<string, string> sym)
{
    // if symbol already used as identifier before, raise error
    if (symbols.find(sym.first) == symbols.end())
        symbols.insert(sym);
    else
        throw_exception("Re-declaration found for identifier '" + sym.first + "'");
}

// parser code
void ParserWrapper()
{
    Program();
}

void match(string tok)
{
    // check if current token correct, and then fetch next
    if (Token::look.lexeme == tok)
        Token::look = next_token(Token::file_iter, Token::file_end);
    else
        throw_exception("Expected token '" + tok + "' but found '" + Token::look.lexeme + "'");
}

// Program -> FuncDec Program | ^
void Program()
{
    ++tabs;
    if (Token::file_iter != Token::file_end)
    {
        PrintTabs("FuncDec");
        FuncDec();

        PrintTabs("Program");
        Program();
    }
    else
        PrintTabs("^");

    --tabs;
}

//FUNCTIONS
//FuncDec -> func Variable ( DecParam ) { AnyCode }
void FuncDec()
{
    tabs++;
    PrintTabs("func");
    match("func");

    PrintTabs("Variable");
    Variable();

    PrintTabs("(");
    match("(");

    PrintTabs("DecParam");
    DecParam();

    PrintTabs(")");
    match(")");

    PrintTabs("{");
    match("{");

    PrintTabs("AnyCode");
    AnyCode();

    PrintTabs("}");
    match("}");
    tabs--;
}

//DecParam -> DataType : Identifier  | , Variable DecParam | ^
void DecParam()
{
    tabs++;
    if (Token::look.token == PUNCTUATION)
    {
        PrintTabs(",");
        match(",");
        PrintTabs("Variable");
        Variable();
        PrintTabs("DecParam");
        DecParam();
    }
    else if (Token::look.token == DATA_TYPE)
    {
        PrintTabs("Variable");
        Variable();
        PrintTabs("DecParam");
        DecParam();
    }
    else
    {
        PrintTabs("^");
        ;
    }
    tabs--;
}

//FuncCall -> Identifier ( CallParams )
void FunctionCall()
{
    tabs++;
    Identifier();

    PrintTabs("(");
    match("(");

    PrintTabs("CallParams");
    CallParams();

    PrintTabs(")");
    match(")");
    tabs--;
}

//CallParams -> Operand MultiCallParams | ^
void CallParams()
{
    tabs++;
    if (Token::look.lexeme == ")")
        PrintTabs("^");
    else
    {
        PrintTabs("Operand");
        Operand();

        PrintTabs("MultiCallParams");
        MultiCallParams();
    }
    tabs--;
}

//MultiCallParams -> , Operand MultiCallParams| ^
void MultiCallParams()
{
    tabs++;
    if (Token::look.lexeme == ",")
    {
        PrintTabs(",");
        match(",");
        PrintTabs("Operand");
        Operand();
        PrintTabs("MutliCallParams");
        MultiCallParams();
    }
    else
    {
        PrintTabs("^");
        ;
    }
    tabs--;
}


//Variable -> DataType : Identifier
void Variable()
{
    pair<string, string> curr_symbol;

    tabs++;

    curr_symbol.second = Token::look.lexeme;
    DataType();

    PrintTabs(":");
    match(":");

    curr_symbol.first = Token::look.lexeme;
    Identifier();
    tabs--;

    add_to_symbol_table(curr_symbol);
}

// AnyCode -> Statement ; AnyCode | ^
void AnyCode()
{
    tabs++;
    if (Token::look.token == DATA_TYPE || Token::look.token == IDENTIFIER || Token::look.lexeme == "while" ||
        Token::look.lexeme == "print" || Token::look.lexeme == "println" || Token::look.lexeme == "In" ||
        Token::look.lexeme == "if" || Token::look.lexeme == "ret")
    {
        PrintTabs("Statement");
        Statement();

        PrintTabs("AnyCode");
        AnyCode();
    }
    else
        PrintTabs("^");
    tabs--;
}

//Statement ->  VariableDec | VarAssign | WhileLoop | PrintStatement | InputStatement | IFStatement | RetStatement
void Statement()
{
    tabs++;
    if (Token::look.token == DATA_TYPE)
    {
        PrintTabs("VariableDec");
        VariableDec();

        PrintTabs(";");
        match(";");
    }
    else if (Token::look.token == IDENTIFIER && peek_token().lexeme == "(")
    {
        PrintTabs("FunctionCall");
        FunctionCall();

        PrintTabs(";");
        match(";");
    }
    else if (Token::look.token == IDENTIFIER)
    {
        PrintTabs("VarAssign");
        VarAssign();

        PrintTabs(";");
        match(";");
    }
    else if (Token::look.lexeme == "while")
    {
        PrintTabs("WhileLoop");
        WhileLoop();
    }
    else if (Token::look.lexeme == "print" || Token::look.lexeme == "println")
    {
        PrintTabs("PrintStatement");
        PrintStatement();

        PrintTabs(";");
        match(";");
    }
    else if (Token::look.lexeme == "In")
    {
        PrintTabs("InputStatement");
        InputStatement();

        PrintTabs(";");
        match(";");
    }
    else if (Token::look.lexeme == "if")
    {
        PrintTabs("IfStatement");
        IfStatement();
    }
    else if (Token::look.lexeme == "ret")
    {
        PrintTabs("RetStatement");
        RetStatement();

        PrintTabs(";");
        match(";");
    }
    else
        throw_exception("Invalid Code statement found");
    tabs--;
}

// ARITHMATIC EXPRESSION
// Exp -> Term Exp`
void Exp()
{
    tabs++;
    PrintTabs("Term");
    Term();

    PrintTabs("ExpPrime");
    ExpPrime();
    tabs--;
}

// Exp` -> + Term Exp` | - Term Exp` | ^
void ExpPrime()
{
    tabs++;
    if (Token::look.lexeme == "+")
    {
        PrintTabs("+");
        match("+");

        PrintTabs("Term");
        Term();

        PrintTabs("ExpPrime");
        ExpPrime();
    }
    else if (Token::look.lexeme == "-")
    {
        PrintTabs("-");
        match("-");

        PrintTabs("Term");
        Term();

        PrintTabs("ExpPrime");
        ExpPrime();
    }
    else
        PrintTabs("^");
    --tabs;
}

// Term -> F Term`
void Term()
{
    tabs++;
    PrintTabs("F");
    F();

    PrintTabs("TermPrime");
    TermPrime();
    tabs--;
}

// Term` -> * F Term` | / F Term` | ^
void TermPrime()
{
    tabs++;
    if (Token::look.lexeme == "*")
    {
        PrintTabs("*");
        match("*");

        PrintTabs("F");
        F();

        PrintTabs("TermPrime");
        TermPrime();
    }
    else if (Token::look.lexeme == "/")
    {
        PrintTabs("/");
        match("/");

        PrintTabs("F");
        F();

        PrintTabs("TermPrime");
        TermPrime();
    }
    else
    {
        PrintTabs("^");
    }
    tabs--;
}

// F -> Identifier | Num | ( Exp )
void F()
{
    tabs++;
    if (Token::look.token == IDENTIFIER)
    {
        Identifier();
    }
    else if (Token::look.token == NUMBER)
    {
        Num();
    }
    else if (Token::look.lexeme == "(")
    {
        PrintTabs("(");
        match("(");

        PrintTabs("Exp");
        Exp();

        PrintTabs(")");
        match(")");
    }
    else
        throw_exception("Invalid expression found.");
    tabs--;
}

// VariableDec -> Variable MultiDec
void VariableDec()
{
    string current_dtype = Token::look.lexeme;

    tabs++;
    PrintTabs("Variable");
    Variable();

    PrintTabs("MultiDec");
    MultiDec(current_dtype);
    tabs--;
}

// MultiDec -> Identifier | , Identifier MultiDec
void MultiDec(string curr_dtype)
{
    pair<string, string> curr_symbol;
    curr_symbol.second = curr_dtype;

    tabs++;
    if (Token::look.lexeme == ",")
    {
        PrintTabs(",");
        match(",");

        // save current declaration in symbol table
        curr_symbol.first = Token::look.lexeme;
        add_to_symbol_table(curr_symbol);

        Identifier();

        PrintTabs("MultiDec");
        MultiDec(curr_dtype);
    }
    else
    {
        PrintTabs("^");
        ;
    }
    tabs--;
}

// VarAssign -> Identifier :=  Operand
void VarAssign()
{
    tabs++;
    Identifier();
    PrintTabs(":=");
    match(":=");
    PrintTabs("Operand");
    Operand();
    tabs--;
}

// Operand - > Character | EXP | funcCall
void Operand()
{
    tabs++;
    if (Token::look.token == CONST_LITERAL)
        Character();
    else if (Token::look.token == IDENTIFIER && peek_token().lexeme == "(")
    {
        PrintTabs("FunctionCall");
        FunctionCall();
    }
    else
    {
        PrintTabs("Exp");
        Exp();
    }
    tabs--;
}

// LOGICAL EXPRESSION
// Condition -> LogicalOperand ConditionPrime
void Condition()
{
    tabs++;
    PrintTabs("LogicalOperand");
    LogicalOperand();

    PrintTabs("ConditionPrime");
    ConditionPrime();
    tabs--;
}

// ConditionPrime -> RelationOP LogicalOperand | ^
void ConditionPrime()
{
    tabs++;
    if (Token::look.token == RELATION_OPS)
    {
        RelationOP();

        PrintTabs("LogicalOperand");
        LogicalOperand();
    }
    else
        PrintTabs("^");
    tabs--;
}

// LogicalOperand -> Identifier | Num | Character
void LogicalOperand()
{
    tabs++;
    if (Token::look.token == IDENTIFIER)
    {
        Identifier();
    }
    else if (Token::look.token == NUMBER)
    {
        Num();
    }
    else if (Token::look.token == CONST_LITERAL)
    {
        Character();
    }
    else
        throw_exception("Invalid Logical operand found.");
    tabs--;
}

// LOOPS
// WhileLoop -> while Condition :  { AnyCode }
void WhileLoop()
{
    tabs++;
    PrintTabs("while");
    match("while");
    PrintTabs("Condition");
    Condition();
    PrintTabs(":");
    match(":");
    PrintTabs("{");
    match("{");
    PrintTabs("AnyCode");
    AnyCode();
    PrintTabs("}");
    match("}");
    tabs--;
}

// I/O
// PrintStatement -> PrintType ( PrintPrime )
void PrintStatement()
{
    tabs++;
    PrintTabs("PrintType");
    PrintType();

    PrintTabs("(");
    match("(");

    PrintTabs("PrintPrime");
    PrintPrime();

    PrintTabs(")");
    match(")");
    tabs--;
}

// PrintPrime -> Operand | String
void PrintPrime()
{
    tabs++;
    if (Token::look.token == STRING)
        String();
    else
    {
        PrintTabs("Operand");
        Operand();
    }
    tabs--;
}

// PrintType -> print | println
void PrintType()
{
    if (Token::look.lexeme == "print" || Token::look.lexeme == "println")
    {
        PrintTabs(Token::look.lexeme);
        Token::look = next_token(Token::file_iter, Token::file_end);
    }
    else
        throw_exception("Expected token 'print' or 'println' but found '" + Token::look.lexeme + "'");
}

// InputStatement -> In >> Identifier
void InputStatement()
{
    tabs++;
    PrintTabs("In");
    match("In");

    PrintTabs(">>");
    match(">>");

    Identifier();
    tabs--;
}

// BRANCHES
// IFStatement -> IF Elif Else
void IfStatement()
{
    tabs++;
    PrintTabs("IF");
    IF();
    PrintTabs("Elif");
    Elif();
    PrintTabs("Else");
    Else();
    tabs--;
}

// IF -> if Condition : { AnyCode }
void IF()
{
    tabs++;
    PrintTabs("if");
    match("if");

    PrintTabs("Condition");
    Condition();

    PrintTabs(":");
    match(":");

    PrintTabs("{");
    match("{");

    PrintTabs("AnyCode");
    AnyCode();

    PrintTabs("}");
    match("}");
    tabs--;
}

// Else -> else { AnyCode } | ^
void Else()
{
    tabs++;
    if (Token::look.lexeme == "else")
    {
        PrintTabs("else");
        match("else");
        PrintTabs("{");
        match("{");
        PrintTabs("AnyCode");
        AnyCode();
        PrintTabs("}");
        match("}");
    }
    else
    {
        PrintTabs("^");
        ;
    }
    tabs--;
}

// Elif -> elif Condition : { AnyCode } Elif | ^
void Elif()
{
    tabs++;
    if (Token::look.lexeme == "elif")
    {
        PrintTabs("elif");
        match("elif");
        PrintTabs("Condition");
        Condition();
        PrintTabs(":");
        match(":");
        PrintTabs("{");
        match("{");
        PrintTabs("AnyCode");
        AnyCode();
        PrintTabs("}");
        match("}");
        PrintTabs("Elif");
        Elif();
    }
    else
    {
        PrintTabs("^");
        ;
    }
    tabs--;
}

// RETURN
// RetStatement -> ret Operand
void RetStatement()
{
    tabs++;
    PrintTabs("ret");
    match("ret");

    PrintTabs("Operand");
    Operand();
    tabs--;
}

// TOKENS
// DataType -> integer | char
void DataType()
{
    // match lexemes for data type
    if (Token::look.lexeme == "char" || Token::look.lexeme == "Integer")
    {
        PrintTabs(Token::look.lexeme);
        Token::look = next_token(Token::file_iter, Token::file_end);
    }
    else
        throw_exception("Expected token 'char' or 'Integer' but found '" + Token::look.lexeme + "'");
}

// Identifier -> [identifier lexeme]
void Identifier()
{
    // match identifiers
    if (Token::look.token == IDENTIFIER)
    {
        PrintTabs(Token::look.lexeme);
        Token::look = next_token(Token::file_iter, Token::file_end);
    }
    else
        throw_exception("Expected an identifier but found '" + Token::look.lexeme + "'");
}

// Num -> [num lexeme]
void Num()
{
    // match numbers
    if (Token::look.token == NUMBER)
    {
        PrintTabs(Token::look.lexeme);
        Token::look = next_token(Token::file_iter, Token::file_end);
    }
    else
        throw_exception("Expected a number but found '" + Token::look.lexeme + "'");
}

// Character -> [char lexeme]
void Character()
{
    // match constant literals
    if (Token::look.token == CONST_LITERAL)
    {
        PrintTabs(Token::look.lexeme);
        Token::look = next_token(Token::file_iter, Token::file_end);
    }
    else
        throw_exception("Expected a constant literal but found '" + Token::look.lexeme + "'");
}

// RelationOP -> > | < | >= |  <= |  /= | =
void RelationOP()
{
    // match relational opeartors
    if (Token::look.token == RELATION_OPS)
    {
        PrintTabs(Token::look.lexeme);
        Token::look = next_token(Token::file_iter, Token::file_end);
    }
    else
        throw_exception("Expected a relational operator but found '" + Token::look.lexeme + "'");
}

// String -> [string lexeme]
void String()
{
    // match string literals
    if (Token::look.token == STRING)
    {
        PrintTabs(Token::look.lexeme);
        Token::look = next_token(Token::file_iter, Token::file_end);
    }
    else
        throw_exception("Expected a string literal but found '" + Token::look.lexeme + "'");
}
