#include "ast.h"
#include "ast_unary.h"
#include "ast_op.h"
#include "ast_group.h"

#include "utils/array.h"
#include "utils/oop.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static TokenType prefix_tokens[] = {
   T_REF, T_DEREF, T_PLUS, T_MINUS, T_NOT,
};

static TokenType postfix_tokens[] = {
   T_REF, T_DEREF,
};

static ASTNode *parse(ASTNode *node, ASTParserFn *parsers) {
   ASTNode *new_node = parser_parse(node, parsers);
   if(new_node) return new_node;

   ASTUnary *unary = (ASTUnary*) node;

   unary->opand = CALL_METHOD(parse, unary->opand, parsers);

   if(node->next) {
      node->next = CALL_METHOD(parse, node->next, parsers);
   }

   return node;
}

static void dump(ASTNode *node, unsigned indent) {
   ASTUnary *unary = (ASTUnary*) node;

   print_indent(indent);
   printf("%s: op:\n", ASTType_names[node->type]);
   CALL_METHOD(dump, unary->op, indent+1);

   print_indent(indent);
   puts("opand:");
   CALL_METHOD(dump, unary->opand, indent+1);

   if(node->next) {
      CALL_METHOD(dump, node->next, indent);
   }
}

static void delete(ASTNode *node) {
   if(!node) return;

   ASTUnary *unary = (ASTUnary*) node;

   if(node->next) {
      CALL_METHOD(delete, node->next);
   }

   CALL_METHOD(delete, unary->op);
   CALL_METHOD(delete, unary->opand);

   free(node);
}

ASTUnary *ast_unary_create(ASTType type, ASTNode *op, ASTNode *opand) {
   ASTUnary *unary = calloc(1, sizeof(ASTUnary));

   ast_init(&unary->node, type, parse, dump, delete);
   unary->op = op;
   unary->opand = opand;

   return unary;
}

ASTNode *prefix_parser(ASTNode *node) {
   if(!node->next) return NULL;

   if(node->type == AST_OP) {
      bool is_prefix = false;
      for(size_t i = 0; i < LEN(prefix_tokens); i++) {
         TokenType op_type = ((ASTOp*) node)->op_type;
         if(op_type == prefix_tokens[i]) {
            is_prefix = true;
            break;
         }
      }
      if(!is_prefix) return NULL;

   } else if(node->type == AST_GROUP) {
      TokenType group_type = ((ASTGroup*) node)->group_type;
      if(group_type != T_PAREN_L) {
         return NULL;
      }
   } else {
      return NULL;
   }

   ASTUnary *unary = ast_unary_create(AST_PREFIX, node, node->next);
   ast_add_next(&unary->node, node->next->next);
   node->next->next = node->next->prev = NULL;
   node->next = node->prev = NULL;

   return &unary->node;
}

ASTNode *postfix_parser(ASTNode *node) {
   if(!node->next) return NULL;

   if(node->next->type == AST_OP) {
      bool is_postfix = false;
      for(size_t i = 0; i < LEN(postfix_tokens); i++) {
         TokenType op_type = ((ASTOp*) node->next)->op_type;
         if(op_type == postfix_tokens[i]) {
            is_postfix = true;
            break;
         }
      }
      if(!is_postfix) return NULL;

   } else if(node->next->type == AST_GROUP) {
      TokenType group_type = ((ASTGroup*) node->next)->group_type;
      if(group_type != T_PAREN_L && group_type != T_BRACK_L) {
         return NULL;
      }
   } else {
      return NULL;
   }

   ASTUnary *unary = ast_unary_create(AST_POSTFIX, node->next, node);
   ast_add_next(&unary->node, node->next->next);
   node->next->next = node->next->prev = NULL;
   node->next = node->prev = NULL;

   return &unary->node;
}
