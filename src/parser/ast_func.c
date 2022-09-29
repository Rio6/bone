#include "ast_func.h"
#include "ast_atom.h"
#include "ast_token.h"
#include "ast_group.h"
#include "lexer/token.h"

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
   printf("%s\n", ASTType_names[node->type]);

   print_indent(indent+1);
   puts("returns:");
   CALL_METHOD(dump, func->returns, indent+1);

   print_indent(indent+1);
   puts("param:");
   if(func->param) {
      CALL_METHOD(dump, func->param, indent+1);
   } else {
      print_indent(indent+1);
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

   if(func->param) {
      CALL_METHOD(delete, func->param);
   }

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
   if(node->type != AST_ATOM || ((ASTAtom*) node)->token->type != T_IDENT) {
      return NULL;
   }

   ASTNode *end_node = node;
   while(1) {
      end_node = end_node->next;

      if(!end_node) {
         return NULL;
      }

      if(end_node->type == AST_GROUP) {
         ASTGroup *group = (ASTGroup*) end_node;

         if(group->group_type == T_PAREN_L) {
            break;
         } else {
            return NULL;
         }
      }

      if(end_node->type == AST_TOKEN) {
         Token *token = ((ASTToken*) end_node)->token;
         if(token->type != T_REF) {
            return NULL;
         }
      }
   }

   if(
      !end_node || node == end_node ||
      !end_node->next ||
      !(
         end_node->next->type == AST_ATOM ||
         end_node->next->type == AST_TOKEN &&
         ((ASTToken*) end_node->next)->token->type == T_REF
      )
   ) {
      return NULL;
   }

   ASTGroup *group = (ASTGroup*) end_node;
   end_node->prev->next = NULL; // terminate returns list

   ASTFuncType *func = ast_func_create(node, group->content);

   ast_add_next(&func->node, end_node->next);
   ast_add_next(node->prev, &func->node);

   group->content = NULL;
   CALL_METHOD(delete, &group->node);

   return &func->node;
}
