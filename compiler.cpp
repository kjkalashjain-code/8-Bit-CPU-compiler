#include <bits/stdc++.h>
#include <iostream>
#include <fstream> // used for input and output for a file
#include <sstream> // for stringstream

#include "header/lexer.hpp"
#include "header/parser.hpp"
#include "header/assembly.hpp"
using namespace std;


int parserdepth(AST_NODE *node)  //to get the depth of the parse tree
{
    if (node == nullptr){

        return 0;
    }

    int leftd = parserdepth(node->LEFT);
    int rightd = parserdepth(node->RIGHT);

    int maxsub = 0;
    for (AST_NODE *stmt : node->SUB_STATEMENTS)
    {
        int d = parserdepth(stmt);
        if (d > maxsub)
            maxsub = d;
    }

    return 1 + max({leftd, rightd, maxsub});
}




int main(int argc , char** argv){//argc :- total strings in input file including name ./lexer input.sl     argv :- picks single string 


    const char* filename = "input.sl";//input file


    



    cout<<"reading from :- "<<filename<<endl;//the file has something to execute and we are reading now


    ifstream sourcefilestream(filename);//used to raed and store it in sourcefilestream(variable)   it works as a pipline to connect input file and this file

    stringstream buffer; //to take input of string    it stores everything whch is in the input file      it can be used to give input and output

    char temp;

    while(sourcefilestream.get(temp)){
        buffer<<temp;                    //pushing the data in the buffer
    }

    string sourcecode = buffer.str();
    cout<<sourcecode<<endl;


    

Lexer lexer(sourcecode);
vector<Token*> tokens = lexer.tokenize();

Token* eoftoken = new Token;    //token EOF
eoftoken->Type = TOKEN_EOF;
eoftoken->VALUE = "file ended"; 
tokens.push_back(eoftoken);

for(Token* temp :tokens){
    cout<<temp->Type;
    cout<<" "<<typetostring(temp->Type);
    cout<<" "<<temp->VALUE;
    
    cout<<endl;

}

cout<<endl;

cout<<"[1]TOKENIZED successfully"<<endl;

cout<<endl;


Parser parser(tokens);
AST_NODE* ROOT = parser.parse();
cout<<"[1]PARSED successfully"<<endl;




Generator generator(ROOT , filename );
generator.generate();




cout<<endl;
cout<<"[1]ASSEMBLY GENERATED SUCCESSFULLY"<<endl;



cout<<endl;
cout<<"end......"<<endl; 

}