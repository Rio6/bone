#include "lexer.h"
#include "stream.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

Token *lex(Stream *stream) {
   Token head_token = {0};
   Token *last_token = &head_token;

   while(1) {
      int has_token = 0;
      char last_c = '\0';

      for(Lexer **lexer = lexers; *lexer != NULL; lexer++) {

         // advance until not a white space
         while(isspace(stream_getc(stream)));
         last_c = stream_ungetc(stream, 1);

         Token *token = (*lexer)(stream);

         if(token) {
            last_token->next = token;
            last_token = token;
            has_token = 1;

            if(token->type == ERROR) {
               fprintf(stderr, "%s\n", token->value);
               token_delete(head_token.next);
               return NULL;
            }

            break;
         }
      }

      if(!has_token) {
         if(last_c != EOF) {
            fprintf(stderr, "Unexpected character: %1$c (%1$X)\n", last_c);
         }
         break;
      }
   }

   return head_token.next;
}

Token *token_create(TokenType type, const char *value) {
   char *copied_value = NULL;
   if(value) {
      size_t len = strlen(value) + 1;
      copied_value = malloc(len);
      memcpy(copied_value, value, len);
   }

   return token_create_raw(type, copied_value);
}

Token *token_create_raw(TokenType type, char *value) {
   Token *token = calloc(1, sizeof(Token));
   token->type = type;
   token->value = value;
   return token;
}

void token_delete(Token *token) {
   if(!token) return;
   token_delete(token->next);
   free(token->value);
   free(token);
}

void token_dump(Token *token) {
   if(!token) return;
   printf("type: %s, value: %s\n", TokenType_names[token->type], token->value);
   token_dump(token->next);
}

char *token_type_name(TokenType type) {
   switch(type) {
      case ERROR:   return "ERROR";
      case IDENT:   return "IDENT";
      case KEYWORD: return "KEYWORD";
      case INT:     return "INT";
      case FLOAT:   return "FLOAT";
      case CHAR:    return "CHAR";
      case STRING:  return "STRING";
      case COMMENT: return "COMMENT";
      case BLOCK_L: return "BLOCK_L";
      case BLOCK_R: return "BLOCK_R";
      case PAREN_L: return "PAREN_L";
      case PAREN_R: return "PAREN_R";
      case LIST_L:  return "LIST_L";
      case LIST_R:  return "LIST_R";
      case DOT:     return "DOT";
      case COMMA:   return "COMMA";
      case SEMICOLON: return "SEMICOLON";
      case ASSIGN:  return "ASSIGN";
      case OP:      return "OP";
      case REF:     return "REF";
      case DEREF:   return "DEREF";
      default:      return "UNKNOWN";
   }
}
