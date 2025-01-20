#pragma once

#include "header.hpp"

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

namespace Parser {
    void Parser();
    void Semantic();
    void match(enum TypeOfToken expectedType);
    unsigned int IdIdentification(struct id idTable[], struct Token TokenTable[], unsigned int tokenCount);
    unsigned int LabelIdentification(struct id labelTable[], struct Token TokenTable[], unsigned int tokenCount);
    void printIdentifiers(int num, struct id* table);
    void fprintIdentifiers(FILE* F, int num, struct id* table);

    //  program :: = "STARTPROGRAM", "STARTBLOCK", { "VARIABLE", variable_declaration, ";" }, { statement }, "ENDBLOCK";
    void program();
    //  programBody ::= { statement };
    void programBody();
    //  variable_declaration ::= "INT16", variable_list;
    void variableDeclaration();
    //  variable_list :: = identifier, { ",", identifier };
    void variableList();
    //  statement ::= input_statement | output_statement | assign_statement | if_else_statement | goto_statement | label_point | for_statement | while_statement | repeat_until_statement | compound_statement;
    void statement();
    //  input_statement ::= "INPUT", identifier;
    void inputStatement();
    //  output_statement ::= "OUTPUT", arithmetic_expression;    
    void outputStatement();
    //  arithmetic_expression :: = low_priority_expression{ low_priority_operator, low_priority_expression };
    void arithmeticExpression();
    //  low_priority_expression :: = middle_priority_expression{ middle_priority_operator, middle_priority_expression };
    void lowPriorityExpression();
    //  low_priority_operator :: = "+" | "-";
	//void lowPriorityOperator();

    //  middle_priority_expression :: = identifier | number | "(", arithmetic_expression, ")";
	void middlePriorityExpression();
    //  middle_priority_operator :: = "*" | "DIV" | "MOD";
	//void middlePriorityOperator();

    //  assign_statement :: = arithmetic_expression, "==>", identifier;
    void assignStatement();
    //  if_else_statement :: = "IF", "(", logical_expression, ")", statement, [ "ELSE", statement];
    void ifStatement();
    //  logical_expression :: = and_expression{ or_operator, and_expression };
    void logicalExpression();
    
    //  or_operator :: = "||";
    //void orOperator();
    
    //  and_expression :: = comparison{ and_operator, and_expression };
    void andExpression();
    
    //  and_operator :: = "&&";
    //  void andOperator();
    
    //  comparison :: = comparison_expression | [not_operator] "(", logical_expression, ")";
    void comparison();
    
    //  not_operator :: = "!!";
    //  void notOperator();
 
    //  comparison_expression :: = arithmetic_expression comparison_operator arithmetic_expression;
	void comparisonExpression();
    //  comparison_operator :: = "=" | "!=" | "<<" | ">>";
	//  void comparisonOperator();
    
    //  goto_statement :: = "GOTO", identifier;
    void gotoStatement();
    //  label_point :: = identifier, ":";
    void labelPoint();
    //  for_to_statement :: = "FOR", assign_statement, "TO" | "DOWNTO", arithmetic_expression, "DO", statement;
    void forStatement();
    //  while_statement :: = "WHILE", logical_expression, { statement }, "END", "WHILE";
    void whileStatement();
    void statementInWhile();
    //  repeat_until_statement :: = "REPEAT", { statement }, "UNTIL", "(", logical_expression, ")";
    void repeatStatement();
    //  compoundStatement :: = "STARTBLOCK", { statement }, "ENDBLOCK";
    void compoundStatement();
}