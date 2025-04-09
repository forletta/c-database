#ifndef TOKEN_H
#define TOKEN_H

#include "vector.h"
#include <stddef.h>
#include <stdio.h>

typedef struct {
    enum {
        // TOKEN_TYPE_KEYWORD,
        TOKEN_TYPE_L_PAREN,
        TOKEN_TYPE_R_PAREN,
        // TOKEN_TYPE_IDENT,
        // TOKEN_TYPE_STR,
        // TOKEN_TYPE_NUM,
    } type;
    size_t token_size;
    char *token;
} Token;

typedef enum {
    TOKENIZE_SUCCESS,
    TOKENIZE_FAILURE,
} TokenParseResult;

static const char *token_types[2] = {
    "TOKEN_TYPE_L_PAREN",
    "TOKEN_TYPE_R_PAREN",
};

TokenParseResult token_parse(Vector *stream, char *src, size_t src_size);

void token_print(Token *token);

#endif /* ifndef TOKEN_H */

