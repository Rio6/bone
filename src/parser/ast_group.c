#include "ast.h"
#include "ast_group.h"
#include "ast_token.h"
#include "parser.h"

#include "utils/oop.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct {
   ASTGroupType type;
   TokenType open;
   TokenType close;
} GroupBoundary;

static const GroupBoundary group_boundaries[] = {
   { PAREN, PAREN_L, PAREN_R },
   { LIST, LIST_L, LIST_R },
   { BLOCK, BLOCK_L, BLOCK_R },
};

const char *ASTGroupType_names[] = {
   "NONE",
   "PAREN",
   "LIST",
   "BLOCK",
};

static void parse(ASTNode *node, ASTParser *parser) {
   ASTGroup *group = (ASTGroup*) node;

   if(group->content) {
      CALL_METHOD(parse, group->content, parser);
      if(group->content->up != node) {
         // AST rearranged, re-find root
         group->content = ast_root(group->content);
         group->content->up = node;
      }
   }

   if(node->next) {
      CALL_METHOD(parse, node->next, parser);
   }
}

static void delete(ASTNode *node) {
   ASTGroup *group = (ASTGroup*) node;
   if(node->next) CALL_METHOD(delete, node->next);
   if(group->content) CALL_METHOD(delete, group->content);
   free(group);
}

static void dump(ASTNode *node, unsigned indent) {
   ASTGroup *group = (ASTGroup*) node;

   print_indent(indent);
   printf("%s: %s\n", ASTType_names[node->type], ASTGroupType_names[group->type]);

   if(group->content) {
      CALL_METHOD(dump, group->content, indent+1);
   }

   if(node->next) {
      CALL_METHOD(dump, node->next, indent);
   }
}

ASTGroup *ast_group_create(ASTGroupType type, ASTNode *content) {
   ASTGroup *group = calloc(1, sizeof(ASTGroup));
   group->type = type;
   group->content = content;
   group->node.type = GROUP;
   group->node.parse = parse;
   group->node.dump = dump;
   group->node.delete = delete;
   content->up = (ASTNode*) group;
   return group;
}

void group_parser(ASTNode *node) {
   if(node->type != TOKEN) return;

   ASTToken *ast_token = (ASTToken*) node;
   const GroupBoundary *group_boundary = NULL;

   for(size_t i = 0; i < sizeof(group_boundaries) / sizeof(group_boundaries[0]); i++) {
      if(ast_token->token->type == group_boundaries[i].open) {
         group_boundary = &group_boundaries[i];
         break;
      }
   }

   if(!group_boundary) return;

   ASTNode *tail = node;

   int level = 0;
   while((tail = tail->next)) {
      if(tail->type == TOKEN) {
         ASTToken *ast_token = (ASTToken*) tail;
         if(ast_token->token->type == group_boundary->open) {
            level++;
         } else if(ast_token->token->type == group_boundary->close) {
            if(level == 0) {
               break;
            } else {
               level--;
            }
         }
      }
   }

   if(!tail) return; // TODO error

   ASTGroup *group = ast_group_create(group_boundary->type, node->next);
   ast_replace(&group->node, node, tail->next);
}
