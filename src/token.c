#include "token.h"
#include <stdio.h>

TokenParseResult token_parse(Vector *stream, const char *src, size_t src_size) {
    TokenCursor cursor = {
        .buffer_size = src_size,
        .buffer = src,
        .cursor = 0,
    };
    const char *c;
    Token token = {};

    while ((c = token_cursor_step(&cursor)) != NULL) {
        switch (*c) {
        case '(':
            token.type = TOKEN_TYPE_L_PAREN;
            token.token_size = 1;
            token.token = c;
            vector_push(stream, &token, sizeof(Token));
            break;
        case ')':
            token.type = TOKEN_TYPE_R_PAREN;
            token.token_size = 1;
            token.token = c;
            vector_push(stream, &token, sizeof(Token));
            break;
        default:
            printf("Unknown token: %c\n", *c);
            break;
        }
    }

    return TOKENIZE_FAILURE;
}

void token_print(Token *token) {
    printf("Token { type = %s, token_size = %zu, token = \"%.*s\" }",
           token_types[token->type], token->token_size, (int)token->token_size,
           token->token);
}

const char *token_cursor_step(TokenCursor *cursor) {
    if (cursor->cursor == cursor->buffer_size) {
        return NULL;
    }

    return cursor->buffer + cursor->cursor++;
}
