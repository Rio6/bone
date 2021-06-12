#pragma once

typedef enum {
   ATOM,
   UNARY,
   BINARY,
   TENARY,
   GROUP,
   TOKEN,
} ASTType;
extern const char *ASTType_names[];

typedef struct ASTNode {
   ASTType type;
   ASTNode *up;
   ASTNode *prev;
   ASTNode *next;

   void (print*)(ASTNode*);
   void (parse*)(ASTNode*, ASTParser);
   void (delete*)(ASTNode*);
} ASTNode;

typedef void (ASTParser*)(ASTNode*);
typedef ASTNode *(Parser*)(Token*);

extern Parser *parsers[];

void ast_replace(ASTNode *node, ASTNode *prev, ASTNode next);
