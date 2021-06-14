#pragma once

#include "parser.h"

typedef enum {
   AST_ERROR,
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

   // Parse method apply ASTParser to its children and also the rest of node behind it.
   // Returns a new node that replaces the current node
   ASTNode *(*parse)(ASTNode*, ASTParser*);
   void (*dump)(ASTNode*, unsigned indent);
   void (*delete)(ASTNode*);
};

void print_indent(unsigned);
ASTNode *ast_root(ASTNode*);
