#pragma once

#include "ast.h"
#include "lexer/token.h"

typedef struct ASTToken ASTAtom;

ASTNode *atom_parser(ASTNode*);
