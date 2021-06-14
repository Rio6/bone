#include "ast_token.h"
#include "utils/oop.h"

#include <stdio.h>
#include <stdlib.h>

static ASTNode *parse(ASTNode *node, ASTParser **parsers) {

   ASTNode *new_node = NULL;
   for(ASTParser **parser = parsers; *parser != NULL; parser++) {
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

ASTToken *ast_token_from_tokens(Token *token) {
   ASTToken head;
   ASTToken *tail = &head;

   while(token) {
      Token *next = token->next;

      tail->node.next = (ASTNode*) calloc(1, sizeof(ASTToken));
      tail->node.next->prev = (ASTNode*) tail;
      tail = (ASTToken*) tail->node.next;

      tail->node.type = TOKEN;
      tail->node.parse = parse;
      tail->node.dump = dump;
      tail->node.delete = delete;

      tail->token = token;
      token->next = NULL;
      token = next;
   }

   if(head.node.next) {
      head.node.next->prev = NULL;
   }

   return (ASTToken*) head.node.next;
}
