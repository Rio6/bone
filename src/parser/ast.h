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

   // Apply null-terminated ASTParser to its children and also the rest of node behind it.
   // Stop at first non-null result, returns a new node that replaces the current node
   ASTNode *(*parse)(ASTNode*, ASTParser**);
   void (*dump)(ASTNode*, unsigned indent);
   void (*delete)(ASTNode*);
};

void print_indent(unsigned);
ASTNode *ast_root(ASTNode*);

// Chop the head, return the new head. Optionally relink the new head to prev
ASTNode *ast_chop(ASTNode *ast, ASTNode *prev);
