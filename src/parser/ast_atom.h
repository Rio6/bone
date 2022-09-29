#pragma once

#include "ast.h"
#include "ast_token.h"
#include "lexer/token.h"

typedef struct ASTToken ASTAtom;

ASTNode *atom_parser(ASTNode*);
