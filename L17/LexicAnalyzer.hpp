#pragma once

#include "header.hpp"
#include <stdio.h>

namespace LexicAnalyzer {
	unsigned int getTokens(FILE* F);
	void printTokens(void);
	void fprintTokens(FILE* F);
	//const char* lexemeTypeName(enum TypeOfToken type);
}
