#include "ast.h"
#include "ast_group.h"
#include "ast_token.h"
#include "ast_error.h"
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

static ASTNode *parse(ASTNode *node, ASTParser *parser) {
   ASTGroup *group = (ASTGroup*) node;

   if(group->content) {
      ASTNode *new_content = CALL_METHOD(parse, group->content, parser);
      if(new_content->up != node) {
         // AST rearranged, re-find root
         group->content = ast_root(new_content);
         group->content->up = node;
      }
   }

   if(node->next) {
      node->next = CALL_METHOD(parse, node->next, parser);
   }

   return node;
}

static void delete(ASTNode *node) {
   if(!node) return;

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
         return ast_error_create_node("Unexpected close bracket", &ast_token->node);
      }
   }

   if(!group_boundary) return NULL;

   // Loop until the end of the group
   ASTNode *head = &ast_token->node;
   ASTNode *tail = head;

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

   if(!tail) return NULL;//ast_error_create_node("Unmatched bracket", &ast_token->node);

   // At this point, head would be the open bracket and tail would be the close bracket

   // Set the end of group's content to be the before the close bracket
   if(tail->prev) tail->prev->next = NULL;

   ASTGroup *group = ast_group_create(group_boundary->type, head->next);

   // Move the nodes after close bracket to be after group
   if(tail->next) {
      group->node.next = tail->next;
      tail->next->prev = &group->node;
   }

   // Isolate and delete open and close brackets
   head->next = NULL;
   tail->next = NULL;
   CALL_METHOD(delete, head);
   CALL_METHOD(delete, tail);

   return &group->node;
}
