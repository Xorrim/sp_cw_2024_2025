#pragma once

#include "header.hpp"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

extern struct Token* TokenTable;
extern int pos;

namespace AST {
    //  program :: = "STARTPROGRAM", "STARTBLOCK", { "VARIABLE", variable_declaration, ";" }, { statement }, "ENDBLOCK";
    struct astNode*  program();
    //  programBody ::= { statement };
    struct astNode*  programBody();
    //  variable_declaration ::= "INT16", variable_list;
    struct astNode*  variableDeclaration();
    //  variable_list ::= identifier, {",", identifier};
    struct astNode*  variableList();
    //  statement ::= input_statement | output_statement | assign_statement | if_else_statement | goto_statement | label_point | for_statement | while_statement | repeat_until_statement | compound_statement;
    struct astNode*  statement();
    //  input_statement ::= "INPUT", identifier;
    struct astNode*  inputStatement();
    //  output_statement ::= "OUTPUT", arithmetic_expression;    
    struct astNode*  outputStatement();
    //  arithmetic_expression :: = low_priority_expression{ low_priority_operator, low_priority_expression };
    struct astNode*  arithmeticExpression();
    //  low_priority_expression :: = middle_priority_expression{ middle_priority_operator, middle_priority_expression };
    struct astNode*  lowPriorityExpression();
    //  low_priority_operator :: = "+" | "-";
    //void lowPriorityOperator();

    //  middle_priority_expression :: = identifier | number | "(", arithmetic_expression, ")";
    struct astNode*  middlePriorityExpression();
    //  middle_priority_operator :: = "*" | "DIV" | "MOD";
    //struct astNode*  middlePriorityOperator();

    //  assign_statement :: = arithmetic_expression, "==>", identifier;
    struct astNode*  assignStatement();
    //  if_else_statement :: = "IF", "(", logical_expression, ")", statement, [ "ELSE", statement];
    struct astNode*  ifStatement();
    //  logical_expression :: = and_expression{ or_operator, and_expression };
    struct astNode*  logicalExpression();

    //  or_operator :: = "||";
    //struct astNode*  orOperator();

    //  and_expression :: = comparison{ and_operator, and_expression };
    struct astNode*  andExpression();

    //  and_operator :: = "&&";
    //  struct astNode*  andOperator();

    //  comparison :: = comparison_expression | [not_operator] "(", logical_expression, ")";
    struct astNode*  comparison();

    //  not_operator :: = "!!";
    //  struct astNode*  notOperator();

    //  comparison_expression :: = arithmetic_expression comparison_operator arithmetic_expression;
    struct astNode*  comparisonExpression();
    //  comparison_operator :: = "=" | "!=" | "<<" | ">>";
    //  struct astNode*  comparisonOperator();

    //  goto_statement :: = "GOTO", identifier;
    struct astNode*  gotoStatement();
    //  label_point :: = identifier, ":";
    struct astNode*  labelPoint();
    //  for_to_statement :: = "FOR", assign_statement, "TO" | "DOWNTO", arithmetic_expression, "DO", statement;
    struct astNode* forStatement();
    //  while_statement :: = "WHILE", logical_expression, { statement_in_while }, "END", "WHILE";
    struct astNode* whileStatement();
    //  statement_in_while :: = statement | "CONTINUE WHILE" | "EXIT WHILE";
    struct astNode* statementInWhile();
    struct astNode* whileBody();
    //  repeat_until_statement :: = "REPEAT", repeat_body, "UNTIL", "(", logical_expression, ")";
    struct astNode*  repeatStatement();
    //  repeat_body :: = { statement };
	struct astNode*  repeatBody();
    //  compoundStatement :: = "STARTBLOCK", { statement }, "ENDBLOCK";
    struct astNode*  compoundStatement();

    void deleteNode(struct astNode* node);
    struct astNode* createNode(enum TypeOfNode type, const char* name, struct astNode* left, struct astNode* right);
    void printAST(struct astNode* node, int level);
    void fPrintAST(FILE* outFile, struct astNode* node, int level);
    void match(enum TypeOfToken expectedType);
    struct astNode* astParser();    
}
