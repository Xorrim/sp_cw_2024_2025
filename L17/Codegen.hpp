#pragma once

#include "header.hpp"

#include <stdio.h>
#include <stdlib.h>

namespace Codegen {
    void codegen(FILE* outFile, struct astNode* node);
}