#pragma once

#include "ast.h"

typedef struct ASTGroup {
   ASTNode node;
   TokenType group_type; // the token type of the open bracket
   ASTNode *content; // owner
} ASTGroup;

// Create a group AST. Type is the TokenType of the opening bracket.
// The content should be NULL on both ends already.
ASTGroup *ast_group_create(TokenType, ASTNode*); // takes owner
ASTNode *group_parser(ASTNode*);
