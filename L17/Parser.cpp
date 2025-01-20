#include "Parser.hpp"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern struct Token* TokenTable;
extern unsigned int TokensNum;

extern struct id* idTable;
extern unsigned int idNum;

extern struct id* labelTable;
extern unsigned int labelNum;

extern FILE* errorFile;

int pos = 0;

namespace Parser {
    void Parser() {
        program();
        printf("\nThe program is syntax correct.\n");
        fprintf(errorFile, "\nThe program is syntax correct.\n");
    }

    void Semantic() {
        idNum = IdIdentification(idTable, TokenTable, TokensNum);
        labelNum = LabelIdentification(labelTable, TokenTable, TokensNum);
        printf("\nThe program is semantic correct.\n");
        printf("\n%d labels found\n", labelNum);
        fprintf(errorFile, "\nThe program is semantic correct.\n");
        fprintf(errorFile, "\n%d labels found\n", labelNum);
        printIdentifiers(labelNum, labelTable);
        fprintIdentifiers(errorFile, labelNum, labelTable);
        printf("\n%d identifiers found\n", idNum);
        fprintf(errorFile, "\n%d identifiers found\n", idNum);
        printIdentifiers(idNum, idTable);
        fprintIdentifiers(errorFile, idNum, idTable);
    }

    void match(enum TypeOfToken expectedType) {
        if (TokenTable[pos].type == expectedType)
            pos++;
        else {
            printf("\nSyntax error in line %d, token number: %d : another type of lexeme was expected (expected: %s | current: %s).\n", TokenTable[pos].line, pos, lexemeTypeName(expectedType), lexemeTypeName(TokenTable[pos].type));
            fprintf(errorFile, "\nSyntax error in line %d, token number: %d : another type of lexeme was expected (expected: %s | current: %s).\n", TokenTable[pos].line, pos, lexemeTypeName(expectedType), lexemeTypeName(TokenTable[pos].type));
            exit(1);
        }
    }

    // функція записує оголошені ідентифікатори в таблицю ідентифікаторів idTable
    // повертає кількість ідентифікаторів
    // перевіряє чи усі використані ідентифікатори оголошені
    unsigned int IdIdentification(struct id idTable[], struct Token TokenTable[], unsigned int tokenCount) {
        unsigned int idCount = 0;
        unsigned int i = 0;

        while (TokenTable[i++].type != Variable && TokenTable[i].type != EndBlock);

        while (TokenTable[i++].type == Type) {
            while (TokenTable[i].type != Semicolon) {
                if (TokenTable[i].type == Identifier) {
                    int yes = 0;
                    for (unsigned int j = 0; j < idCount; j++) {
                        if (!strcmp(TokenTable[i].name, idTable[j].name)) {
                            yes = 1;
                            break;
                        }
                    }
                    if (yes == 1) {
                        printf("\nidentifier \"%s\" is already declared !\n", TokenTable[i].name);
                        return idCount;
                    }

                    if (idCount < MAX_IDENTIFIER) {
                        strcpy_s(idTable[idCount++].name, TokenTable[i++].name);
                    }
                    else {
                        printf("\nToo many identifiers !\n");
                        return idCount;
                    }
                }
                else
                    ++i;
            }
            ++i;
        }

        for (; i < tokenCount; ++i) {
            if (
                TokenTable[i].type == Identifier
                && TokenTable[i - 1].type != Goto
                && TokenTable[i + 1].type != Colon
                ) {
                bool yes = 0;
                for (unsigned int j = 0; j < idCount; ++j) {
                    if (!strcmp(TokenTable[i].name, idTable[j].name)) {
                        yes = 1;
                        break;
                    }
                }
                if (!yes) {
                    printf("\nSemantic Error In line %d, an undeclared identifier \"%s\"!", TokenTable[i].line, TokenTable[i].name);
                    fprintf(errorFile, "\nSemantic Error In line %d, an undeclared identifier \"%s\"!", TokenTable[i].line, TokenTable[i].name);
                    exit(1);
                }
            }

        }

        return idCount; // Повертає кількість  ідентифікаторів
    }

    void program() {
        match(StartProgram);
        match(StartBlock);
        //match(Variable);
        if (TokenTable[pos].type == Variable) {
            ++pos;  //  for VARIABLE
            variableDeclaration();
            match(Semicolon);
        }
        programBody();
        match(EndBlock);
    }

    void variableDeclaration() {
        match(Type);
        variableList();
    }

    void variableList() {
        match(Identifier);
        while (TokenTable[pos].type == Comma) {
            ++pos;  // for Comma
            match(Identifier);
        }

    }

    void statement() {
        switch (TokenTable[pos].type) {
        case Input:     inputStatement();   break;
        case Output:    outputStatement();  break;
        case If:        ifStatement();      break;
        case Goto:      gotoStatement();    break;
        case For:       forStatement();     break;
        case While:     whileStatement();   break;
        case Repeat:    repeatStatement();  break;
        case StartBlock:compoundStatement(); break;
        default: {
            if (TokenTable[pos + 1].type == Colon)
                labelPoint();
            else
                assignStatement();
        }
        }
    }

    void inputStatement() {
        ++pos;  // for Input
        match(Identifier);
    }

    void outputStatement() {
        ++pos;  // for Output
        arithmeticExpression();
    }

    void arithmeticExpression() {
        lowPriorityExpression();
        //lowPriorityOperator
        while (TokenTable[pos].type == Add || TokenTable[pos].type == Sub) {
            ++pos;  // for + or -
            lowPriorityExpression();
        }
    }

    void lowPriorityExpression() {
        middlePriorityExpression();
        //  middlePriorityOperator
        while (TokenTable[pos].type == Mul || TokenTable[pos].type == Mod || TokenTable[pos].type == Div) {
            ++pos;  // for * or Mod or Div
            middlePriorityExpression();
        }
    }

    void middlePriorityExpression() {
        switch (TokenTable[pos].type) {
        case Identifier:    ++pos;  break;
        case Number:        ++pos;  break;
        case LBracket: {
            ++pos;  //  for (
            arithmeticExpression();
            match(RBracket);  
            break;
        }
        default: {
            printf("\nSyntax error in line %d, token number: %d : middle priority operation was expected (current: %s).\n", TokenTable[pos].line, pos, lexemeTypeName(TokenTable[pos].type));
            fprintf(errorFile, "\nSyntax error in line %d, token number: %d : middle priority operation was expected (current: %s).\n", TokenTable[pos].line, pos, lexemeTypeName(TokenTable[pos].type));
            exit(1);
        }
        }
    }

    void assignStatement() {
        match(Identifier);
        match(Assign);
        arithmeticExpression();
    }

    void ifStatement() {
        match(If);
        match(LBracket);
        logicalExpression();
        match(RBracket);
        statement();
        if (TokenTable[pos].type == Else) {
            //match(Semicolon);
            ++pos;  //  for else
            statement();
        }
    }

    void logicalExpression() {
        andExpression();
        while (TokenTable[pos].type == Or) {
            ++pos;  //  for ||
            andExpression();
        }
    }

    void andExpression() {
        comparison();
        while (TokenTable[pos].type == And) {
            ++pos;  //  for &&
            andExpression();
        }
    }

    void comparison() {
        if (TokenTable[pos].type == Not) {
            ++pos;  //  for !
            match(LBracket);
            logicalExpression();
            match(RBracket);
        }
        else if (TokenTable[pos].type == LBracket) {
            ++pos;  //  for (
            logicalExpression();
            match(RBracket);
        } else {
            comparisonExpression();
        }        
    }

    void comparisonExpression() {
        arithmeticExpression();
        if (TokenTable[pos].type == Equality ||
            TokenTable[pos].type == NotEquality ||
            TokenTable[pos].type == Greate ||
            TokenTable[pos].type == Less
            ) {
            ++pos;  //  for "=" | "!=" | "<<" | ">>"

        }
        else {
            printf("\nSyntax error in line %d : Comparison operator was Expected, %s token gained).\n", TokenTable[pos].line, lexemeTypeName(TokenTable[pos].type));
            fprintf(errorFile, "\nSyntax error in line %d : Comparison operator was Expected, %s token gained).\n", TokenTable[pos].line, lexemeTypeName(TokenTable[pos].type));
            exit(1);
        }
        arithmeticExpression();
    }

    void gotoStatement() {
        match(Goto);
        match(Identifier);
    }

    void labelPoint() {
        match(Identifier);
        match(Colon);
    }

    void forStatement() {
        match(For);
        assignStatement();
        if (TokenTable[pos].type == To || TokenTable[pos].type == Downto) {
            ++pos;  //  for to or downto
        }
        else {
            printf("\nSyntax error in line %d : TO or DOWNTO was expected, %s token gained).\n", TokenTable[pos].line, lexemeTypeName(TokenTable[pos].type));
            fprintf(errorFile, "\nSyntax error in line %d : TO or DOWNTO was expected, %s token gained).\n", TokenTable[pos].line, lexemeTypeName(TokenTable[pos].type));
            exit(1);
        }
        arithmeticExpression();
        match(Do);
        statement();
    }

    void whileStatement() {
        match(While);
        logicalExpression();
        while (TokenTable[pos].type != End) {
            statementInWhile();
            //match(Semicolon);
        }
        //match(WEnd);
        ++pos;  //  for END
        match(While);
    }

    void statementInWhile() {
        switch (TokenTable[pos].type) {
        case Continue: {
            ++pos;  //  for CONTINUE
            match(While);
            break;
        }
        case Exit: {
            ++pos;  //  for EXIT
            match(While);
            break;
        }
        default: {
            statement();
            break;
        }
        }
    }

    void repeatStatement() {
        match(Repeat);
        while (TokenTable[pos].type != Until) {
            statement();
            //match(Semicolon);
        }
        //match(Until);
        ++pos;  //  for UNTIL
        match(LBracket);
        logicalExpression();
        match(RBracket);
    }

    void compoundStatement() {
        match(StartBlock);
        programBody();
        match(EndBlock);
    }

    void programBody() {
        while (TokenTable[pos].type != EndBlock) {
            statement();
            //match(Semicolon);
        }
    }

    void printIdentifiers(int num, struct id* table) {
        for (int i = 0; i < num; i++) {
            printf("%s\n", table[i].name);
        }
    }
    void fprintIdentifiers(FILE* F, int num, struct id* table) {
        for (int i = 0; i < num; i++) {
            fprintf(F, "%s\n", table[i].name);
        }
    }

    unsigned int LabelIdentification(struct id labelTable[], struct Token TokenTable[], unsigned int tokenCount) {
        unsigned int labelNum = 0;
        unsigned int i = 0;
        unsigned int start = 0;

        while (TokenTable[start++].type != StartProgram);

        i = start;
        while (TokenTable[i].type != EndBlock) {
            if (TokenTable[i].type == Identifier && TokenTable[i + 1].type == Colon) {
                strcpy_s(labelTable[labelNum++].name, TokenTable[i].name);
            }
            ++i;
        }

        i = start;
        while (TokenTable[i].type != EndBlock) {
            if (TokenTable[i].type == Identifier && TokenTable[i - 1].type == Goto) {
                bool found = false;
                for (unsigned int j = 0; j < labelNum; j++) {
                    if (strcmp(labelTable[j].name, TokenTable[i].name) == 0) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    printf("\n Semantic error: In line %d label %s is not defined\n", TokenTable[i].line, TokenTable[i].name);
                    fprintf(errorFile, "\n Semantic error: In line %d label %s is not defined\n", TokenTable[i].line, TokenTable[i].name);
                    exit(1);
                }
            }
            ++i;
        }

        return labelNum;
    }
}