#pragma once

#include "ast.h"
#include "lexer/token.h"

typedef struct ASTToken {
   ASTNode node;
   Token *token; // owner
} ASTToken;

ASTToken *ast_token_from_tokens(Token*); // Takes ownership
