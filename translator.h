// 17L-4125 Hamza Murad
// 17L-6315 Muhammad Wasiq

#include <iostream>
#include <streambuf>
#include <fstream>
#include <vector>
#include <string>
#include <ctype.h>
#include <iterator>
#include <unordered_map>
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
void LogicalOperand(vector<string>&);
void ConditionPrime(vector<string>&);
string PrintType();
string PrintPrime();
void IF(vector<int> &);
void Elif(vector<int> &);
void Else();
void VariableDec();
void VarAssign();
void WhileLoop();
void PrintStatement();
void FunctionCall();
void CallParams();
void MultiCallParams();

// globals
int varAddress = 0;
vector<string> TAC;
int tacLineCounter = 1;
int tacTempCounter = 0;
vector<tuple<int,int,int,int>> quad;
unordered_map<string, int> opcode_table;
unordered_map<string, tuple<string, int, int>> symbols;

void intialise_opcodes()
{
    opcode_table[":="] = 1;
    opcode_table["+"] = 2;
    opcode_table["-"] = 3;
    opcode_table["*"] = 4;
    opcode_table["/"] = 5;
    opcode_table["in"] = 6;
    opcode_table["out"] = 7;
    opcode_table["="] = 8;
    opcode_table[">="] = 9;
    opcode_table["<="] = 10;
    opcode_table[">"] = 11;
    opcode_table["<"] = 12;
    opcode_table["goto"] = 13;
    opcode_table["outln"] = 14;
}

string newTemp()
{
    return "t" + to_string(++tacTempCounter);
}

// helper function
void throw_exception(string caller_error)
{

    cout << "[ln: " << Token::look.curr_line << ", col: " << Token::look.col << "] " << caller_error << endl;
    exit(1);
}

// symbol table generation
void write_symbol_table()
{
    ofstream symbol_table("parser-symboltable.txt");

    // write symbols in file
    unordered_map<string, tuple<string, int, int>>::iterator itr;
    for (itr = symbols.begin(); itr != symbols.end(); ++itr)
        symbol_table << itr->first << " : " << get<0>(itr->second) << " : " << get<1>(itr->second) << " : " << get<2>(itr->second) << '\n';

    symbol_table.close();
}

void add_to_symbol_table(string type, string name, int size, int initial_value)
{
    if (symbols.find(name) == symbols.end())
        symbols[name] = make_tuple(type, varAddress, initial_value);
    else
        throw_exception("Re-declaration found for'" + name + "'");

    varAddress += size;
}

int get_symbol_address(string s)
{
    if(symbols.find(s) != symbols.end())
        return get<1>(symbols[s]);
    else
        throw_exception("No such symbol found:'" + s + "'");
}

void write_quads()
{
    ofstream machine_code("machine-code.txt");

    // write symbols in file
    for (int i = 0 ; i < quad.size(); i++)
        machine_code<< i + 1 << ") " << get<0>(quad[i]) << " " << get<1>(quad[i]) << " " << get<2>(quad[i]) << " " << get<3>(quad[i]) <<endl;

    machine_code.close();
}

int add_to_quads(int opcode, int op1, int op2, int op3)
{
    quad.push_back(make_tuple(opcode, op1, op2, op3));
    return quad.size();
}

bool isNumber(const string& str)
{
    for (char const &c : str) {
        if (std::isdigit(c) == 0) return false;
    }
    return true;
}

string handle_numeric_constants(string s)
{
    if(isNumber(s))
    {
        string tempName = newTemp();
        add_to_symbol_table("Integer", tempName, 4, stoi(s));
        return tempName;
    }
    else
        return s;
}

int emit(string s)
{
    string output = to_string(tacLineCounter) + ") " + s;

    TAC.push_back(output);

    return tacLineCounter++;
}


void backpatch(int lineNo, int value)
{
    TAC[lineNo - 1] += to_string(value);
}

void write_TAC()
{
    ofstream three_address_code("TAC.txt");

    // write symbols in file
    vector<string>::iterator itr;
    for (itr = TAC.begin(); itr != TAC.end(); ++itr)
        three_address_code << *itr << '\n';

    three_address_code.close();
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

    add_to_symbol_table(dtype, id, size, 0);
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

    return  secondTerm;
}

// Exp` -> + Term Exp` | - Term Exp` | ^
string ExpPrime(string previous)
{
    string retVal;

    tabs++;
    if (Token::look.lexeme == "+")
    {
        PrintTabs("+");
        match("+");

        PrintTabs("Term");
        string term = Term();

        previous = handle_numeric_constants(previous);
        term = handle_numeric_constants(term);

        string tempName = newTemp();
        emit(tempName + ":=" + previous + "+" + term);
        add_to_symbol_table("Integer", tempName, 4, 0);
        add_to_quads(opcode_table["+"], get_symbol_address(previous), get_symbol_address(term), get_symbol_address(tempName));

        PrintTabs("ExpPrime");
        retVal = ExpPrime(tempName);
    }
    else if (Token::look.lexeme == "-")
    {
        PrintTabs("-");
        match("-");

        PrintTabs("Term");
        string term = Term();

        previous = handle_numeric_constants(previous);
        term = handle_numeric_constants(term);

        string tempName = newTemp();
        emit(tempName + ":=" + previous + "-" + term);
        add_to_symbol_table("Integer", tempName, 4, 0);
        add_to_quads(opcode_table["-"], get_symbol_address(previous), get_symbol_address(term), get_symbol_address(tempName));

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
        PrintTabs("*");
        match("*");

        PrintTabs("F");
        string term = F();

        previous = handle_numeric_constants(previous);
        term = handle_numeric_constants(term);

        string tempName = newTemp();
        emit(tempName + ":=" + previous + "*" + term);
        add_to_symbol_table("Integer", tempName, 4, 0);
        add_to_quads(opcode_table["*"], get_symbol_address(previous), get_symbol_address(term), get_symbol_address(tempName));

        PrintTabs("TermPrime");
        retVal = TermPrime(tempName);
    }
    else if (Token::look.lexeme == "/")
    {
        PrintTabs("/");
        match("/");

        PrintTabs("F");
        string term = F();

        previous = handle_numeric_constants(previous);
        term = handle_numeric_constants(term);

        string tempName = newTemp();
        emit(tempName + ":=" + previous + "/" + term);
        add_to_symbol_table("Integer", tempName, 4, 0);
        add_to_quads(opcode_table["/"], get_symbol_address(previous), get_symbol_address(term), get_symbol_address(tempName));

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
        add_to_symbol_table(curr_dtype, id, size, 0);

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

    op = handle_numeric_constants(op);
    emit(id + ":=" + op);
    add_to_quads(opcode_table[":="], get_symbol_address(op), get_symbol_address(id), 0);
    tabs--;
}

// Operand - > Character | EXP | funcCall
string Operand()
{
    tabs++;
    if (Token::look.token == CONST_LITERAL)
        return Character();
    else
    {
        PrintTabs("Exp");
        return Exp();
    }
    tabs--;
}

// LOGICAL EXPRESSION
// Condition -> LogicalOperand ConditionPrime
vector<string> Condition()
{
    vector<string> retVal;
    tabs++;

    PrintTabs("LogicalOperand");
    LogicalOperand(retVal);

    PrintTabs("ConditionPrime");
    ConditionPrime(retVal);
    tabs--;

    return retVal;
}

// ConditionPrime -> RelationOP LogicalOperand | ^
void ConditionPrime(vector<string>& retVal)
{
    tabs++;
    if (Token::look.token == RELATION_OPS)
    {
        retVal.push_back(RelationOP());

        PrintTabs("LogicalOperand");
        LogicalOperand(retVal);
    }
    else
        PrintTabs("^");

    tabs--;
}

// LogicalOperand -> Identifier | Num | Character
void LogicalOperand(vector<string>& retVal)
{
    tabs++;
    if (Token::look.token == IDENTIFIER)
        retVal.push_back(Identifier());
    else if (Token::look.token == NUMBER)
        retVal.push_back(Num());
    else if (Token::look.token == CONST_LITERAL)
        retVal.push_back(Character());
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
    vector<string> conditionString = Condition();

    PrintTabs(":");
    match(":");
    PrintTabs("{");
    match("{");

    string op1 = handle_numeric_constants(conditionString[0]);
    string ro = conditionString[1];
    string op2 = handle_numeric_constants(conditionString[2]);

    int nextLine = tacLineCounter + 2;
    emit("if " + op1 + ro + op2  + " goto " + to_string(nextLine));
    add_to_quads(opcode_table[ro], get_symbol_address(op1), get_symbol_address(op2), nextLine);

    int whileStart = emit("goto ");
    int quadStart = add_to_quads(opcode_table["goto"], 0, 0, 0);

    PrintTabs("AnyCode");
    AnyCode();

    PrintTabs("}");
    match("}");

    emit("goto " + to_string(whileStart - 1));
    add_to_quads(opcode_table["goto"], quadStart - 1, 0, 0);

    backpatch(whileStart, tacLineCounter);
    quad[quadStart - 1] = make_tuple(opcode_table["goto"], tacLineCounter, 0, 0);

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

    op = handle_numeric_constants(op);
    emit("out " + op);
    add_to_quads(opcode_table["out"], get_symbol_address(op), 0, 0);

    if (type == "println")
    {
        emit("outln");
        add_to_quads(opcode_table["outln"], 0, 0, 0);
    }

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
    add_to_quads(opcode_table["in"], get_symbol_address(id), 0, 0);
    tabs--;
}

// BRANCHES
// IFStatement -> IF Elif Else
void IfStatement()
{
    vector<int> else_patch;

    tabs++;
    PrintTabs("IF");
    IF(else_patch);
    PrintTabs("Elif");
    Elif(else_patch);
    PrintTabs("Else");
    Else();
    tabs--;

    for (int i = 0; i < else_patch.size(); i++)
    {
        backpatch(else_patch[i], tacLineCounter);
        quad[else_patch[i]-1] = make_tuple(opcode_table["goto"], tacLineCounter, 0, 0);
    }
}

// IF -> if Condition : { AnyCode }
void IF(vector<int> &else_patch)
{
    tabs++;
    PrintTabs("if");
    match("if");

    PrintTabs("Condition");
    vector<string> conditionString = Condition();

    string op1 = handle_numeric_constants(conditionString[0]);
    string ro = conditionString[1];
    string op2 = handle_numeric_constants(conditionString[2]);

    int nextLine = tacLineCounter + 2;
    emit("if " + op1 + ro + op2 + " goto " + to_string(nextLine));
    add_to_quads(opcode_table[ro], get_symbol_address(op1), get_symbol_address(op2), nextLine);

    PrintTabs(":");
    match(":");

    PrintTabs("{");
    match("{");
    int ifFalse = emit("goto ");
    int quadFalse = add_to_quads(opcode_table["goto"], 0, 0, 0);

    PrintTabs("AnyCode");
    AnyCode();
    else_patch.push_back(emit("goto "));
    add_to_quads(opcode_table["goto"], 0, 0, 0);

    PrintTabs("}");
    match("}");
    backpatch(ifFalse, tacLineCounter);
    quad[quadFalse - 1] = make_tuple(opcode_table["goto"], tacLineCounter, 0, 0);

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
void Elif(vector<int> &else_patch)
{
    tabs++;
    if (Token::look.lexeme == "elif")
    {
        PrintTabs("elif");
        match("elif");

        PrintTabs("Condition");
        vector<string> conditionString = Condition();


        string op1 = handle_numeric_constants(conditionString[0]);
        string ro = conditionString[1];
        string op2 = handle_numeric_constants(conditionString[2]);

        int nextLine = tacLineCounter + 2;
        emit("if " + op1 + ro + op2 + " goto " + to_string(nextLine));
        add_to_quads(opcode_table[ro], get_symbol_address(op1), get_symbol_address(op2), nextLine);

        PrintTabs(":");
        match(":");
        PrintTabs("{");
        match("{");

        int ifFalse = emit("goto ");
        int quadFalse = add_to_quads(opcode_table["goto"], 0, 0, 0);

        PrintTabs("AnyCode");
        AnyCode();
        else_patch.push_back(emit("goto "));
        add_to_quads(opcode_table["goto"], 0, 0, 0);

        PrintTabs("}");
        match("}");

        backpatch(ifFalse, tacLineCounter);
        quad[quadFalse - 1] = make_tuple(opcode_table["goto"], tacLineCounter, 0, 0);


        PrintTabs("Elif");
        Elif(else_patch);
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
