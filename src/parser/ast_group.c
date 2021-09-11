#include "ast.h"
#include "ast_group.h"
#include "ast_token.h"
#include "ast_error.h"
#include "parser.h"

#include "utils/oop.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
   ASTGroupType type;
   TokenType open;
   TokenType close;
} GroupBoundary;

static const GroupBoundary group_boundaries[] = {
   { GROUP_PAREN, T_PAREN_L, T_PAREN_R },
   { GROUP_BRACK, T_BRACK_L, T_BRACK_R },
   { GROUP_BRACE, T_BRACE_L, T_BRACE_R },
};

const char *ASTGroupType_names[] = {
   "GROUP_NONE",
   "GROUP_PAREN",
   "GROUP_BRACK",
   "GROUP_BRACE",
};

static ASTNode *parse(ASTNode *node, ASTParserFn *parsers) {
   ASTGroup *group = (ASTGroup*) node;

   if(group->content) {
      ASTNode *new_content = CALL_METHOD(parse, group->content, parsers);
      if(new_content->up != node) {
         // AST rearranged, re-find root
         group->content = ast_root(new_content);
         group->content->up = node;
      }
   }

   if(node->next) {
      node->next = CALL_METHOD(parse, node->next, parsers);
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

   ast_init(&group->node, AST_GROUP, parse, dump, delete);

   group->type = type;
   group->content = content;

   if(content) {
      content->up = &group->node;
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
         return (ASTNode*) ast_error_create("Unexpected close bracket", &ast_token->node);
      }
   }

   if(!group_boundary) return NULL;

   // Loop until the end of the group
   ASTNode *head = &ast_token->node;
   ASTNode *tail = head;

   int level = 0;
   while((tail = tail->next)) {
      if(tail->type == AST_TOKEN) {
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

   if(!tail) return (ASTNode*) ast_error_create("Unmatched bracket", &ast_token->node);

   // At this point, head should be the open bracket and tail would be the close bracket
   assert(head->type == AST_TOKEN && tail->type == AST_TOKEN);

   ASTNode *content = ast_token_chop((ASTToken*) head);
   if(content == tail) content = NULL;
   if(tail->prev) tail->prev->next = NULL;
   ASTNode *after = ast_token_chop((ASTToken*) tail);
   ASTGroup *group = ast_group_create(group_boundary->type, content);

   // Move the nodes after close bracket to be after group
   if(after) {
      group->node.next = after;
      after->prev = &group->node;
   }

   return &group->node;
}
