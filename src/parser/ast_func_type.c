#include "ast_func_type.h"
#include "ast_token.h"
#include "ast_group.h"

#include "utils/array.h"
#include "utils/oop.h"

#include <stdio.h>
#include <stdlib.h>

static ASTNode *parse(ASTNode *node, ASTParserFn *parsers) {
   ASTNode *new_node = parser_parse(node, parsers);
   if(new_node) return new_node;

   ASTFuncType *func = (ASTFuncType*) node;
   func->returns = CALL_METHOD(parse, func->returns, parsers);

   if(func->param) {
      func->param = CALL_METHOD(parse, func->param, parsers);
   }

   if(node->next) {
      node->next = CALL_METHOD(parse, node->next, parsers);
   }

   return node;
}

static void dump(ASTNode *node, unsigned indent) {
   ASTFuncType *func = (ASTFuncType*) node;

   print_indent(indent);
   printf("%s: returns:\n", ASTType_names[node->type]);
   CALL_METHOD(dump, func->returns, indent+1);

   puts("param:");
   if(func->param) {
      CALL_METHOD(dump, func->param, indent+1);
   } else {
      puts("void");
   }

   if(node->next) {
      CALL_METHOD(dump, node->next, indent);
   }
}

static void delete(ASTNode *node) {
   if(node->next) {
      CALL_METHOD(delete, node->next);
   }

   ASTFuncType *func = (ASTFuncType*) node;
   CALL_METHOD(delete, func->returns);
   CALL_METHOD(delete, func->param);

   free(func);
}

ASTFuncType *ast_func_create(ASTNode *returns, ASTNode *param) {
   ASTFuncType *func = calloc(1, sizeof(ASTFuncType));
   ast_init(&func->node, AST_FUNC_TYPE, &parse, &dump, &delete);

   func->returns = returns;
   func->param = param;

   return func;
}

ASTNode *func_type_parser(ASTNode *node) {
   if(
      node->type != AST_GROUP || ((ASTGroup*) node)->group_type != T_PAREN_L
      || !node->next || node->next->type != AST_TOKEN
      || ((ASTToken*) node->next)->token->type != T_REF
   ) {
      return NULL;
   }

   // find where return type starts
   ASTNode *returns_start = node;
   while(1) {
      ASTNode *prev = returns_start->prev;

      if(
         !prev
         || (prev->type == AST_GROUP && ((ASTGroup*) prev)->group_type == T_BRACE_L)
         || (prev->type == AST_TOKEN && ((ASTToken*) prev)->token->type == T_SEMICOLON)
      ) {
         break;
      }

      returns_start = prev;
   }

   if(returns_start == node) return NULL;

   ASTGroup *group = (ASTGroup*) node;
   node->prev->next = NULL; // terminate returns list XXX

   ASTFuncType *func_type = ast_func_create(returns_start, group->content);

   ast_add_next(&func_type->node, node->next);
   ast_add_next(returns_start->prev, &func_type->node);

   group->content = NULL;
   //CALL_METHOD(delete, node);

   return &func_type->node;
}
