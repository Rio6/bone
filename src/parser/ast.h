#pragma once

#include "parser.h"

typedef enum {
   ATOM,
   UNARY,
   BINARY,
   TENARY,
   GROUP,
   TOKEN,
} ASTType;
extern const char *ASTType_names[];

typedef struct ASTNode ASTNode;
struct ASTNode {
   ASTType type;
   ASTNode *up;
   ASTNode *prev;
   ASTNode *next;

   void (*parse)(ASTNode*, ASTParser*);
   void (*dump)(ASTNode*, unsigned indent);
   void (*delete)(ASTNode*);
};

void print_indent(unsigned);
void ast_replace(ASTNode *node, ASTNode *prev, ASTNode *next);
ASTNode *ast_root(ASTNode*);
