#ifndef TOKEN_H
#define TOKEN_H

#include "vector.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define TOKEN_KEYWORDS_LEN 5

typedef enum {
    // Brackets
    TOKEN_TYPE_L_PAREN,
    TOKEN_TYPE_R_PAREN,

    // Punctuations
    TOKEN_TYPE_COMMA,
    TOKEN_TYPE_SEMI,

    // Literals
    TOKEN_TYPE_NUM,
    TOKEN_TYPE_IDENT,
    TOKEN_TYPE_STR,

    // Keywords
    TOKEN_TYPE_KW_INSERT,
    TOKEN_TYPE_KW_INTO,
    TOKEN_TYPE_KW_VALUES,
    TOKEN_TYPE_KW_SELECT,
    TOKEN_TYPE_KW_FROM,
} TokenType;

typedef struct {
    TokenType type;
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

typedef struct {
    TokenType token_type;
    size_t keyword_len;
    const char *keyword;
} TokenKeword;

typedef struct {
    bool is_keyword;
    union {
        struct {
        } null;
        TokenType token_type;
    } token_type;
} TokenIsKeywordResult;

static const char *token_types[12] = {
    "TOKEN_TYPE_L_PAREN",   "TOKEN_TYPE_R_PAREN",   "TOKEN_TYPE_COMMA",
    "TOKEN_TYPE_SEMI",      "TOKEN_TYPE_NUM",       "TOKEN_TYPE_IDENT",
    "TOKEN_TYPE_STR",       "TOKEN_TYPE_KW_INSERT", "TOKEN_TYPE_KW_INTO",
    "TOKEN_TYPE_KW_VALUES", "TOKEN_TYPE_KW_SELECT", "TOKEN_TYPE_KW_FROM"};

static const TokenKeword TOKEN_KEYWORDS[TOKEN_KEYWORDS_LEN] = {
    {
        .token_type = TOKEN_TYPE_KW_INSERT,
        .keyword_len = 6,
        .keyword = "insert",
    },
    {
        .token_type = TOKEN_TYPE_KW_INTO,
        .keyword_len = 4,
        .keyword = "into",
    },
    {
        .token_type = TOKEN_TYPE_KW_VALUES,
        .keyword_len = 6,
        .keyword = "values",
    },
    {
        .token_type = TOKEN_TYPE_KW_SELECT,
        .keyword_len = 6,
        .keyword = "select",
    },
    {
        .token_type = TOKEN_TYPE_KW_FROM,
        .keyword_len = 4,
        .keyword = "from",
    },
};

TokenParseResult token_parse(Vector *stream, const char *src, size_t src_size);
TokenParseResult token_parse_str(Vector *stream, TokenCursor *cursor,
                                 const char *token_start);
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
TokenIsKeywordResult token_iskeyword(const char *str, size_t str_size);

#endif /* ifndef TOKEN_H */
