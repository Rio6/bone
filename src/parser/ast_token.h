#pragma once

#include "ast.h"
#include "lexer/token.h"

typedef struct {
   ASTNode node;
   Token *token; // owner
} ASTToken;

ASTToken *ast_token_from_tokens(Token*); // Takes ownership
