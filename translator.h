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
string ExpPrime(string);
string Term();
string F();
string TermPrime(string);
void FuncDec();
void Variable();
void DecParam();
void AnyCode();
int DataType();
string String();
string RelationOP();
string Character();
string Num();
string Identifier();
void Statement();
void MultiDec(string);
string Operand();
string LogicalOperand();
string ConditionPrime();
string PrintType();
string PrintPrime();
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

// globals
vector<tuple<string, string, int>> symbols;
int tacLineCounter = 1;
int tacTempCounter = 0;
int varAddress = 0;
vector<string> TAC;

int emit(string s)
{
    string output = to_string(tacLineCounter) + ") " + s;

    TAC.push_back(output);

    return tacLineCounter++;
}

string newTemp()
{
    return "t" + to_string(++tacTempCounter);
}

void backpatch(int lineNo, int value)
{
    TAC[lineNo - 1] += to_string(value);
}

void write_TAC()
{
    ofstream three_address_code("TAC.txt");

     // write symbols in file
     vector<string> ::iterator itr;
     for (itr = TAC.begin(); itr != TAC.end(); ++itr)
        three_address_code << *itr<< '\n';

     three_address_code.close();
}


// helper function
void throw_exception(string caller_error)
{

    cout << "[ln: " << Token::look.curr_line << ", col: " << Token::look.col << "] " << caller_error << endl;
    exit(1);
}

// parse tree generation functions
int tabs = 0;
ofstream parse_tree("parsetree.txt"); // global file pointer

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

// symbol table generation
void write_symbol_table()
{
     ofstream symbol_table("parser-symboltable.txt");

     // write symbols in file
     vector<tuple<string, string, int>> ::iterator itr;
     for (itr = symbols.begin(); itr != symbols.end(); ++itr)
        symbol_table << get<0>(*itr) << " : " << get<1>(*itr) << " : " << get<2>(*itr) << '\n';


     symbol_table.close();
}

void add_to_symbol_table(string name, string type, int size)
{
    symbols.push_back(make_tuple(name, type, varAddress));
    // cout << "name: " << name << " type: " << type << " size: " << varAddress << endl;

    varAddress += size;
}

// parser code
void Translator()
{
    Program();

    parse_tree.close();
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
    DataType();
    match(":");
    Identifier();

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

    tabs++;

    string dtype = Token::look.lexeme;
    int size = DataType();

    PrintTabs(":");
    match(":");

    string id = Token::look.lexeme;
    Identifier();
    tabs--;

    add_to_symbol_table(dtype, id, size);
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
string Exp()
{

    tabs++;
    PrintTabs("Term");
    string term = Term();

    PrintTabs("ExpPrime");
    string secondTerm = ExpPrime(term);

    tabs--;

    return secondTerm;
}

// Exp` -> + Term Exp` | - Term Exp` | ^
string ExpPrime(string previous)
{
    string retVal;

    tabs++;
    if (Token::look.lexeme == "+")
    {
        string term = "";
        PrintTabs("+");
        match("+");
        term += "+";

        PrintTabs("Term");
        term += Term();

        string tempName = newTemp();
        emit(tempName + ":=" + previous + term);

        PrintTabs("ExpPrime");
        retVal = ExpPrime(tempName);
    }
    else if (Token::look.lexeme == "-")
    {
        string term = "";
        PrintTabs("-");
        match("-");
        term += "-";

        PrintTabs("Term");
        term += Term();

        string tempName = newTemp();
        emit(tempName + ":=" + previous + term);

        PrintTabs("ExpPrime");
        retVal = ExpPrime(tempName);
    }
    else
    {
        PrintTabs("^");
        retVal = previous;
    }

    --tabs;
    return retVal;
}

// Term -> F Term`
string Term()
{

    tabs++;
    PrintTabs("F");
    string term = F();

    PrintTabs("TermPrime");
    string secondTerm = TermPrime(term);
    tabs--;

    return secondTerm;
}

// Term` -> * F Term` | / F Term` | ^
string TermPrime(string previous)
{
    string retVal;

    tabs++;
    if (Token::look.lexeme == "*")
    {
        string term = "";
        PrintTabs("*");
        match("*");
        term += "*";

        PrintTabs("F");
        term += F();

        string tempName = newTemp();
        emit(tempName + ":=" + previous + term);

        PrintTabs("TermPrime");
        retVal = TermPrime(tempName);
    }
    else if (Token::look.lexeme == "/")
    {
        string term = "";
        PrintTabs("/");
        match("/");
        term += "/";

        PrintTabs("F");
        term += F();

        string tempName = newTemp();
        emit(tempName + ":=" + previous + term);

        PrintTabs("TermPrime");
        retVal = TermPrime(tempName);
    }
    else
    {
        PrintTabs("^");
        retVal = previous;
    }

    tabs--;
    return retVal;
}

// F -> Identifier | Num | ( Exp )
string F()
{
    string retVal;
    tabs++;
    if (Token::look.token == IDENTIFIER)
    {
        retVal = Identifier();
    }
    else if (Token::look.token == NUMBER)
    {
        retVal = Num();
    }
    else if (Token::look.lexeme == "(")
    {
        PrintTabs("(");
        match("(");

        PrintTabs("Exp");
        retVal = Exp();
        PrintTabs(")");
        match(")");
    }
    else
        throw_exception("Invalid expression found.");
    tabs--;

    return retVal;
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
    tabs++;

    int size;
    if (curr_dtype == "char")
        size = 1;
    else if (curr_dtype == "Integer")
        size = 4;

    if (Token::look.lexeme == ",")
    {
        PrintTabs(",");
        match(",");

        // save current declaration in symbol table
        string id = Token::look.lexeme;
        add_to_symbol_table(curr_dtype, id, size);

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
    string id = Identifier();

    PrintTabs(":=");
    match(":=");

    PrintTabs("Operand");
    string op = Operand();

    emit(id + ":=" + op);
    tabs--;
}

// Operand - > Character | EXP | funcCall
string Operand()
{
    tabs++;
    if (Token::look.token == CONST_LITERAL)
        return Character();
    else if (Token::look.token == IDENTIFIER && peek_token().lexeme == "(")
    {
        PrintTabs("FunctionCall");
        FunctionCall();
        return "func TAC not implemented.";
    }
    else
    {
        PrintTabs("Exp");
        return Exp();
    }
    tabs--;
}

// LOGICAL EXPRESSION
// Condition -> LogicalOperand ConditionPrime
string Condition()
{
    string retVal = "";
    tabs++;

    PrintTabs("LogicalOperand");
    retVal += LogicalOperand();

    PrintTabs("ConditionPrime");
    retVal += ConditionPrime();
    tabs--;

    return retVal;
}

// ConditionPrime -> RelationOP LogicalOperand | ^
string ConditionPrime()
{
    string retVal = "";
    tabs++;
    if (Token::look.token == RELATION_OPS)
    {
        retVal += RelationOP();

        PrintTabs("LogicalOperand");
        retVal += LogicalOperand();
    }
    else
        PrintTabs("^");

    tabs--;
    return retVal;
}

// LogicalOperand -> Identifier | Num | Character
string LogicalOperand()
{
    tabs++;
    if (Token::look.token == IDENTIFIER)
    {
        tabs--;
        return Identifier();
    }
    else if (Token::look.token == NUMBER)
    {
        tabs--;
        return Num();
    }
    else if (Token::look.token == CONST_LITERAL)
    {
        tabs--;
        return Character();
    }
    else
        throw_exception("Invalid Logical operand found.");
}

// LOOPS
// WhileLoop -> while Condition :  { AnyCode }
void WhileLoop()
{
    tabs++;
    PrintTabs("while");
    match("while");

    PrintTabs("Condition");
    string conditionString = Condition();

    PrintTabs(":");
    match(":");
    PrintTabs("{");
    match("{");

    emit("if " + conditionString + " goto " + to_string(tacLineCounter + 2));
    int whileStart = emit("goto ");

    PrintTabs("AnyCode");
    AnyCode();

    PrintTabs("}");
    match("}");

    emit("goto " + to_string(whileStart - 1));
    backpatch(whileStart, tacLineCounter);
    tabs--;
}

// I/O
// PrintStatement -> PrintType ( PrintPrime )
void PrintStatement()
{

    tabs++;
    PrintTabs("PrintType");
    string type = PrintType();

    PrintTabs("(");
    match("(");

    PrintTabs("PrintPrime");
    string op = PrintPrime();

    PrintTabs(")");
    match(")");

    emit("out " + op);

    if (type == "println")
        emit("out '\\n'");

    tabs--;
}

// PrintPrime -> Operand | String
string PrintPrime()
{
    string retVal = "";
    tabs++;
    if (Token::look.token == STRING)
        retVal += String();
    else
    {
        PrintTabs("Operand");
        retVal += Operand();
    }
    tabs--;

    return retVal;
}

// InputStatement -> In >> Identifier
void InputStatement()
{
    tabs++;
    PrintTabs("In");
    match("In");

    PrintTabs(">>");
    match(">>");

    string id = Identifier();

    emit("in " + id);
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
    string conditionString = Condition();
    emit("if " + conditionString + " goto " + to_string(tacLineCounter + 2));

    PrintTabs(":");
    match(":");

    PrintTabs("{");
    match("{");
    int ifFalse = emit("goto ");

    PrintTabs("AnyCode");
    AnyCode();

    PrintTabs("}");
    match("}");
    backpatch(ifFalse, tacLineCounter);
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
        string conditionString = Condition();
        emit("if " + conditionString + " goto " + to_string(tacLineCounter + 2));

        PrintTabs(":");
        match(":");
        PrintTabs("{");
        match("{");

        int ifFalse = emit("goto ");
        PrintTabs("AnyCode");
        AnyCode();

        PrintTabs("}");
        match("}");
        backpatch(ifFalse, tacLineCounter);

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
    string op = Operand();

    emit("ret " + op);
    tabs--;
}

// TOKENS
// DataType -> integer | char
int DataType()
{
    // match lexemes for data type
    if (Token::look.lexeme == "char")
    {
        PrintTabs(Token::look.lexeme);
        Token::look = next_token(Token::file_iter, Token::file_end);
        return 1;
    }
    else if (Token::look.lexeme == "Integer")
    {
        PrintTabs(Token::look.lexeme);
        Token::look = next_token(Token::file_iter, Token::file_end);
        return 4;
    }
    else
        throw_exception("Expected token 'char' or 'Integer' but found '" + Token::look.lexeme + "'");
}

// Identifier -> [identifier lexeme]
string Identifier()
{
    // match identifiers
    if (Token::look.token == IDENTIFIER)
    {
        string curr_lex = Token::look.lexeme;
        PrintTabs(Token::look.lexeme);
        Token::look = next_token(Token::file_iter, Token::file_end);
        return curr_lex;
    }
    else
        throw_exception("Expected an identifier but found '" + Token::look.lexeme + "'");
}

// Num -> [num lexeme]
string Num()
{
    // match numbers
    if (Token::look.token == NUMBER)
    {
        string curr_lex = Token::look.lexeme;
        PrintTabs(Token::look.lexeme);
        Token::look = next_token(Token::file_iter, Token::file_end);
        return curr_lex;
    }
    else
        throw_exception("Expected a number but found '" + Token::look.lexeme + "'");
}

// Character -> [char lexeme]
string Character()
{
    // match constant literals
    if (Token::look.token == CONST_LITERAL)
    {
        string curr_lex = Token::look.lexeme;
        PrintTabs(Token::look.lexeme);
        Token::look = next_token(Token::file_iter, Token::file_end);
        return curr_lex;
    }
    else
        throw_exception("Expected a constant literal but found '" + Token::look.lexeme + "'");
}

// RelationOP -> > | < | >= |  <= |  /= | =
string RelationOP()
{
    // match relational opeartors
    if (Token::look.token == RELATION_OPS)
    {
        string curr_lex = Token::look.lexeme;
        PrintTabs(Token::look.lexeme);
        Token::look = next_token(Token::file_iter, Token::file_end);
        return curr_lex;
    }
    else
        throw_exception("Expected a relational operator but found '" + Token::look.lexeme + "'");
}

// String -> [string lexeme]
string String()
{
    // match string literals
    if (Token::look.token == STRING)
    {
        string curr_lex = Token::look.lexeme;
        PrintTabs(Token::look.lexeme);
        Token::look = next_token(Token::file_iter, Token::file_end);
        return curr_lex;
    }
    else
        throw_exception("Expected a string literal but found '" + Token::look.lexeme + "'");
}

// PrintType -> print | println
string PrintType()
{
    if (Token::look.lexeme == "print" || Token::look.lexeme == "println")
    {
        string curr_lex = Token::look.lexeme;
        PrintTabs(Token::look.lexeme);
        Token::look = next_token(Token::file_iter, Token::file_end);
        return curr_lex;
    }
    else
        throw_exception("Expected token 'print' or 'println' but found '" + Token::look.lexeme + "'");
}
