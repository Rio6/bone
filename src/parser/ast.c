#include "ast.h"

const char *ASTType_names[] = {
   "ATOM",
   "UNARY",
   "BINARY",
   "TENARY",
   "GROUP",
   "TOKEN",
};

void ast_replace(ASTNode *node, ASTNode *prev, ASTNode *next) {
   prev->next = node;
   next->prev = node;
   node->prev = prev;
   node->next = next;
}

ASTNode *ast_root(ASTNode *node) {
   while(node->up) node = node->up;
   return node;
}
