#ifndef TOKEN_H
#define TOKEN_H

#include "vector.h"
#include <stddef.h>
#include <stdio.h>

typedef struct {
    enum {
        TOKEN_TYPE_L_PAREN,
        TOKEN_TYPE_R_PAREN,
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

static const char *token_types[2] = {
    "TOKEN_TYPE_L_PAREN",
    "TOKEN_TYPE_R_PAREN",
};

TokenParseResult token_parse(Vector *stream, const char *src, size_t src_size);

void token_print(Token *token);

const char *token_cursor_step(TokenCursor *cursor);
const char *token_cursor_peek(TokenCursor *cursor);

#endif /* ifndef TOKEN_H */
