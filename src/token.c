#include "token.h"
#include "vector.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

TokenParseResult token_parse(Vector *stream, const char *src, size_t src_size) {
    TokenCursor cursor = {
        .buffer_size = src_size,
        .buffer = src,
        .cursor = 0,
    };
    const char *c;
    Token token = {
        .token_size = 1,
    };
    TokenParseResult result;

    while ((c = token_cursor_step(&cursor)) != NULL) {
        token.token = c;

        switch (*c) {
        case '(':
            token.type = TOKEN_TYPE_L_PAREN;
            break;
        case ')':
            token.type = TOKEN_TYPE_R_PAREN;
            break;
        case ',':
            token.type = TOKEN_TYPE_COMMA;
            break;
        case ';':
            token.type = TOKEN_TYPE_SEMI;
            break;
        default:
            if (token_isalnumlit(*c)) {
                if ((result = token_parse_alnum(stream, &cursor, c)) !=
                    TOKENIZE_SUCCESS) {
                    return result;
                }
            } else if (isspace(*c)) {
            } else {
                printf("Unknown token: %c\n", *c);
            }
            continue;
        }

        vector_push(stream, &token, sizeof(Token));
    }

    return TOKENIZE_SUCCESS;
}

TokenParseResult token_parse_alnum(Vector *stream, TokenCursor *cursor,
                                   const char *token_start) {
    if (isdigit(*token_start)) {
        return token_parse_digit(stream, cursor, token_start);
    } else if (token_isalnumlit(*token_start)) {
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

    while ((c = token_cursor_peek(cursor)) != NULL) {
        if (!isdigit(*c)) {
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
    Token token = {
        .type = TOKEN_TYPE_IDENT,
        .token = token_start,
        .token_size = 1,
    };

    while ((c = token_cursor_peek(cursor)) != NULL && token_isalnumlit(*c)) {
        token.token_size += 1;
        token_cursor_step(cursor);
    }

    TokenIsKeywordResult is_keyword =
        token_iskeyword(token.token, token.token_size);

    if (is_keyword.is_keyword)
        token.type = is_keyword.token_type.token_type;

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

bool token_isalnumlit(char c) { return isalnum(c) || c == '_'; }

TokenIsKeywordResult token_iskeyword(const char *str, size_t str_size) {
    TokenIsKeywordResult result = {.is_keyword = false};

    for (size_t i = 0; i < TOKEN_KEYWORDS_LEN; i++) {
        bool iskeyword = true;

        if (TOKEN_KEYWORDS[i].keyword_len > str_size)
            continue;

        for (size_t j = 0; j < str_size; j++) {
            if (TOKEN_KEYWORDS[i].keyword[j] != tolower(str[j])) {
                iskeyword = false;
                break;
            }
        }

        if (iskeyword == true) {
            result.is_keyword = true;
            result.token_type.token_type = TOKEN_KEYWORDS[i].token_type;

            break;
        }
    }

    return result;
}
