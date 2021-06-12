#include "group.h"

void parser_group(ASTNode *node) {
}

ASTGroup *ast_group_create(ASTNode *content) {
   ASTGroup *node = calloc(sizeof(ASTGroup));
   node->content = content;
   node->parse = parser_group;
   node->delete = ast_group_delete;
   return node;
}

void ast_group_delete(ASTNode *node) {
   ASTGroup *group = (ASTGroup*) node;
   if(group->next) group->next->delete(group->next);
   if(group->content) group->content->delete(group->content);
   free(group);
}

void ast_group_print(ASTNode *node) {
   ASTGroup *group = (ASTGroup*) node;
   puts("group");
}
