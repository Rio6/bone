#include "ast.h"
#include "utils/oop.h"

#include <stdio.h>

const char *ASTType_names[] = {
   "AST_ERROR",
   "AST_ATOM",
   "AST_PREFIX",
   "AST_POSTFIX",
   "AST_BINARY",
   "AST_TENARY",
   "AST_GROUP",
   "AST_TOKEN",
};

void print_indent(unsigned indent) {
   for(unsigned i = 0; i < indent; i++) {
      fputs("   ", stdout);
   }
}

void ast_init(ASTNode *node, ASTType type, ASTParseFn parse, ASTDumpFn dump, ASTDeleteFn delete) {
   node->type = type;
   node->parse = parse;
   node->dump = dump;
   node->delete = delete;
}

ASTNode *ast_root(ASTNode *node) {
   while(node->up) node = node->up;
   return node;
}

ASTNode *ast_replace(ASTNode *old, ASTNode *new) {
   ASTNode *prev = old->prev, *next = old->next;
   if(prev) prev->next = new;
   if(next) next->prev = new;
   new->prev = prev;
   new->next = next;
   old->next = NULL;
   CALL_METHOD(delete, old);
   return new;
}

ASTNode *ast_chop(ASTNode *ast) {
   ASTNode *next = ast->next;
   if(ast->prev) ast->prev->next = NULL;
   if(next)           next->prev = NULL;
   ast->next = NULL;
   CALL_METHOD(delete, ast);
   return next;
}

void ast_remove(ASTNode *ast) {
   ASTNode *prev = ast->prev, *next = ast->next;
   if(prev) prev->next = next;
   if(next) next->prev = prev;
   ast->next = NULL;
   CALL_METHOD(delete, ast);
}
