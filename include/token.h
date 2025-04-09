#ifndef TOKEN_H
#define TOKEN_H

#include "vector.h"
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    enum {
        // Brackets
        TOKEN_TYPE_L_PAREN,
        TOKEN_TYPE_R_PAREN,

        // Punctuation
        TOKEN_TYPE_COMMA,
        TOKEN_TYPE_SEMI,

        // Literal
        TOKEN_TYPE_NUM,
        TOKEN_TYPE_IDENT,
    } type;
    size_t token_size;
    const char *token;
} Token;

typedef enum {
    TOKENIZE_SUCCESS,
    TOKENIZE_FAILURE,
} TokenParseResult;

typedef struct {
    size_t buffer_size;
    const char *buffer;
    size_t cursor;
} TokenCursor;

static const char *token_types[6] = {
    "TOKEN_TYPE_L_PAREN", "TOKEN_TYPE_R_PAREN", "TOKEN_TYPE_COMMA",
    "TOKEN_TYPE_SEMI",    "TOKEN_TYPE_NUM",     "TOKEN_TYPE_IDENT",
};

TokenParseResult token_parse(Vector *stream, const char *src, size_t src_size);
TokenParseResult token_parse_alnum(Vector *stream, TokenCursor *cursor,
                                   const char *token_start);
TokenParseResult token_parse_digit(Vector *stream, TokenCursor *cursor,
                                   const char *token_start);
TokenParseResult token_parse_alpha(Vector *stream, TokenCursor *cursor,
                                   const char *token_start);

void token_print(Token *token);

const char *token_cursor_step(TokenCursor *cursor);
const char *token_cursor_peek(TokenCursor *cursor);

bool token_isalnumlit(char c);

#endif /* ifndef TOKEN_H */
