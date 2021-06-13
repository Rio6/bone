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

   group->node.type = GROUP;
   group->node.parse = parse;
   group->node.dump = dump;
   group->node.delete = delete;

   group->type = type;
   group->content = content;

   if(content) {
      content->up = (ASTNode*) group;
      content->prev = NULL;
   }

   return group;
}

ASTNode *group_parser(ASTToken *ast_token) {
   // See if token is start of a group
   const GroupBoundary *group_boundary = NULL;
   for(size_t i = 0; i < sizeof(group_boundaries) / sizeof(group_boundaries[0]); i++) {
      TokenType type = ast_token->token->type;
      if(type == group_boundaries[i].open) {
         group_boundary = &group_boundaries[i];
         break;
      } else if(type == group_boundaries[i].close) {
         return NULL; // TODO  error
      }
   }

   if(!group_boundary) return NULL;

   // Loop until the end of the group
   ASTNode *tail = &ast_token->node;

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

   if(!tail) return NULL; // TODO error

   if(tail->prev) tail->prev->next = NULL;

   ASTGroup *group = ast_group_create(group_boundary->type, ast_token->node.next);
   ast_replace(&group->node, ast_token->node.prev, tail->next);

   ast_token->node.next = NULL;
   CALL_METHOD(delete, &ast_token->node);

   tail->next = NULL;
   CALL_METHOD(delete, tail);

   return &group->node;
}
