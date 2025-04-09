#include "token.h"
#include "vector.h"
#include <ctype.h>
#include <stdio.h>

TokenParseResult token_parse(Vector *stream, const char *src, size_t src_size) {
    TokenCursor cursor = {
        .buffer_size = src_size,
        .buffer = src,
        .cursor = 0,
    };
    const char *c;
    Token token = {};
    TokenParseResult result;

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
        case ',':
            token.type = TOKEN_TYPE_COMMA;
            token.token_size = 1;
            token.token = c;
            vector_push(stream, &token, sizeof(Token));
            break;
        case ';':
            token.type = TOKEN_TYPE_SEMI;
            token.token_size = 1;
            token.token = c;
            vector_push(stream, &token, sizeof(Token));
            break;
        default:
            if (isalnum(*c)) {
                if ((result = token_parse_alnum(stream, &cursor, c)) !=
                    TOKENIZE_SUCCESS) {
                    return result;
                }
            } else if (isspace(*c)) {
            } else {
                printf("Unknown token: %c\n", *c);
            }
            break;
        }
    }

    return TOKENIZE_SUCCESS;
}

TokenParseResult token_parse_alnum(Vector *stream, TokenCursor *cursor,
                                   const char *token_start) {
    if (isdigit(*token_start)) {
        return token_parse_digit(stream, cursor, token_start);
    } else if (isalpha(*token_start)) {
        return token_parse_alpha(stream, cursor, token_start);
    }

    return TOKENIZE_FAILURE;
}

TokenParseResult token_parse_digit(Vector *stream, TokenCursor *cursor,
                                   const char *token_start) {
    const char *c;
    const char *peek;
    Token token = {
        .type = TOKEN_TYPE_NUM,
        .token = token_start,
        .token_size = 1,
    };

    while ((c = token_cursor_peek(cursor)) != NULL && isdigit(*c)) {
        if ((peek = token_cursor_peek(cursor)) != NULL && isalpha(*peek)) {
            return TOKENIZE_FAILURE;
        }

        token.token_size += 1;
        token_cursor_step(cursor);
    }

    vector_push(stream, &token, sizeof(Token));

    return TOKENIZE_SUCCESS;
}

TokenParseResult token_parse_alpha(Vector *stream, TokenCursor *cursor,
                                   const char *token_start) {
    const char *c;
    const char *peek;
    Token token = {
        .type = TOKEN_TYPE_IDENT,
        .token = token_start,
        .token_size = 1,
    };

    while ((c = token_cursor_peek(cursor)) != NULL && isalnum(*c)) {
        token.token_size += 1;
        token_cursor_step(cursor);
    }

    vector_push(stream, &token, sizeof(Token));

    return TOKENIZE_SUCCESS;
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

const char *token_cursor_peek(TokenCursor *cursor) {
    if (cursor->cursor == cursor->buffer_size) {
        return NULL;
    }

    return cursor->buffer + cursor->cursor;
}
