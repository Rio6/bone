#pragma once

#include "ast.h"
#include "lexer/token.h"

typedef struct ASTAtom {
   ASTNode node;
   Token *token; // owner
} ASTAtom;

ASTNode *atom_parser(ASTNode*);
ASTAtom *ast_atom_create(Token*); // Takes ownership
