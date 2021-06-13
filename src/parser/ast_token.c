#include "ast_token.h"
#include "utils/oop.h"

#include <stdio.h>
#include <stdlib.h>

static void parse(ASTNode *node, ASTParser *parser) {
   parser(node);
   if(node->next) {
      CALL_METHOD(parse, node->next, parser);
   }
}

static void dump(ASTNode *node, unsigned indent) {
   ASTToken *ast_token = (ASTToken*) node;
   printf("%s: %s\n", ASTType_names[node->type], TokenType_names[ast_token->token->type]);

   if(node->next) {
      CALL_METHOD(dump, node->next, indent);
   }
}

static void delete(ASTNode *node) {
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
