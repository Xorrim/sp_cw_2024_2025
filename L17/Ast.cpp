#include "Ast.hpp"

extern struct Token* TokenTable;
extern int pos;

namespace AST {
    void deleteNode(struct astNode* node) {
        if (node == nullptr) return;
        deleteNode(node->left);
        deleteNode(node->right);
        free(node);
    }

    struct astNode* createNode(enum TypeOfNode type, const char* name, struct astNode* left, struct astNode* right) {
        struct astNode* node = (struct astNode*)malloc(sizeof(struct astNode));
        node->type = type;
        strcpy_s(node->name, name);
        node->left = left;
        node->right = right;
        return node;
    }

    void printAST(struct astNode* node, int level) {
        if (node == nullptr)
            return;

        for (int i = 0; i < level; i++)
            printf("|    ");

        printf("|-- %s(%d)", node->name, node->type);
        printf("\n");

        if (node->left || node->right)
        {
            printAST(node->left, level + 1);
            printAST(node->right, level + 1);
        }
    }

    void fPrintAST(FILE* outFile, struct astNode* node, int level) {
        if (node == nullptr)
            return;

        for (int i = 0; i < level; i++)
            fprintf(outFile, "|    ");

        fprintf(outFile, "|-- %s(%d)", node->name, node->type);
        fprintf(outFile, "\n");

        if (node->left || node->right)
        {
            fPrintAST(outFile, node->left, level + 1);
            fPrintAST(outFile, node->right, level + 1);
        }
    }

    void match(enum TypeOfToken expectedType) {
        if (TokenTable[pos].type == expectedType)
            pos++;
        else {
            printf("\nSyntax error in line %d : another type of lexeme was expected (expected: %s | current: %s).\n", TokenTable[pos].line, lexemeTypeName(expectedType), lexemeTypeName(TokenTable[pos].type));
            exit(1);
        }
    }

    struct astNode* astParser() {
        pos = 0;
        struct astNode* tree = program();

        printf("AST created.\n");

        return tree;
    }

    struct astNode* program() {
        match(StartProgram);
        match(StartBlock);
        struct astNode* declaration = nullptr;
        if (TokenTable[pos].type == Variable) {
            ++pos;  //  for VARIABLE
            declaration = variableDeclaration();
            match(Semicolon);
        }
        struct astNode* body = programBody();
        match(EndBlock);
        return createNode(program_node, "Program", declaration, body);
    }

    struct astNode* variableDeclaration() {
        match(Type);
        return variableList();
    }

    struct astNode* variableList() {
        match(Identifier);
        struct astNode* id = createNode(id_node, TokenTable[pos - 1].name, nullptr, nullptr);
        struct astNode* list = list = createNode(var_node, "var", id, nullptr);
        while (TokenTable[pos].type == Comma)
        {
            match(Comma);
            match(Identifier);
            id = createNode(id_node, TokenTable[pos - 1].name, nullptr, nullptr);
            list = createNode(var_node, "var", id, list);
        }
        return list;
    }

    struct astNode* programBody() {
        if (TokenTable[pos].type != EndBlock) {
            struct astNode* stmt = statement();
            //match(Semicolon);
            struct astNode* body = stmt;
            while (TokenTable[pos].type != EndBlock)
            {
                struct astNode* nextStmt = statement();
                //match(Semicolon);
                body = createNode(statement_node, "statement", body, nextStmt);
            }
            return body;
        }
        return nullptr;
    }

    struct astNode* statement() {
        switch (TokenTable[pos].type) {
        case Input:     return  inputStatement();
        case Output:    return  outputStatement();
        case If:        return  ifStatement();
        case Goto:      return  gotoStatement();
        case For:       return  forStatement();
        case While:     return  whileStatement();
        case Repeat:    return  repeatStatement();
        case StartBlock:return  compoundStatement();
        default: {
            if (TokenTable[pos + 1].type == Colon)
                return  labelPoint();
            else
                return  assignStatement();
        }
        }
    }

    struct astNode* inputStatement() {
        match(Input);
        match(Identifier);
        return createNode(input_node, "input", createNode(id_node, TokenTable[pos - 1].name, nullptr, nullptr), nullptr);
    }

    struct astNode* outputStatement() {
        match(Output);
        return createNode(output_node, "output", arithmeticExpression(), nullptr);
    }

    struct astNode* arithmeticExpression() {
        struct astNode* left = lowPriorityExpression();
        if (TokenTable[pos].type == Add || TokenTable[pos].type == Sub) {
            enum TypeOfToken op = TokenTable[pos].type;
            ++pos;  //  for add or sub
            struct astNode* right = arithmeticExpression();
            return createNode(op == Add ? add_node : sub_node, lexemeTypeName(op), left, right);
        }
        return left;
    }

    struct astNode* lowPriorityExpression() {
        struct astNode* left = middlePriorityExpression();
        if (TokenTable[pos].type == Mul || TokenTable[pos].type == Mod || TokenTable[pos].type == Div) {
            enum TypeOfToken op = TokenTable[pos].type;
            ++pos;  //  for mul or mod or div
            struct astNode* right = lowPriorityExpression();
            return createNode(op == Mul ? mul_node : op == Div ? div_node : mod_node, lexemeTypeName(op), left, right);
        }
        return left;
    }

    struct astNode* middlePriorityExpression() {
        switch (TokenTable[pos].type) {
        case Identifier:    match(Identifier);  return  createNode(id_node, TokenTable[pos - 1].name, nullptr, nullptr);
        case Number:        match(Number);      return  createNode(number_node, TokenTable[pos - 1].name, nullptr, nullptr);
        case LBracket: {
            ++pos;  //  for (
            struct astNode* expr = arithmeticExpression();
            match(RBracket);
            return expr;
        }
        default: {
            printf("\nSyntax error in line %d, token number: %d : middle priority operation was expected (current: %s).\n", TokenTable[pos].line, pos, lexemeTypeName(TokenTable[pos].type));
            exit(1);
        }
        }
    }

    struct astNode* assignStatement() {
        match(Identifier);
        struct astNode* left = createNode(id_node, TokenTable[pos - 1].name, nullptr, nullptr);
        match(Assign);
        struct astNode* right = arithmeticExpression();
		return createNode(assign_node, "assign", left, right);
    }

    struct astNode* ifStatement() {
        match(If);
        match(LBracket);
        struct astNode* expr = logicalExpression();
        match(RBracket);
        struct astNode* stmt = statement();
        if (TokenTable[pos].type == Else) {
            //match(Semicolon);
            ++pos;  //  for ELSE
			struct astNode* elseStmt = statement();
            stmt = createNode(else_node, "else", stmt, elseStmt);
        }
        return createNode(if_node, "if", expr, stmt);
    }

    struct astNode* logicalExpression() {
        struct astNode* left = andExpression();
        if (TokenTable[pos].type == Or) {
            ++pos;  //  for Or
            struct astNode* right = logicalExpression();
			return createNode(or_node, "or", left, right);
        }
        return left;
    }

    struct astNode* andExpression() {
        struct astNode* left = comparison();
        if (TokenTable[pos].type == And) {
			++pos;  //  for And
			struct astNode* right = andExpression();
			return createNode(and_node, "and", left, right);
        }
        return left;
    }

    struct astNode* comparison() {
        struct astNode* comp;
        switch (TokenTable[pos].type) {
        case Not: {
            ++pos;  //  for Not
            match(LBracket);
            comp = createNode(not_node, "not", logicalExpression(), nullptr);
            match(RBracket);
            break;
        }
        case LBracket: {
            ++pos;  //  for (
            comp = logicalExpression();
			match(RBracket);
            break;
        }
        default: {
            comp = comparisonExpression();
        }
        }
        return comp;
    }

    struct astNode* comparisonExpression() {
        struct astNode* left = arithmeticExpression();
        if (TokenTable[pos].type == Equality ||
            TokenTable[pos].type == NotEquality ||
            TokenTable[pos].type == Greate ||
            TokenTable[pos].type == Less
            ) {
            enum TypeOfToken op = TokenTable[pos].type;
            ++pos;  //  for Equality or NotEquality or Greate or Less
			struct astNode* right = arithmeticExpression();
			return createNode(op == Equality ? eq_node : op == NotEquality ? neq_node : op == Greate ? gr_node : ls_node, lexemeTypeName(op), left, right);
        }
        else {
            printf("\nSyntax error in line %d : Comparison operator was Expected, %s token gained).\n", TokenTable[pos].line, lexemeTypeName(TokenTable[pos].type));
            exit(1);
        }
    }

    struct astNode* gotoStatement() {
        match(Goto);
        match(Identifier);
        return createNode(goto_node, "goto", createNode(id_node, TokenTable[pos - 1].name, nullptr, nullptr), nullptr);
    }

    struct astNode* labelPoint() {
        match(Identifier);
        match(Colon);
        return createNode(label_node, "label", createNode(id_node, TokenTable[pos - 2].name, nullptr, nullptr), nullptr);
    }

    struct astNode* forStatement() {
        match(For);
        struct astNode* assign = assignStatement();
        switch (TokenTable[pos].type) {
        case To: {
            ++pos;  //  for To
            struct astNode* expr = arithmeticExpression();
            match(Do);
            return createNode(for_node, "for", createNode(to_node, "to", assign, expr), statement());
        }
        case Downto: {
            ++pos;  // for Downto
            struct astNode* expr = arithmeticExpression();
			match(Do);
            return createNode(for_node, "for", createNode(to_node, "downto", assign, expr), statement());
        }
        default: {
            printf("\nSyntax error in line %d : TO | DOWNTO operator was xpected, %s token gained).\n", TokenTable[pos].line, lexemeTypeName(TokenTable[pos].type));
            exit(1);
        }
        }
    }

    struct astNode* whileStatement() {
        match(While);
        struct astNode* expr = logicalExpression();
        struct astNode* body = whileBody();
        match(End);
        match(While);
        return createNode(while_node, "while", expr, body);
    }

    struct astNode* whileBody() {
        if (TokenTable[pos].type != End) {
            struct astNode* stmt = statementInWhile();
            //match(Semicolon);
            struct astNode* body = stmt;
            while (TokenTable[pos].type != End)
            {
                struct astNode* nextStmt = statementInWhile();
                //match(Semicolon);
                body = createNode(statement_node, "statement", body, nextStmt);
            }
            return body;
        }
        return nullptr;
    }

    struct astNode* statementInWhile() {
        switch (TokenTable[pos].type) {
        case Continue: {
            ++pos;  //  for CONTINUE
            match(While);
            return createNode(continue_node, "continue", nullptr, nullptr);
        }
        case Exit: {
            ++pos;  //  for EXIT
            match(While);
            return createNode(exit_node, "exit", nullptr, nullptr);
        }
        default:    return statement();
        }
    }

    struct astNode* repeatStatement() {
        match(Repeat);
		struct astNode* body = repeatBody();
		match(Until);
		match(LBracket);
		struct astNode* expr = logicalExpression();
		match(RBracket);
		return createNode(repeat_node, "repeat", body, expr);
    }

    struct astNode* repeatBody() {
        if (TokenTable[pos].type != Until) {
            struct astNode* stmt = statement();
            match(Semicolon);
            struct astNode* body = stmt;
            while (TokenTable[pos].type != Until)
            {
                struct astNode* nextStmt = statement();
                //match(Semicolon);
                body = createNode(statement_node, "statement", body, nextStmt);
            }
            return body;
        }
        return nullptr;
    }

	struct astNode* compoundStatement() {
		match(StartBlock);
		struct astNode* body = programBody();
		match(EndBlock);
		return createNode(compound_node, "compound", body, nullptr);
	}
}