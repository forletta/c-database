#include "token.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

ARRAY_IMPL(Token);

// Helper Functions:

void TokenArray_push_slice(TokenArray *stream, charArrayIter *iter,
                           TokenType token_type, size_t slice_start) {
    size_t slice_end = charArrayIter_current_index(iter) + 1;
    charArray slice = charArray_slice(iter->array, slice_start, slice_end);

    TokenArray_push(stream, &(Token){.type = token_type, .slice = slice});
}

// TokenType:

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

// Parsing:

ParseResult token_parse(TokenArray *stream, charArray *input) {
    charArrayIter iter = charArrayIter_create(input);
    char *c;

    while ((c = charArrayIter_next(&iter)) != NULL) {
        TokenType token_type = {};

        switch (*c) {
        case '(':
            token_type = TOKEN_TYPE_L_PAREN;
            break;
        case ')':
            token_type = TOKEN_TYPE_R_PAREN;
            break;
        case ',':
            token_type = TOKEN_TYPE_COMMA;
            break;
        case ';':
            token_type = TOKEN_TYPE_SEMI;
            break;
        case '\'':
        case '"':
            TRY_PARSE(token_parse_str(stream, &iter));
            continue;
        default:
            if (isspace(*c))
                continue;
            if (token_isalnumlit(*c) && token_parse_alnum(stream, &iter))
                continue;

            return PARSE_ERR;
        }

        size_t slice_start = charArrayIter_current_index(&iter);
        TokenArray_push_slice(stream, &iter, token_type, slice_start);
    }

    return PARSE_OK;
}

ParseResult token_parse_str(TokenArray *stream, charArrayIter *iter) {
    char quote_type = *charArrayIter_current(iter);

    if (quote_type != '\'' && quote_type != '"')
        return PARSE_ERR;

    size_t slice_start = charArrayIter_current_index(iter);
    char *c;

    while ((c = charArrayIter_next(iter)) != NULL) {
        switch (*c) {
        case '\\':
            TRY_PARSE((c = charArrayIter_next(iter)));
            break;
        case '\'':
        case '"':
            if (*c != quote_type)
                break;

            TokenArray_push_slice(stream, iter, TOKEN_TYPE_STR, slice_start);
            return PARSE_OK;
        }
    }

    return PARSE_ERR;
}

ParseResult token_parse_alnum(TokenArray *stream, charArrayIter *iter) {
    char current_char = *charArrayIter_current(iter);

    if (isdigit(current_char)) {
        return token_parse_digit(stream, iter);
    } else if (token_isalnumlit(current_char)) {
        return token_parse_alpha(stream, iter);
    }

    return PARSE_ERR;
}

ParseResult token_parse_digit(TokenArray *stream, charArrayIter *iter) {
    size_t slice_start = charArrayIter_current_index(iter);
    char *c;

    while ((c = charArrayIter_peek(iter)) != NULL) {
        if (isspace(*c) || !isalnum(*c))
            break;

        if (isalpha(*c))
            return PARSE_ERR;

        charArrayIter_next(iter);
    }

    TokenArray_push_slice(stream, iter, TOKEN_TYPE_NUM, slice_start);
    return PARSE_OK;
}

ParseResult token_parse_alpha(TokenArray *stream, charArrayIter *iter) {
    size_t slice_start = charArrayIter_current_index(iter);
    char *c;

    while ((c = charArrayIter_peek(iter)) != NULL && token_isalnumlit(*c)) {
        charArrayIter_next(iter);
    }

    size_t slice_end = charArrayIter_current_index(iter) + 1;
    charArray slice = charArray_slice(iter->array, slice_start, slice_end);
    TokenIsKeywordResult is_keyword = token_iskeyword(&slice);
    TokenType token_type = TOKEN_TYPE_IDENT;

    if (is_keyword.is_keyword)
        token_type = is_keyword.token_type.token_type;

    TokenArray_push_slice(stream, iter, token_type, slice_start);
    return PARSE_OK;
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

// Printing:

void Token_print(Token *token) {
    printf("Token {type = %s, token = \"%.*s\"}", token_types[token->type],
           (int)token->slice.len, token->slice.ptr);
}
