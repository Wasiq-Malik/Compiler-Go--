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

class Token
{
public:
    static int curr_line;
    static int curr_col;
    static int tab_space;
    static string::iterator file_begin;
    static string::iterator file_iter;

    TOKENS_TYPE token;
    string lexeme;
    int line;
    int col;

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
string::iterator Token::file_iter;
string::iterator Token::file_begin;

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
        while (curr + 1 != end && isdigit(*(curr + 1)))
        {
            curr++;
            curr_tok += *curr;
        }

        if (isalpha(*curr))
            return Token(INVALID, "Incorect Number Token");
        else
            return Token(NUMBER, curr_tok);

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
    Token::file_begin = str.begin();

    int token_count = 0;
    for (Token::file_iter = str.begin(); Token::file_iter != str.end(); Token::file_iter++)
    {
        // ignore white spaces in the source file
        if (*Token::file_iter == ' ' || *Token::file_iter == '\n' || *Token::file_iter == '\t')
        {
            if (*Token::file_iter == '\n')
            {
                Token::curr_line++;
                Token::curr_col = std::distance(Token::file_begin, Token::file_iter); // num of chars from start to new line
                Token::tab_space = 1;   // reset tab spaces counter
            }

            if (*Token::file_iter == '\t')
                Token::tab_space += 3;  // inc column location according to 4-spaces tabs

            continue;
        }

        // collect next token
        token_count += 1;
        Token my_token = next_token(Token::file_iter, str.end());
        if (my_token.token == INVALID)
        {
            cout << "[ln: " << my_token.curr_line << ", col: " << my_token.col << "] Token: " << my_token.to_string() << endl;
            cout << "Exiting ... " << endl;
            return 1;
        }
        cout << my_token.to_string() << endl;
        word_file << my_token.to_string() << endl;
    }

    cout << "Total number of tokens: " << token_count << endl;

    return 0;
}