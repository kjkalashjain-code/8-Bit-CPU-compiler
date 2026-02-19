#include <bits/stdc++.h>
#include <iostream>
#include <sstream>

#ifndef __LEXER_H
#define __LEXER_H
#include <string>
using namespace std;

enum type 
{
    TOKEN_ID,            // variable
    TOKEN_CONDITION,     // if
    TOKEN_TYPE,          // int
    TOKEN_INT,           // 123
    TOKEN_EQUALS,        //=
    TOKEN_DOUBLE_EQUALS, //==
    TOKEN_PLUS,          //+
    TOKEN_MINUS,         //-
    TOKEN_SEMICOLON,     //;
    TOKEN_LEFT_PAREN,    //(
    TOKEN_RIGHT_PAREN,   //)
    TOKEN_LEFT_CB,       //{
    TOKEN_RIGHT_CB,      //}
    TOKEN_EOF            // end of file

};

struct Token
{

    enum type Type;
    string VALUE;
};
string typetostring(enum type Type)
{

    switch (Type)
    {
    case TOKEN_ID:
        return "TOKEN_ID";
    case TOKEN_CONDITION:
        return "TOKEN_CONDITION";
    case TOKEN_TYPE:
        return "TOKEN_TYPE";
    case TOKEN_INT:
        return "TOKEN_INT";
    case TOKEN_EQUALS:
        return "TOKEN_EQUALS";
    case TOKEN_DOUBLE_EQUALS:
        return "TOKEN_DOUBLE_EQUALS";
    case TOKEN_PLUS:
        return "TOKEN_PLUS";
    case TOKEN_MINUS:
        return "TOKEN_MINUS";
    case TOKEN_SEMICOLON:
        return "TOKEN_SEMICOLON";
    case TOKEN_LEFT_PAREN:
        return "TOKEN_LEFT_PAREN";
    case TOKEN_RIGHT_PAREN:
        return "TOKEN_RIGHT_PAREN";
    case TOKEN_LEFT_CB:
        return "TOKEN_LEFT_CB";
    case TOKEN_RIGHT_CB:
        return "TOKEN_RIGHT_CB";
    case TOKEN_EOF:
        return "TOKEN_EOF";
    }
}
class Lexer
{
public:
    Lexer(string sourcecode)
    {
        source = sourcecode;
        cursor = 0; 
        size = sourcecode.length();
        current = sourcecode.at(cursor);
        current = (size > 0) ? sourcecode[0] : '\0'; // if the file in empty
    }

   void checkandskip()
{
    while (true) {
        // skip ASCII whitespace
        while (current != '\0' && isspace((unsigned char)current)) {
            advance();
        }

        // skip single-line comments starting with //
        if (current == '/' && peek(1) == '/') {
            // consume '//'
            advance(); advance();
            // skip until newline or EOF
            while (current != '\0' && current != '\n') advance();
            // continue outer loop to skip following whitespace/comments
            continue;
        }

        // nothing left to skip
        break;
    }
}

   Token* tokenizeIdentifier() {
    std::stringstream buffer;

    // Collect all valid identifier characters: letters, digits, underscores
    while (isalnum(current)) {
        buffer << advance();
    }

    string temp = buffer.str();
    Token* newtoken = new Token();

    // Check for keywords
    if (temp == "int") {
        newtoken->Type = TOKEN_TYPE;
        newtoken->VALUE = "int";
    } else if (temp == "if") {
        newtoken->Type = TOKEN_CONDITION;
        newtoken->VALUE = "if";
    } else {
        newtoken->Type = TOKEN_ID;
        newtoken->VALUE = temp;
    }

    return newtoken;
}

    Token *tokenizeINT()
    {
        stringstream buffer; // for 123
        while (isdigit(current))
        {

            buffer << advance();
        }
        Token *newtoken = new Token();
        newtoken->Type = TOKEN_INT;
        newtoken->VALUE = buffer.str();
        return newtoken;
    }

    Token* tokenizeSpecial(enum type Type) {
    Token* newtoken = new Token;
    newtoken->Type = Type;

    if (Type == TOKEN_DOUBLE_EQUALS) {
        newtoken->VALUE = "==";
        advance();
        advance();
        checkandskip(); //  skip after ==
    } else {
        newtoken->VALUE = string(1, advance());
        checkandskip(); //  skip after single-char symbol
    }

    return newtoken;
}

    vector<Token*> tokenize() {
    vector<Token*> tokens;

    while (cursor < size) {
        checkandskip(); // skip spaces and comments

        if (isalpha(current)) {
            tokens.push_back(tokenizeIdentifier());
            continue;
        }

        else if (isdigit(current)) {
            tokens.push_back(tokenizeINT());
            continue;
        }

        else {
            switch (current) {
                case ';': tokens.push_back(tokenizeSpecial(TOKEN_SEMICOLON)); break;
                case '(': tokens.push_back(tokenizeSpecial(TOKEN_LEFT_PAREN)); break;
                case ')': tokens.push_back(tokenizeSpecial(TOKEN_RIGHT_PAREN)); break;
                case '{': tokens.push_back(tokenizeSpecial(TOKEN_LEFT_CB)); break;
                case '}': tokens.push_back(tokenizeSpecial(TOKEN_RIGHT_CB)); break;
                case '=':
                    if (peek(1) == '=') {
                        tokens.push_back(tokenizeSpecial(TOKEN_DOUBLE_EQUALS));
                    } else {
                        tokens.push_back(tokenizeSpecial(TOKEN_EQUALS));
                    }
                    break;
                case '+': tokens.push_back(tokenizeSpecial(TOKEN_PLUS)); break;
                case '-': tokens.push_back(tokenizeSpecial(TOKEN_MINUS)); break;

                default:
                    cout << "[!] LEXER ERROR : unidentified symbol: '" << current << "' at index " << cursor << endl;
                    exit(1);
            }
        }
    }

    // EOF token
    Token* eof = new Token();
    eof->Type = TOKEN_EOF;
    eof->VALUE = "EOF";
    tokens.push_back(eof);

    return tokens;
}
    char advance()
    {

        if (cursor < size)
        {
            char temp = current;
            cursor++;                                          //  to move the cursor
            current = (cursor < size) ? source[cursor] : '\0'; // to the next
            return temp;
        }
        else
        {
            current = '\0';
            return '\0';
        }
    }

    char peek(int offset = 0)
    {

        if (cursor + offset < size)
        {
            return source[cursor + offset]; // for value next to cursor
        }

        else
        {
            return '\0';
        }
    }

private:
    string source;
    int cursor;
    int size;
    char current;
};

#endif
