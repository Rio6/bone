#pragma once

#include "ast.h"
#include "lexer/token.h"

extern ASTParser *parsers[];

ASTNode *parser_run(Token*);
