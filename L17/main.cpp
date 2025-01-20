#include <iostream>
#include <windows.h>

#include "LexicAnalyzer.hpp"
#include "Parser.hpp"
#include "Ast.hpp"
#include "Codegen.hpp"

#define LANGUAGE	".l17"

struct Token* TokenTable;   // Таблиця лексем
unsigned int TokensNum;     // Кількість лексем

struct id* idTable;         // Таблиця ідентифікаторів
unsigned int idNum;         // кількість ідентифікаторів

struct id* labelTable;      // Таблиця міток
unsigned int labelNum;      // кількість міток

FILE* errorFile;

int main(int argc, char* argv[])
{
	// виділення пам'яті під таблицю лесем
	TokenTable = (struct Token*)malloc(MAX_TOKENS * sizeof(struct Token));

	// виділення пам'яті під таблицю ідентифікаторів
	idTable = (struct id*)malloc(MAX_IDENTIFIER * sizeof(struct id));
	labelTable = (struct id*)malloc(MAX_IDENTIFIER * sizeof(struct id));

	char InputFile[32] = "";

	FILE* InFile, *TokenFileP;

	if (argc != 2)
	{
		printf("Input file name: ");
		gets_s(InputFile);
	}
	else
	{
		strcpy_s(InputFile, argv[1]);
	}

	if (!strstr(InputFile, LANGUAGE)) {
		free(TokenTable);
		free(idTable);
		free(labelTable);
		_fcloseall();
		printf("Program file name should contain \"%s\"", LANGUAGE);
		exit(1);
	}

	if ((fopen_s(&InFile, InputFile, "rt")) != 0)
	{
		printf("Error: Can not open file: %s\n", InputFile);
		return 1;
	}

	char NameFile[32] = "";
	int i = 0;
	while (InputFile[i] != '.' && InputFile[i] != '\0')
	{
		NameFile[i] = InputFile[i];
		i++;
	}
	NameFile[i] = '\0';

	char TokenFile[32], errorFileName[32];
	strcpy_s(TokenFile, NameFile);
	strcat_s(TokenFile, ".token");
	strcpy_s(errorFileName, NameFile);
	strcat_s(errorFileName, ".errorlist");

	// лексичний аналіз
	if ((fopen_s(&TokenFileP, TokenFile, "wt")) != 0)
	{
		printf("Error: Can not create file: %s\n", TokenFile);
		free(TokenTable);
		free(idTable);
		free(labelTable);
		_fcloseall();
		return 1;
	}

	if ((fopen_s(&errorFile, errorFileName, "wt")) != 0)
	{
		printf("Error: Can not create file: %s\n", errorFileName);
		free(TokenTable);
		free(idTable);
		free(labelTable);
		_fcloseall();
		return 1;
	}

	TokensNum = LexicAnalyzer::getTokens(InFile);

	LexicAnalyzer::fprintTokens(TokenFileP);
	fclose(InFile);
	fclose(TokenFileP);

	printf("\nLexical analysis completed: %d tokens. List of tokens in the file %s\n", TokensNum, TokenFile);
	//PrintTokens(TokenTable, TokensNum);

	// синтаксичний аналіз
	Parser::Parser();
	Parser::Semantic();

	// створення абстрактного синтаксичного дерева
	struct astNode* ast = AST::astParser();

	//printf("\nAbstract Syntax Tree:\n");
	//PrintAST(ASTree, 0);

	char AST[32];
	strcpy_s(AST, NameFile);
	strcat_s(AST, ".ast");
	// Open output file
	FILE* ASTFile;
	fopen_s(&ASTFile, AST, "w");
	if (!ASTFile)
	{
		printf("Failed to open output file.\n");
		free(TokenTable);
		free(idTable);
		free(labelTable);
		AST::deleteNode(ast);
		exit(1);
	}
	AST::fPrintAST(ASTFile, ast, 0);
	printf("\nAST has been created and written to %s.\n", AST);

	char OutputFile[32];
	strcpy_s(OutputFile, NameFile);
	strcat_s(OutputFile, ".c");

	// Open output file
	FILE* outFile;
	fopen_s(&outFile, OutputFile, "w");
	if (!outFile)
	{
		printf("Failed to open output file.\n");
		free(TokenTable);
		free(idTable);
		free(labelTable);
		AST::deleteNode(ast);
		exit(1);
	}
	// Generate C code from AST
	Codegen::codegen(outFile, ast);

	// генерація вихідного С коду
	printf("\nC code has been generated and written to %s.\n", OutputFile);

	// Close the file
	_fcloseall();

	free(TokenTable);
	free(idTable);
	free(labelTable);

	char setVar[256] = "\"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat\"";

	char createExe[128];
	sprintf_s(createExe, "cl %s", OutputFile);
	strcat_s(setVar, " && ");
	strcat_s(setVar, createExe);
	system(setVar);
	//*/

	system("pause");
	return 0;
}