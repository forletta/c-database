#include "token.h"
#include "c_core_error.h"
#include "void_vector.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

// TokenVector:

Token *TokenVector_get(const TokenVector *v, size_t i) {
    if (i < v->len)
        return v->ptr + i;

    out_of_bounds();
}

void TokenVector_push(TokenVector *v, Token *token) {
    VoidVector_ensure_capacity((VoidVector *)v, sizeof(Token), 1);

    v->ptr[v->len++] = *token;
}
void TokenVector_free(TokenVector *v) {
    VoidVector_free((VoidVector *)v);
}

// TokenVector:

Token *TokenVectorIter_next(TokenVectorIter *iter) {
    if (iter->cursor >= iter->stream->len)
        return NULL;

    return TokenVector_get(iter->stream, iter->cursor++);
}

Token *TokenVectorIter_peek(TokenVectorIter *iter) {
    if (iter->cursor >= iter->stream->len)
        return NULL;

    return TokenVector_get(iter->stream, iter->cursor);
}

void TokenVectorIter_context_enter(TokenVectorIter *iter) {
    iter->context_start = iter->cursor;
}

void TokenVectorIter_context_exit(TokenVectorIter *iter) {
    iter->cursor = iter->context_start;
}

// Token:

void Token_print(Token *token) {
    printf("Token { type = %s, token = \"%.*s\" }", token_types[token->type],
           (int)token->token.len, token->token.ptr);
}

// TokenCursor:

char TokenCursor_step(TokenCursor *cursor) {
    if (cursor->cursor >= cursor->str.len)
        return EOF;

    return *AsciiStr_get(&cursor->str, cursor->cursor++);
}

char TokenCursor_peek(TokenCursor *cursor) {
    if (cursor->cursor >= cursor->str.len)
        return EOF;

    return *AsciiStr_get(&cursor->str, cursor->cursor);
}

size_t TokenCursor_current_index(TokenCursor *cursor) {
    return cursor->cursor == 0 ? 0 : cursor->cursor - 1;
}

char TokenCursor_current_char(TokenCursor *cursor) {
    size_t current_index = TokenCursor_current_index(cursor);
    return *AsciiStr_get(&cursor->str, current_index);
}

// Parsing:

TokenParseResult token_parse(TokenVector *stream, AsciiStr input) {
    TokenParseResult result = {};

    TokenCursor cursor = {
        .str = input,
        .cursor = 0,
    };
    char c;

    Token token = {};

    while ((c = TokenCursor_step(&cursor)) != EOF) {
        size_t current_index = TokenCursor_current_index(&cursor);

        switch (c) {
        case '(':
            token.type = TOKEN_TYPE_L_PAREN;
            token.token =
                AsciiStr_substr(&input, current_index, current_index + 1);
            break;
        case ')':
            token.type = TOKEN_TYPE_R_PAREN;
            token.token =
                AsciiStr_substr(&input, current_index, current_index + 1);
            break;
        case ',':
            token.type = TOKEN_TYPE_COMMA;
            token.token =
                AsciiStr_substr(&input, current_index, current_index + 1);
            break;
        case ';':
            token.type = TOKEN_TYPE_SEMI;
            token.token =
                AsciiStr_substr(&input, current_index, current_index + 1);
            break;
        case '\'':
        case '"':
            if ((result = token_parse_str(stream, &cursor)) !=
                TOKENIZE_SUCCESS) {
                return result;
            }
            continue;
        default:
            if (token_isalnumlit(c)) {
                if ((result = token_parse_alnum(stream, &cursor)) !=
                    TOKENIZE_SUCCESS) {
                    return result;
                }
            } else if (isspace(c)) {
            } else {
                printf("Unknown token: %c\n", c);
            }
            continue;
        }

        TokenVector_push(stream, &token);
    }

    return result;
}

TokenParseResult token_parse_str(TokenVector *stream, TokenCursor *cursor) {
    size_t start_index = TokenCursor_current_index(cursor);
    char quote_type = TokenCursor_current_char(cursor);
    char c;

    if (quote_type != '\'' && quote_type != '"')
        return TOKENIZE_FAILURE;

    while ((c = TokenCursor_step(cursor)) != EOF) {
        switch (c) {
        case '\\':
            if ((c = TokenCursor_step(cursor)) == EOF)
                return TOKENIZE_FAILURE;

            break;
        case '\'':
        case '"':
            if (c == quote_type) {
                Token token = {
                    .type = TOKEN_TYPE_STR,
                    .token =
                        AsciiStr_substr(&cursor->str, start_index,
                                        TokenCursor_current_index(cursor) + 1),
                };
                TokenVector_push(stream, &token);
                return TOKENIZE_SUCCESS;
            }

            break;
        }
    }

    return TOKENIZE_FAILURE;
}

TokenParseResult token_parse_alnum(TokenVector *stream, TokenCursor *cursor) {
    char current_char = TokenCursor_current_char(cursor);

    if (isdigit(current_char)) {
        return token_parse_digit(stream, cursor);
    } else if (token_isalnumlit(current_char)) {
        return token_parse_alpha(stream, cursor);
    }

    return TOKENIZE_FAILURE;
}

TokenParseResult token_parse_digit(TokenVector *stream, TokenCursor *cursor) {
    size_t start_index = TokenCursor_current_index(cursor);
    char c;

    while ((c = TokenCursor_peek(cursor)) != EOF) {
        if (isspace(c) || !isalnum(c)) {
            break;
        }

        if (isalpha(c)) {
            return TOKENIZE_FAILURE;
        }

        TokenCursor_step(cursor);
    }

    Token token = {
        .type = TOKEN_TYPE_NUM,
        .token = AsciiStr_substr(&cursor->str, start_index,
                                 TokenCursor_current_index(cursor) + 1),
    };

    TokenVector_push(stream, &token);

    return TOKENIZE_SUCCESS;
}

TokenParseResult token_parse_alpha(TokenVector *stream, TokenCursor *cursor) {
    size_t start_index = TokenCursor_current_index(cursor);
    char c;

    while ((c = TokenCursor_peek(cursor)) != EOF && token_isalnumlit(c)) {
        TokenCursor_step(cursor);
    }

    Token token = {
        .type = TOKEN_TYPE_IDENT,
        .token = AsciiStr_substr(&cursor->str, start_index,
                                 TokenCursor_current_index(cursor) + 1),
    };

    TokenIsKeywordResult is_keyword = token_iskeyword(&token.token);

    if (is_keyword.is_keyword)
        token.type = is_keyword.token_type.token_type;

    TokenVector_push(stream, &token);

    return TOKENIZE_SUCCESS;
}

// Validation:

bool token_isalnumlit(char c) { return isalnum(c) || c == '_'; }

TokenIsKeywordResult token_iskeyword(AsciiStr *str) {
    TokenIsKeywordResult result = {.is_keyword = false};

    for (size_t i = 0; i < TOKEN_KEYWORDS_LEN; i++) {
        bool iskeyword = true;

        if (TOKEN_KEYWORDS[i].keyword.len > str->len)
            continue;

        for (size_t j = 0; j < str->len; j++) {
            if (*AsciiStr_get(&TOKEN_KEYWORDS[i].keyword, j) !=
                tolower(*AsciiStr_get(str, j))) {
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
