#include "ast.h"
#include "ast_group.h"
#include "parser.h"

#include "utils/oop.h"

#include <stdio.h>
#include <stdlib.h>

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

   for(unsigned i = 0; i < indent; i++) {
      printf("  ");
   }

   puts(ASTType_names[node->type]);

   if(group->content) {
      CALL_METHOD(dump, group->content, indent+1);
   }

   if(node->next) {
      CALL_METHOD(dump, node->next, indent);
   }
}

ASTGroup *ast_group_create(ASTNode *content) {
   ASTGroup *group = calloc(1, sizeof(ASTGroup));
   group->content = content;
   group->node.parse = parse;
   group->node.dump = dump;
   group->node.delete = delete;
   return group;
}

void group_parser(ASTNode *node) {
   puts("hi");
}
