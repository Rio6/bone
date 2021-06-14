#include "ast.h"
#include "utils/oop.h"

#include <stdio.h>

const char *ASTType_names[] = {
   "AST_ERROR",
   "ATOM",
   "PREFIX",
   "POSTFIX",
   "BINARY",
   "TENARY",
   "GROUP",
   "TOKEN",
};

void print_indent(unsigned indent) {
   for(unsigned i = 0; i < indent; i++) {
      fputs("   ", stdout);
   }
}

ASTNode *ast_root(ASTNode *node) {
   while(node->up) node = node->up;
   return node;
}

ASTNode *ast_chop(ASTNode *ast, ASTNode *prev) {
   ASTNode *next = ast->next;
   if(prev) prev->next = next;
   if(next) next->prev = prev;
   ast->next = NULL;
   CALL_METHOD(delete, ast);
   return next;
}
