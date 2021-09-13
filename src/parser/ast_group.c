#include "ast.h"
#include "ast_group.h"
#include "ast_token.h"
#include "ast_error.h"
#include "parser.h"

#include "utils/array.h"
#include "utils/oop.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
   TokenType open;
   TokenType close;
} GroupBoundary;

static const GroupBoundary group_boundaries[] = {
   { T_PAREN_L, T_PAREN_R },
   { T_BRACK_L, T_BRACK_R },
   { T_BRACE_L, T_BRACE_R },
};

static ASTNode *parse(ASTNode *node, ASTParserFn *parsers) {
   ASTNode *new_node = parser_parse(node, parsers);
   if(new_node) return new_node;

   ASTGroup *group = (ASTGroup*) node;

   if(group->content) {
      group->content = CALL_METHOD(parse, group->content, parsers);
   }

   if(node->next) {
      node->next = CALL_METHOD(parse, node->next, parsers);
   }

   return node;
}

static void delete(ASTNode *node) {
   if(!node) return;

   if(node->next) {
      CALL_METHOD(delete, node->next);
   }

   ASTGroup *group = (ASTGroup*) node;
   if(group->content) {
      CALL_METHOD(delete, group->content);
   }

   free(group);
}

static void dump(ASTNode *node, unsigned indent) {
   ASTGroup *group = (ASTGroup*) node;

   print_indent(indent);
   printf("%s: type: %s\n", ASTType_names[node->type], TokenType_names[group->group_type]);

   if(group->content) {
      CALL_METHOD(dump, group->content, indent+1);
   }

   if(node->next) {
      CALL_METHOD(dump, node->next, indent);
   }
}

ASTGroup *ast_group_create(TokenType group_type, ASTNode *content) {
   ASTGroup *group = calloc(1, sizeof(ASTGroup));

   ast_init(&group->node, AST_GROUP, parse, dump, delete);

   group->group_type = group_type;
   group->content = content;

   return group;
}

ASTNode *group_parser(ASTNode *node) {
   if(node->type != AST_TOKEN) return NULL;

   // See if token is start of a group
   const GroupBoundary *group_boundary = NULL;
   for(size_t i = 0; i < LEN(group_boundaries); i++) {
      TokenType type = ((ASTToken*) node)->token->type;
      if(type == group_boundaries[i].open) {
         group_boundary = &group_boundaries[i];
         break;
      } else if(type == group_boundaries[i].close) {
         return (ASTNode*) ast_error_create("Unexpected close bracket", node);
      }
   }

   if(!group_boundary) return NULL;

   // Loop until the end of the group
   ASTNode *head = node;
   ASTNode *tail = head;

   int level = 0;
   while((tail = tail->next)) {
      if(tail->type == AST_TOKEN) {
         ASTToken *token = (ASTToken*) tail;
         if(token->token->type == group_boundary->open) {
            level++;
         } else if(token->token->type == group_boundary->close) {
            if(level == 0) {
               break;
            } else {
               level--;
            }
         }
      }
   }

   if(!tail) return (ASTNode*) ast_error_create("Unmatched bracket", node);

   // At this point, head should be the open bracket and tail would be the close bracket
   assert(head->type == AST_TOKEN && tail->type == AST_TOKEN);

   // Get content
   ASTNode *content = head->next;

   content->prev = NULL;
   head->next = NULL;
   CALL_METHOD(delete, head);

   ASTGroup *group = ast_group_create(group_boundary->open, content != tail ? content : NULL);

   // Move the nodes after close bracket to be after group
   ASTNode *after = tail->next;

   tail->next = NULL;
   if(tail->prev) tail->prev->next = NULL;
   CALL_METHOD(delete, tail);

   if(after) {
      group->node.next = after;
      after->prev = &group->node;
   }

   return &group->node;
}
