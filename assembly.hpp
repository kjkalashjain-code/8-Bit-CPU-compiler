#ifndef __ASSEMBLY_H
#define __ASSEMBLY_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <set>
#include "lexer.hpp"
#include "parser.hpp"

using namespace std;

// Generator that emits assembly for the lightcode/8bit-computer.
// Assumptions (kept simple and explicit):
// - Registers: A, B
// - Instructions used: ldi, mov, add, sub, cmp, jz, jnz, sta
// - Memory labels are emitted as "name: 0"
// - If the target assembler uses different mnemonics (lda/sta, je/jne), swap them easily.

class Generator
{
public:
    Generator(AST_NODE *rootNode, const string &outName)
        : AST_ROOT(rootNode), outBase(outName)
    {
        assemblyfile.open(outBase + ".asm");
        if (!assemblyfile.is_open())
        {
            cerr << "[!] Failed to open output file: " << outBase << ".asm\n";
            exit(1);
        }
    }

    void generate()
    {
        collectVariables(AST_ROOT);

        // Data (memory) section
        sectionData << "; data (memory) definitions\n";
        for (const string &var : variables)
        {
            sectionData << var << ": 0\n";
        }
        sectionData << "\n; program\n";

        // Code section
        sectionText << "; code start\n";

        for (AST_NODE *currentNode : AST_ROOT->SUB_STATEMENTS)
        {
            switch (currentNode->TYPE)
            {
            case NODE_DECL:
                // already emitted in data section
                break;

            case NODE_ASSIGN:
            {
                string varname = *(currentNode->LEFT->VALUE);
                generateExpressionToRegister(currentNode->RIGHT, "A");
                sectionText << "    sta " << varname << "\n";
                break;
            }

            case NODE_IF:
            {
                string endLabel = newLabel("endif");
                AST_NODE *cond = currentNode->LEFT;

                if (cond && cond->TYPE == NODE_DOUBLE_EQUALS)
                {
                    // equality: cmp A B ; if not equal -> skip block
                    generateCondition(cond);
                    sectionText << "    jnz " << endLabel << "\n"; // jump if not equal -> skip block
                }
                else
                {
                    // truthy: evaluate into A; if zero -> skip block
                    generateCondition(cond);
                    sectionText << "    jz " << endLabel << "\n"; // zero -> false -> skip
                }

                // Emit block statements
                for (AST_NODE *stmt : currentNode->RIGHT->SUB_STATEMENTS)
                {
                    switch (stmt->TYPE)
                    {
                    case NODE_ASSIGN:
                    {
                        string vn = *(stmt->LEFT->VALUE);
                        generateExpressionToRegister(stmt->RIGHT, "A");
                        sectionText << "    sta " << vn << "\n";
                        break;
                    }
                    case NODE_DECL:
                        // already in data section
                        break;
                    default:
                        sectionText << "    ; [!] Unhandled statement inside if-block\n";
                        break;
                    }
                }

                sectionText << endLabel << ":\n";
                break;
            }

            default:
                sectionText << "    ; [!] Unhandled top-level AST node\n";
                break;
            }
        }

        // Halt or end marker (assembler-specific; many 8-bit examples use a halt)
        sectionText << "\n; end\n";

        writeSections();
    }

private:
    stringstream sectionData;
    stringstream sectionText;
    AST_NODE *AST_ROOT;
    ofstream assemblyfile;
    string outBase;
    set<string> variables;

    int labelCounter = 0;
    string newLabel(const string &base)
    {
        return base + "_" + to_string(labelCounter++);
    }

    void writeSections()
    {
        assemblyfile << sectionData.str() << "\n";
        assemblyfile << sectionText.str();
        assemblyfile.close();
    }

    void collectVariables(AST_NODE *node)
    {
        if (!node) return;
        if (node->TYPE == NODE_DECL && node->RIGHT && node->RIGHT->VALUE)
        {
            variables.insert(*(node->RIGHT->VALUE));
        }
        for (AST_NODE *child : node->SUB_STATEMENTS)
            collectVariables(child);
        if (node->LEFT) collectVariables(node->LEFT);
        if (node->RIGHT) collectVariables(node->RIGHT);
    }

    // Generate expression result into targetReg ("A" or "B")
    void generateExpressionToRegister(AST_NODE *expr, const string &targetReg)
    {
        if (!expr) return;

        switch (expr->TYPE)
        {
        case NODE_INT:
        {
            sectionText << "    ldi " << targetReg << " " << *(expr->VALUE) << "\n";
            break;
        }
        case NODE_ID:
        {
            sectionText << "    mov " << targetReg << " " << *(expr->VALUE) << "\n";
            break;
        }
        case NODE_PLUS:
        {
            // left -> A, right -> B, add A B (result in A)
            generateExpressionToRegister(expr->LEFT, "A");
            generateExpressionToRegister(expr->RIGHT, "B");
            sectionText << "    add A B\n";
            if (targetReg != "A") sectionText << "    mov " << targetReg << " A\n";
            break;
        }
        case NODE_MINUS:
        {
            generateExpressionToRegister(expr->LEFT, "A");
            generateExpressionToRegister(expr->RIGHT, "B");
            sectionText << "    sub A B\n";
            if (targetReg != "A") sectionText << "    mov " << targetReg << " A\n";
            break;
        }
        case NODE_DOUBLE_EQUALS:
        {
            // left -> A, right -> B, cmp A B ; result left in A as 1/0 if needed
            generateExpressionToRegister(expr->LEFT, "A");
            generateExpressionToRegister(expr->RIGHT, "B");
            sectionText << "    cmp A B\n";
            // produce boolean in A: 1 if equal, 0 otherwise
            string done = newLabel("eq_done");
            sectionText << "    ldi A 0\n";
            sectionText << "    jnz " << done << "\n"; // if not zero (not equal) skip setting 1
            sectionText << "    ldi A 1\n";
            sectionText << done << ":\n";
            if (targetReg != "A") sectionText << "    mov " << targetReg << " A\n";
            break;
        }
        default:
            sectionText << "    ; [!] Unsupported expression node\n";
            break;
        }
    }

    // Generate condition: for equality emits cmp A B; for others evaluates into A and compares with 0
    void generateCondition(AST_NODE *cond)
    {
        if (!cond) return;

        if (cond->TYPE == NODE_DOUBLE_EQUALS)
        {
            generateExpressionToRegister(cond->LEFT, "A");
            generateExpressionToRegister(cond->RIGHT, "B");
            sectionText << "    cmp A B\n";
        }
        else
        {
            generateExpressionToRegister(cond, "A");
            sectionText << "    cmp A 0\n";
        }
    }
};

#endif