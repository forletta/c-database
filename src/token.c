#include "token.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

ARRAY_IMPL(Token);

bool TokenType_cmp(TokenType lhs, TokenType rhs) {
    if (lhs == TOKEN_TYPE_LIT || rhs == TOKEN_TYPE_LIT) {
        switch (lhs == TOKEN_TYPE_LIT ? rhs : lhs) {
        case TOKEN_TYPE_NUM:
        case TOKEN_TYPE_STR:
            return true;
        default:
            return false;
        }
    }

    return lhs == rhs;
}

// Token:

void Token_print(Token *token) {
    printf("Token { type = %s, token = \"%.*s\" }", token_types[token->type],
           (int)token->token.len, token->token.ptr);
}

// Parsing:

TokenParseResult token_parse(TokenArray *stream, charArray *input) {
    TokenParseResult result = {};

    charArrayIter cursor = charArrayIter_create(input);
    char *c;

    Token token = {};

    while ((c = charArrayIter_next(&cursor)) != NULL) {
        size_t current_index = charArrayIter_current_index(&cursor);

        switch (*c) {
        case '(':
            token.type = TOKEN_TYPE_L_PAREN;
            token.token =
                charArray_slice(input, current_index, current_index + 1);
            break;
        case ')':
            token.type = TOKEN_TYPE_R_PAREN;
            token.token =
                charArray_slice(input, current_index, current_index + 1);
            break;
        case ',':
            token.type = TOKEN_TYPE_COMMA;
            token.token =
                charArray_slice(input, current_index, current_index + 1);
            break;
        case ';':
            token.type = TOKEN_TYPE_SEMI;
            token.token =
                charArray_slice(input, current_index, current_index + 1);
            break;
        case '\'':
        case '"':
            if ((result = token_parse_str(stream, &cursor)) !=
                TOKENIZE_SUCCESS) {
                return result;
            }
            continue;
        default:
            if (token_isalnumlit(*c)) {
                if ((result = token_parse_alnum(stream, &cursor)) !=
                    TOKENIZE_SUCCESS) {
                    return result;
                }
            } else if (isspace(*c)) {
            } else {
                return TOKENIZE_FAILURE;
            }
            continue;
        }

        TokenArray_push(stream, &token);
    }

    return result;
}

TokenParseResult token_parse_str(TokenArray *stream, charArrayIter *cursor) {
    size_t start_index = charArrayIter_current_index(cursor);
    char quote_type = *charArrayIter_current(cursor);
    char *c;

    if (quote_type != '\'' && quote_type != '"')
        return TOKENIZE_FAILURE;

    while ((c = charArrayIter_next(cursor)) != NULL) {
        switch (*c) {
        case '\\':
            if ((c = charArrayIter_next(cursor)) == NULL)
                return TOKENIZE_FAILURE;

            break;
        case '\'':
        case '"':
            if (*c == quote_type) {
                Token token = {
                    .type = TOKEN_TYPE_STR,
                    .token = charArray_slice(
                        cursor->array, start_index,
                        charArrayIter_current_index(cursor) + 1),
                };
                TokenArray_push(stream, &token);
                return TOKENIZE_SUCCESS;
            }

            break;
        }
    }

    return TOKENIZE_FAILURE;
}

TokenParseResult token_parse_alnum(TokenArray *stream, charArrayIter *cursor) {
    char current_char = *charArrayIter_current(cursor);

    if (isdigit(current_char)) {
        return token_parse_digit(stream, cursor);
    } else if (token_isalnumlit(current_char)) {
        return token_parse_alpha(stream, cursor);
    }

    return TOKENIZE_FAILURE;
}

TokenParseResult token_parse_digit(TokenArray *stream, charArrayIter *cursor) {
    size_t start_index = charArrayIter_current_index(cursor);
    char *c;

    while ((c = charArrayIter_peek(cursor)) != NULL) {
        if (isspace(*c) || !isalnum(*c)) {
            break;
        }

        if (isalpha(*c)) {
            return TOKENIZE_FAILURE;
        }

        charArrayIter_next(cursor);
    }

    Token token = {
        .type = TOKEN_TYPE_NUM,
        .token = charArray_slice(cursor->array, start_index,
                                 charArrayIter_current_index(cursor) + 1),
    };

    TokenArray_push(stream, &token);

    return TOKENIZE_SUCCESS;
}

TokenParseResult token_parse_alpha(TokenArray *stream, charArrayIter *cursor) {
    size_t start_index = charArrayIter_current_index(cursor);
    char *c;

    while ((c = charArrayIter_peek(cursor)) != NULL && token_isalnumlit(*c)) {
        charArrayIter_next(cursor);
    }

    Token token = {
        .type = TOKEN_TYPE_IDENT,
        .token = charArray_slice(cursor->array, start_index,
                                 charArrayIter_current_index(cursor) + 1),
    };

    TokenIsKeywordResult is_keyword = token_iskeyword(&token.token);

    if (is_keyword.is_keyword)
        token.type = is_keyword.token_type.token_type;

    TokenArray_push(stream, &token);

    return TOKENIZE_SUCCESS;
}

// Validation:

bool token_isalnumlit(char c) { return isalnum(c) || c == '_'; }

TokenIsKeywordResult token_iskeyword(charArray *str) {
    TokenIsKeywordResult result = {.is_keyword = false};

    for (size_t i = 0; i < TOKEN_KEYWORDS_LEN; i++) {
        bool iskeyword = true;

        if (TOKEN_KEYWORDS[i].keyword.len > str->len)
            continue;

        for (size_t j = 0; j < str->len; j++) {
            if (*charArray_get(&TOKEN_KEYWORDS[i].keyword, j) !=
                tolower(*charArray_get(str, j))) {
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
