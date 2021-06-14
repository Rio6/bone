#pragma once

#include "ast.h"

typedef enum {
   NONE,
   PAREN,
   BRACK,
   BRACE,
} ASTGroupType;
extern const char *ASTGroupType_names[];

typedef struct {
   ASTNode node;
   ASTGroupType type;
   ASTNode *content; // owner
} ASTGroup;

ASTGroup *ast_group_create(ASTGroupType, ASTNode*);
ASTNode *group_parser(ASTToken*);
