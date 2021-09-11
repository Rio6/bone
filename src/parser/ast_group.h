#pragma once

#include "ast.h"

typedef enum {
   GROUP_NONE,
   GROUP_PAREN,
   GROUP_BRACK,
   GROUP_BRACE,
} ASTGroupType;
extern const char *ASTGroupType_names[];

typedef struct {
   ASTNode node;
   ASTGroupType type;
   ASTNode *content; // owner
} ASTGroup;

// Create a group AST. The content should be NULL on both ends already.
ASTGroup *ast_group_create(ASTGroupType, ASTNode*);
ASTNode *group_parser(ASTToken*);
