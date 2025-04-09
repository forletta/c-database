#include "token.h"
#include <stdio.h>

TokenParseResult token_parse(Vector *stream, char *src, size_t src_size) {
    Token token = {};

    for (size_t i = 0; i < src_size; i++) {
        switch (src[i]) {
        case '(':
            token.type = TOKEN_TYPE_L_PAREN;
            token.token_size = 1;
            token.token = src + i;
            vector_push(stream, &token, sizeof(Token));
            break;
        case ')':
            token.type = TOKEN_TYPE_R_PAREN;
            token.token_size = 1;
            token.token = src + i;
            vector_push(stream, &token, sizeof(Token));
            break;
        default:
            printf("Unknown token: %c\n", src[i]);
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
