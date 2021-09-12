#include "ast_token.h"
#include "utils/oop.h"

#include <stdio.h>
#include <stdlib.h>

static ASTNode *parse(ASTNode *node, ASTParserFn *parsers) {

   ASTNode *new_node = NULL;
   for(ASTParserFn *parser = parsers; *parser != NULL; parser++) {
      new_node = (*parser)((ASTToken*) node);
      if(new_node) break;
   }

   if(new_node && new_node != node) {
      return CALL_METHOD(parse, new_node, parsers);
   }

   if(node->next) {
      node->next = CALL_METHOD(parse, node->next, parsers);
   }

   return node;
}

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

   ASTToken *ast_token = (ASTToken*) node;
   token_delete(ast_token->token);

   if(node->next) {
      CALL_METHOD(delete, node->next);
   }

   free(ast_token);
}

ASTToken *ast_token_create(Token *token) {
   ASTToken *ast = calloc(1, sizeof(ASTToken));
   ast_init(&ast->node, AST_TOKEN, parse, dump, delete);
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

ASTNode *ast_token_replace(ASTToken *old, ASTNode *new) {
   ASTNode *prev = old->node.prev, *next = old->node.next;
   if(prev) prev->next = new;
   if(next) next->prev = new;
   new->prev = prev;
   new->next = next;
   old->node.next = NULL;
   CALL_METHOD(delete, &old->node);
   return new;
}

ASTNode *ast_token_chop(ASTToken *token) {
   ASTNode *next = token->node.next;
   if(next) next->prev = NULL;
   token->node.next = NULL;
   CALL_METHOD(delete, &token->node);
   return next;
}

void ast_token_remove(ASTToken *token) {
   ASTNode *prev = token->node.prev, *next = token->node.next;
   if(next) next->prev = prev;
   token->node.next = NULL;
}
