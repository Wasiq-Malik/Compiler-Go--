// 17L-4125 Hamza Murad
// 17L-6315 Muhammad Wasiq

#include <iostream>
#include <streambuf>
#include <fstream>
#include <vector>
#include <string>
#include <ctype.h>

using namespace std;

enum TOKENS_TYPE
{
    DATA_TYPE,
    DECISION,
    LOOP,
    READ,
    WRITE,
    FUNCTION,
    ARITH_OPS,
    RELATION_OPS,
    INPUT_OP,
    IDENTIFIER,
    ASSIGNMENT,
    RETURN,
    CONST_LITERAL,
    STRING,
    NUMBER,
    BRACKET,
    PUNCTUATION,
    COMMENTS,
    INVALID
};

void RetStatement();
void IfStatement();
void InputStatement();
void ExpPrime();
void Term();
void F();
void TermPrime();
void FuncDec();
void Variable();
void Param();
void AnyCode();
void DataType();
void String();
void RelationOP();
void Character();
void Num();
void Identifier();
void Statement();
void MultiDec();
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

class Token
{
public:
    static int curr_line;
    static int curr_col;
    static int tab_space;
    static string::iterator file_begin;
    static string::iterator file_end;
    static string::iterator file_iter;
    static Token look;

    TOKENS_TYPE token;
    string lexeme;
    int line;
    int col;

    Token()
    {
        token = INVALID;
        lexeme = "";
        line = 0;
        col = 0;
    }

    Token(TOKENS_TYPE type, string lex)
    {
        token = type;
        lexeme = lex;
        line = curr_line;
        col = std::distance(Token::file_begin, Token::file_iter) - curr_col + Token::tab_space;
    }

    string to_string()
    {
        const string tokens_name[] = {"DATA_TYPE", "DECISION", "LOOP", "READ", "WRITE", "FUNCTION", "ARITH_OPS", "RELATION_OPS", "INPUT_OP", "IDENTIFIER", "ASSIGNMENT", "RETURN", "CONST_LITERAL", "STRING", "NUMBER", "BRACKET", "PUNCTUATION", "COMMENTS", "INVALID"};
        return "{" + tokens_name[this->token] + ", " + this->lexeme + "}";
    }
};

int Token::curr_line = 1;
int Token::curr_col = 1;
int Token::tab_space = 1;
Token Token::look = Token();
string::iterator Token::file_iter;
string::iterator Token::file_begin;
string::iterator Token::file_end;

bool is_dtype(const string &str)
{
    const vector<string> keywords{"Integer", "char"};
    for (const auto &keyword : keywords)
        if (keyword == str)
            return true;

    return false;
}

bool is_decision(const string &str)
{
    const vector<string> keywords{"if", "else", "elif"};
    for (const auto &keyword : keywords)
        if (keyword == str)
            return true;

    return false;
}

bool is_write(const string &str)
{
    const vector<string> keywords{"print", "println"};
    for (const auto &keyword : keywords)
        if (keyword == str)
            return true;

    return false;
}

Token next_token(string::iterator &curr, const string::iterator end)
{
    if (curr != Token::file_begin)
        curr++;

    // ignore white spaces in the source file
    while (*curr == ' ' || *curr == '\n' || *curr == '\t')
    {
        if (*curr == '\n')
        {
            Token::curr_line++;
            Token::curr_col = std::distance(Token::file_begin, curr); // num of chars from start to new line
            Token::tab_space = 1;                                     // reset tab spaces counter
        }

        if (*curr == '\t')
            Token::tab_space += 3; // inc column location according to 4-spaces tabs

        curr++;
    }

    // analyse tokens that start with an alphabet
    if (isalpha(*curr))
    {
        string curr_tok(1, *curr);
        while (curr + 1 != end && (isalpha(*(curr + 1)) || isdigit(*(curr + 1))))
        {
            curr++;
            curr_tok += *curr;
        }
        // check if current word is any keyword or not
        if (is_dtype(curr_tok))
            return Token(DATA_TYPE, curr_tok);
        else if (is_decision(curr_tok))
            return Token(DECISION, curr_tok);
        else if (is_write(curr_tok))
            return Token(WRITE, curr_tok);
        else if (curr_tok == "func")
            return Token(FUNCTION, curr_tok);
        else if (curr_tok == "In")
            return Token(READ, curr_tok);
        else if (curr_tok == "while")
            return Token(LOOP, curr_tok);
        else if (curr_tok == "ret")
            return Token(RETURN, curr_tok);
        else
            return Token(IDENTIFIER, curr_tok); // it's an identifier if not a keyword
    }

    // analyse tokens that start with a digit
    if (isdigit(*curr))
    {
        string curr_tok(1, *curr);
        curr++;
        while (curr != end && isdigit(*curr))
        {
            curr_tok += *curr;
            curr++;
        }

        if (isalpha(*curr))
            return Token(INVALID, "Incorect Number Token");
        else
        {
            curr--;
            return Token(NUMBER, curr_tok);
        }
    }

    // analyse tokens that start with symbols
    switch (*curr)
    {
    case '(':
        return Token(BRACKET, string("("));
    case ')':
        return Token(BRACKET, string(")"));
    case '[':
        return Token(BRACKET, string("["));
    case ']':
        return Token(BRACKET, string("]"));
    case '{':
        return Token(BRACKET, string("{"));
    case '}':
        return Token(BRACKET, string("}"));
    case '>':
        if (curr + 1 != end)
        {
            curr++;
            if (*curr == '=')
                return Token(RELATION_OPS, string(">="));
            else if (*curr == '>')
                return Token(INPUT_OP, string(">>"));
        }
        return Token(RELATION_OPS, ">");
    case '<':
        if (curr + 1 != end && *(curr + 1) == '=')
        {
            curr++;
            return Token(RELATION_OPS, string("<="));
        }

        return Token(RELATION_OPS, string("<"));
    case '=':
        return Token(RELATION_OPS, string("="));
    case ':':
        if (curr + 1 != end && *(curr + 1) == '=')
        {
            curr++;
            return Token(ASSIGNMENT, string(":="));
        }
        return Token(PUNCTUATION, string(":"));
    case ';':
        return Token(PUNCTUATION, string(";"));
    case ',':
        return Token(PUNCTUATION, string(","));
    case '+':
        return Token(ARITH_OPS, string("+"));
    case '-':
        return Token(ARITH_OPS, string("-"));
    case '*':
        return Token(ARITH_OPS, string("*"));
    case '/':
        if (curr + 1 != end)
        {
            string curr_tok(1, *curr);
            char next_char = *(curr + 1);
            switch (next_char)
            {
            case '=':
                return Token(RELATION_OPS, "/=");
            case '*':
                // skip the comments
                curr++;
                curr_tok += *curr;
                while (curr + 1 != end && curr + 2 != end && !(*(curr + 1) == '*' && *(curr + 2) == '/'))
                {
                    if (*curr == '\n')
                        Token::curr_line++;
                    curr++;
                    curr_tok += *curr;
                }

                curr++;
                // if end token not found, invalid comment
                if (curr + 1 == end)
                    return Token(INVALID, curr_tok + string(1, *curr));

                curr++; // move pointer to ahead of comment
                return next_token(curr, end);
            }
        }

        return Token(ARITH_OPS, string("/"));
    case '\'':
        if (curr + 1 != end && isalpha(*(curr + 1)) && curr + 2 != end && *(curr + 2) == '\'')
        { // check for one letter constant literal
            string ch(1, *(curr + 1));
            curr += 2;
            return Token(CONST_LITERAL, "\'" + ch + "\'");
        }
        else
            return Token(INVALID, "Incorrect Constant Literal");

    case '\"':
        if (curr + 1 != end)
        { // check for n-length string literal
            string curr_tok(1, *curr);
            while (curr + 1 != end && *(curr + 1) != '\"')
            {
                curr++;
                curr_tok += *curr;
            }

            if (curr + 1 == end)
                return Token(INVALID, "Incorrect String Literal");

            curr++;
            return Token(STRING, curr_tok + "\"");
        }

    default: // incase no valid token found
        return Token(INVALID, string(1, *curr));
    }
}

void throw_exception()
{
    cout << "[ln: " << Token::look.curr_line << ", col: " << Token::look.col << "] Invalid Token: " << Token::look.to_string() << endl;
    throw "compiler crashed";
}

void match(string tok)
{
    cout << "Current look is: " << Token::look.to_string() << "\t\t";
    if (Token::look.lexeme == tok)
    {
        Token::look = next_token(Token::file_iter, Token::file_end);
        cout << "Next look is: " << Token::look.to_string() << endl;
    }
    else
        throw_exception();
}

void Program()
{
    if (Token::file_iter != Token::file_end)
    {
        FuncDec();
        Program();
    }
    else
        ;
}

//FUNCTION
//FuncDec -> func Variable ( Param ) { AnyCode }
void FuncDec()
{
    match("func");
    Variable();
    match("(");
    Param();
    match(")");
    match("{");
    AnyCode();
    match("}");
}

//Param -> DataType : Identifier  | , Variable Param | ^
void Param()
{
    if (Token::look.token == PUNCTUATION)
    {
        match(",");
        Variable();
        Param();
    }
    else if (Token::look.token == DATA_TYPE)
    {
        Variable();
        Param();
    }
    else
        ;
}

//Variable -> DataType : Identifier
void Variable()
{
    DataType();
    match(":");
    Identifier();
}

// AnyCode -> Statement ; AnyCode | ^
void AnyCode()
{

    if (Token::look.token == DATA_TYPE || Token::look.token == IDENTIFIER || Token::look.lexeme == "while" ||
        Token::look.lexeme == "print" || Token::look.lexeme == "println" || Token::look.lexeme == "In" ||
        Token::look.lexeme == "if" || Token::look.lexeme == "ret")
    {
        Statement();
        AnyCode();
    }
    else
        ;
}

//Statement ->  VariableDec | VarAssign | WhileLoop |
//PrintStatement | InputStatement | IFStatement | RetStatement
void Statement()
{
    if (Token::look.token == DATA_TYPE)
    {
        VariableDec();
        match(";");

    }
    else if (Token::look.token == IDENTIFIER)
    {
        VarAssign();
        match(";");
    }
    else if (Token::look.lexeme == "while")
    {
        WhileLoop();
    }
    else if (Token::look.lexeme == "print" || Token::look.lexeme == "println")
    {
        PrintStatement();
        match(";");
    }
    else if (Token::look.lexeme == "In")
    {
        InputStatement();
        match(";");
    }
    else if (Token::look.lexeme == "if")
    {
        IfStatement();
    }
    else if (Token::look.lexeme == "ret")
    {
        RetStatement();
        match(";");
    }
    else
        throw_exception();
}

// Exp -> Term Exp`
void Exp()
{
    Term();
    ExpPrime();
}

// Exp` -> + Term Exp` | - Term Exp` | ^
void ExpPrime()
{
    if (Token::look.lexeme == "+")
    {
        match("+");
        Term();
        ExpPrime();
    }
    else if (Token::look.lexeme == "-")
    {
        match("-");
        Term();
        ExpPrime();
    }
    else
        ;
}

// Term -> F Term`
void Term()
{
    F();
    TermPrime();
}

// Term` -> * F Term` | / F Term` | ^
void TermPrime()
{
    if (Token::look.lexeme == "*")
    {
        match("*");
        F();
        TermPrime();
    }
    else if (Token::look.lexeme == "/")
    {
        match("/");
        F();
        TermPrime();
    }
    else
        ;
}

// F -> Identifier | Num | ( Exp )
void F()
{
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
        match("(");
        Exp();
        match(")");
    }
    else
        throw_exception();
}

// VARIABLE DECLARATION
// VariableDec -> Datatype :  MultiDec
void VariableDec()
{
    DataType();
    match(":");
    Identifier();
    MultiDec();
}

// MultiDec -> Identifier | , Identifier MultiDec
void MultiDec()
{
    if (Token::look.lexeme == ",")
    {
        match(",");
        Identifier();
        MultiDec();
    }
    else;
}

// VarAssign -> Identifier :=  Operand
void VarAssign()
{
    Identifier();
    match(":=");
    Operand();
}

// Operand - > Character | EXP
void Operand()
{
    if (Token::look.token == CONST_LITERAL)
    {
        Character();
    }
    else
        Exp();
}

// LOGICAL EXPRESSION
// Condition -> LogicalOperand ConditionPrime
void Condition()
{
    LogicalOperand();
    ConditionPrime();
}

// ConditionPrime> RelationOP LogicalOperand | ^
void ConditionPrime()
{
    if (Token::look.token == RELATION_OPS)
    {
        RelationOP();
        LogicalOperand();
    }
    else
        ;
}

// Operand -> Identifier | Num | Character
void LogicalOperand()
{
    if (Token::look.token == IDENTIFIER)
        Identifier();
    else if (Token::look.token == NUMBER)
        Num();
    else if (Token::look.token == CONST_LITERAL)
        Character();
    else
        throw_exception();
}

// LOOPS
// WhileLoop -> while Condition :  { AnyCode }
void WhileLoop()
{
    match("while");
    Condition();
    match(":");
    match("{");
    AnyCode();
    match("}");
}

// I/O
// PrintStatement -> PrintType ( PrintPrime )
void PrintStatement()
{
    PrintType();
    match("(");
    PrintPrime();
    match(")");
}

// PrintPrime -> Operand | String
void PrintPrime()
{
    if (Token::look.token == STRING)
        String();
    else
        Operand();
}

// PrintType -> print | println
void PrintType()
{
    if (Token::look.lexeme == "print" || Token::look.lexeme == "println")
        Token::look = next_token(Token::file_iter, Token::file_end);
    else
        throw "Invalid tok, expected print";
}

// InputStatement -> In >> Identifier
void InputStatement()
{
    match("In");
    match(">>");
    Identifier();
}
// BRANCHES
// IFStatement -> IF Elif Else
void IfStatement()
{
    IF();
    Elif();
    Else();
}

// IF -> if Condition : { AnyCode }
void IF()
{
    match("if");
    Condition();
    match(":");
    match("{");
    AnyCode();
    match("}");
}

// Else -> else { AnyCode } | ^
void Else()
{
    if (Token::look.lexeme == "else")
    {
        match("else");
        match("{");
        AnyCode();
        match("}");
    }
    else
        ;
}

// Elif -> elif Condition : { AnyCode } Elif | ^
void Elif()
{
    if (Token::look.lexeme == "elif")
    {
        match("elif");
        Condition();
        match(":");
        match("{");
        AnyCode();
        match("}");
        Elif();
    }
    else
        ;
}

// RETURN
// RetStatement -> ret Operand
void RetStatement()
{
    match("ret");
    Operand();
}

// TOKENS
// DataType -> integer | char
void DataType()
{
    // match lexemes for data type
    if (Token::look.lexeme == "char" || Token::look.lexeme == "Integer")
        Token::look = next_token(Token::file_iter, Token::file_end);
    else
        throw_exception();
}

// Identifier -> [identifier lexeme]
void Identifier()
{
    if (Token::look.token == IDENTIFIER)
        Token::look = next_token(Token::file_iter, Token::file_end);
    else
        throw_exception();
}

// Num -> [num lexeme]
void Num()
{
    if (Token::look.token == NUMBER)
        Token::look = next_token(Token::file_iter, Token::file_end);
    else
        throw_exception();
}

// Character -> [char lexeme]
void Character()
{
    if (Token::look.token == CONST_LITERAL)
    {
        Token::look = next_token(Token::file_iter, Token::file_end);
    }
    else
        throw_exception();
}

// RelationOP -> > | < | >= |  <= |  /= | =
void RelationOP()
{
    if (Token::look.token == RELATION_OPS)
    {
        Token::look = next_token(Token::file_iter, Token::file_end);
    }
    else
        throw_exception();
}

// String -> [string lexeme]
void String()
{
    if (Token::look.token == STRING)
        Token::look = next_token(Token::file_iter, Token::file_end);
    else
        throw_exception();
}

int main()
{
    string file_path;
    cout << "Please Enter file path for Go-- Source file." << endl;
    cin >> file_path;

    if (file_path.find(string(".go")) == string::npos)
    {
        cout << "Please provide path to a go source file." << endl;
        return 1;
    }

    ifstream file(file_path);
    ofstream word_file("words.txt");

    if (!file.is_open())
    {
        cout << "File open() failed." << endl;
        cout << "Exiting ... " << endl;
        return 1;
    }

    string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    Token::file_begin = Token::file_iter = str.begin();
    Token::file_end = str.end();

    Token::look = next_token(Token::file_iter, Token::file_end);
    Program();

    if (Token::file_iter != Token::file_end)
    {
        throw "eof error";
        return 1;
    }

    return 0;
}
