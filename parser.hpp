#ifndef __PARSER_H
#define __PARSER_H

using namespace std;

#include <bits/stdc++.h>
#include <iostream>
#include <sstream>
#include "lexer.hpp"

enum NODE_TYPE
{
    NODE_ROOT, //holds all statements

    // Declarations
    NODE_DECL, // int a ; statement
    NODE_TYPE, // int
    NODE_ID,   // a

    // Literals
    NODE_INT, //123

    // Assignments
    NODE_ASSIGN, // assignment: a = 123; statement

    // Expressions
   
    NODE_EQUALS,        // assignment operator (=)
    NODE_DOUBLE_EQUALS, // equality operator (==)
    NODE_PLUS,          // addition operator (+)
    NODE_MINUS,         // subtraction operator (-)

    // Control flow
    NODE_IF,        // if statement
    NODE_CONDITION, // condition inside if
    NODE_BLOCK,     // block of statements { ... }

    // Utility
    NODE_SEMICOLON // ;
};

struct AST_NODE
{

    enum NODE_TYPE TYPE;
    string *VALUE;
    AST_NODE *LEFT = nullptr;
    AST_NODE *RIGHT = nullptr;
    vector<AST_NODE *> SUB_STATEMENTS; // only for root
};

class Parser
{
public:
    Parser(vector<Token *> tokens)  //constructor
    {
        parsertokens = tokens;
        index = 0;
        limit = parsertokens.size();
        current = parsertokens.at(index);
    }

    Token *proceed(enum type TYPE)
    {
        if (current->Type != TYPE)
        {
            cout << "[!] SYNTAX ERROR" << endl;
            exit(1);
        }
        else
        {
            index++;
            current = parsertokens.at(index);
            return current;
        }
    }

    AST_NODE *parse()
    {
        AST_NODE *ROOT = new AST_NODE();
        ROOT->TYPE = NODE_ROOT;

        while (current->Type != TOKEN_EOF)
        {

            ROOT->SUB_STATEMENTS.push_back(parse_statement());
        }

        return ROOT;
    }

private:
    int limit;
    int index;
    Token *current;
    vector<Token *> parsertokens;

    AST_NODE *parse_statement()
    {
        if (current->Type == TOKEN_TYPE)
        {
            return parse_declaration();
        }
        else if (current->Type == TOKEN_ID)
        {
            return parse_assignment();
        }
        else if (current->Type == TOKEN_CONDITION)
        {
            return parse_if();
        }
        else
        {
            cout << "[!] Unexpected token :- " << current->VALUE << endl;
            exit(1);
        }
    }

    AST_NODE *parse_declaration()
    {
        AST_NODE *node = new AST_NODE;
        node->TYPE = NODE_DECL;

        AST_NODE *typenode = new AST_NODE;
        typenode->TYPE = NODE_TYPE;
        typenode->VALUE = new string(current->VALUE);
        proceed(TOKEN_TYPE);

        AST_NODE *idnode = new AST_NODE;
        idnode->TYPE = NODE_ID;
        idnode->VALUE = new string(current->VALUE);
        proceed(TOKEN_ID);
        proceed(TOKEN_SEMICOLON);

        node->LEFT = typenode;
        node->RIGHT = idnode; // int -> node -> id
        return node;
    }

    AST_NODE *parse_assignment()
    {
        AST_NODE *node = new AST_NODE();
        node->TYPE = NODE_ASSIGN;

        AST_NODE *idnode = new AST_NODE;
        idnode->TYPE = NODE_ID;
        idnode->VALUE = new string(current->VALUE);
        proceed(TOKEN_ID);

        proceed(TOKEN_EQUALS);

        AST_NODE *exprnode = parse_expression();

        proceed(TOKEN_SEMICOLON);

        node->LEFT = idnode;
        node->RIGHT = exprnode;
        return node;
    }

    AST_NODE *parse_expression() // a = a+2;
    {
        AST_NODE *left;


        if (current->Type == TOKEN_INT)
        {
            left = new AST_NODE();
            left->TYPE = NODE_INT;
            left->VALUE = new string(current->VALUE);
            proceed(TOKEN_INT);
        }

        else if (current->Type == TOKEN_ID)
        {
            left = new AST_NODE;
            left->TYPE = NODE_ID;
            left->VALUE = new string(current->VALUE);
            proceed(TOKEN_ID);
        }
        else
        {
            cout << "[!] INVALID SYNTAX :- " << current->VALUE << endl;
            exit(1);
        }

        AST_NODE *binop;
        if (current->Type == TOKEN_PLUS)
        {
            binop = new AST_NODE;
            binop->TYPE = NODE_PLUS;
            binop->VALUE = new string(current->VALUE);
            proceed(current->Type);

            binop->LEFT = left;
            binop->RIGHT = parse_expression();
            return binop;
        }
        else if (current->Type == TOKEN_MINUS)
        {
            binop = new AST_NODE;
            binop->TYPE = NODE_MINUS;
            binop->VALUE = new string(current->VALUE);
            proceed(current->Type);

            binop->LEFT = left;
            binop->RIGHT = parse_expression();
            return binop;
        }

        else if (current->Type == TOKEN_DOUBLE_EQUALS)
        {
            binop = new AST_NODE;
            binop->TYPE = NODE_DOUBLE_EQUALS;
            binop->VALUE = new string(current->VALUE);
            proceed(current->Type);

            binop->LEFT = left;
            binop->RIGHT = parse_expression();
            return binop;
        }
        return left;
    }

    AST_NODE *parse_if()
    {

        AST_NODE *node = new AST_NODE;
        node->TYPE = NODE_IF;
        proceed(TOKEN_CONDITION);
        proceed(TOKEN_LEFT_PAREN);

        AST_NODE *cond = parse_expression();

        proceed(TOKEN_RIGHT_PAREN);
        proceed(TOKEN_LEFT_CB);

        AST_NODE *block = new AST_NODE;
        block->TYPE = NODE_BLOCK;

        while (current->Type != TOKEN_RIGHT_CB)
        {
            block->SUB_STATEMENTS.push_back(parse_statement());
        }

        proceed(TOKEN_RIGHT_CB);

        node->LEFT = cond;
        node->RIGHT = block;
        return node;
    }
};

#endif
