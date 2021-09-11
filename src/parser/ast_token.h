#pragma once

#include "ast.h"
#include "lexer/token.h"

typedef struct ASTToken {
   ASTNode node;
   Token *token; // owner
} ASTToken;

ASTToken *ast_token_create(Token*);
ASTToken *ast_token_from_tokens(Token*); // Takes ownership

ASTNode *ast_token_replace(ASTToken *old, ASTNode *new);
// Chop the head, return the new head.
ASTNode *ast_token_chop(ASTToken*);
void ast_token_remove(ASTToken*);
