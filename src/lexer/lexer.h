#pragma once

#include "token.h"
#include "utils/stream.h"

typedef Token *(Lexer)(Stream *stream);

extern Lexer *lexers[];

Token *lexer_run(Stream *stream);
