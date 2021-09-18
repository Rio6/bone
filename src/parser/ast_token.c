#include "ast_token.h"
#include "utils/oop.h"

#include <stdio.h>
#include <stdlib.h>

static void dump(ASTNode *node, unsigned indent) {
   ASTToken *ast_token = (ASTToken*) node;

   print_indent(indent);
   printf("%s: ", ASTType_names[node->type]);
   token_print(ast_token->token);

   if(node->next) {
      CALL_METHOD(dump, node->next, indent);
   }
}

static void delete(ASTNode *node) {
   if(!node) return;

   if(node->next) {
      CALL_METHOD(delete, node->next);
   }

   ASTToken *ast_token = (ASTToken*) node;
   token_delete(ast_token->token);

   free(ast_token);
}

ASTToken *ast_token_create(Token *token) {
   ASTToken *ast = calloc(1, sizeof(ASTToken));
   ast_init(&ast->node, AST_TOKEN, NULL, &dump, &delete);
   token->next = NULL;
   ast->token = token;
   return ast;
}

ASTToken *ast_token_from_tokens(Token *token) {
   ASTToken head;
   ASTToken *tail = &head;

   while(token) {
      Token *next = token->next;
      tail->node.next = (ASTNode*) ast_token_create(token);
      tail->node.next->prev = &tail->node;
      tail = (ASTToken*) tail->node.next;
      token = next;
   }

   if(head.node.next) {
      head.node.next->prev = NULL;
   }

   return (ASTToken*) head.node.next;
}
