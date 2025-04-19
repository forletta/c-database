#ifndef TOKEN_H
#define TOKEN_H

#include "char_array.h"
#include "parse.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define TOKEN_KEYWORDS_LEN 5

typedef enum {
    // Meta tokens
    TOKEN_TYPE_NULL,
    TOKEN_TYPE_LIT,

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
    charArray slice;
} Token;

ARRAY(Token);

// typedef enum {
//     TOKEN_PARSE_ERR,
//     TOKEN_PARSE_OK,
// } TokenParseResult;

typedef struct {
    TokenType token_type;
    charArray keyword;
} TokenKeword;

typedef struct {
    bool is_keyword;
    union {
        struct {
        } null;
        TokenType token_type;
    } token_type;
} TokenIsKeywordResult;

static const char *token_types[14] = {
    "TOKEN_TYPE_NULL",      "TOKEN_TYPE_LIT",     "TOKEN_TYPE_L_PAREN",
    "TOKEN_TYPE_R_PAREN",   "TOKEN_TYPE_COMMA",   "TOKEN_TYPE_SEMI",
    "TOKEN_TYPE_NUM",       "TOKEN_TYPE_IDENT",   "TOKEN_TYPE_STR",
    "TOKEN_TYPE_KW_INSERT", "TOKEN_TYPE_KW_INTO", "TOKEN_TYPE_KW_VALUES",
    "TOKEN_TYPE_KW_SELECT", "TOKEN_TYPE_KW_FROM"};

static TokenKeword TOKEN_KEYWORDS[TOKEN_KEYWORDS_LEN] = {
    {
        .token_type = TOKEN_TYPE_KW_INSERT,
        .keyword =
            {
                .ptr = "insert",
                .len = 6,
            },
    },
    {
        .token_type = TOKEN_TYPE_KW_INTO,
        .keyword =
            {
                .ptr = "into",
                .len = 4,
            },
    },
    {
        .token_type = TOKEN_TYPE_KW_VALUES,
        .keyword =
            {
                .ptr = "values",
                .len = 6,
            },
    },
    {
        .token_type = TOKEN_TYPE_KW_SELECT,
        .keyword =
            {
                .ptr = "select",
                .len = 6,
            },
    },
    {
        .token_type = TOKEN_TYPE_KW_FROM,
        .keyword =
            {
                .ptr = "from",
                .len = 4,
            },
    },
};

// TokenType:

bool TokenType_cmp(TokenType lhs, TokenType rhs);

// Parsing:

ParseResult token_parse(TokenArray *stream, charArray *input);
ParseResult token_parse_str(TokenArray *stream, charArrayIter *cursor);
ParseResult token_parse_alnum(TokenArray *stream, charArrayIter *cursor);
ParseResult token_parse_digit(TokenArray *stream, charArrayIter *cursor);
ParseResult token_parse_alpha(TokenArray *stream, charArrayIter *cursor);

// Validation:

bool token_isalnumlit(char c);
TokenIsKeywordResult token_iskeyword(charArray *str);

// Printing:

void Token_print(Token *token);

#endif /* ifndef TOKEN_H */
