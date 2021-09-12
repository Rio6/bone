#pragma once

#include "ast.h"

typedef struct {
   ASTNode node;
   TokenType group_type; // owner, the token type of the open bracket
   ASTNode *content; // owner
} ASTGroup;

// Create a group AST. Type is the TokenType of the opening bracket.
// The content should be NULL on both ends already.
ASTGroup *ast_group_create(TokenType, ASTNode*);
ASTNode *group_parser(ASTToken*);
