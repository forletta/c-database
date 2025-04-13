#ifndef TOKEN_H
#define TOKEN_H

#include "ascii_string.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#define TOKEN_KEYWORDS_LEN 5

typedef enum {
    TOKEN_TYPE_NULL,

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
    AsciiStr token;
} Token;

typedef struct {
    Token *ptr;
    size_t len;
    size_t cap;
} TokenVector;

typedef struct {
    TokenVector *stream;
    size_t cursor;
    size_t context_start;
} TokenVectorIter;

typedef enum {
    TOKENIZE_SUCCESS,
    TOKENIZE_FAILURE,
} TokenParseResult;

typedef struct {
    AsciiStr str;
    size_t cursor;
} TokenCursor;

typedef struct {
    TokenType token_type;
    AsciiStr keyword;
} TokenKeword;

typedef struct {
    bool is_keyword;
    union {
        struct {
        } null;
        TokenType token_type;
    } token_type;
} TokenIsKeywordResult;

static const char *token_types[13] = {
    "TOKEN_TYPE_NULL",    "TOKEN_TYPE_L_PAREN",   "TOKEN_TYPE_R_PAREN",
    "TOKEN_TYPE_COMMA",   "TOKEN_TYPE_SEMI",      "TOKEN_TYPE_NUM",
    "TOKEN_TYPE_IDENT",   "TOKEN_TYPE_STR",       "TOKEN_TYPE_KW_INSERT",
    "TOKEN_TYPE_KW_INTO", "TOKEN_TYPE_KW_VALUES", "TOKEN_TYPE_KW_SELECT",
    "TOKEN_TYPE_KW_FROM"};

static const TokenKeword TOKEN_KEYWORDS[TOKEN_KEYWORDS_LEN] = {
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

// TokenVector:

Token *TokenVector_get(const TokenVector *v, size_t i);
void TokenVector_push(TokenVector *v, Token *token);
void TokenVector_free(TokenVector *v);

// TokenVector:

Token *TokenVectorIter_next(TokenVectorIter *iter);
Token *TokenVectorIter_peek(TokenVectorIter *iter);
void TokenVectorIter_context_enter(TokenVectorIter *iter);
void TokenVectorIter_context_exit(TokenVectorIter *iter);

// Token:

void Token_print(Token *token);

// TokenCursor:

char TokenCursor_step(TokenCursor *cursor);
size_t TokenCursor_current_index(TokenCursor *cursor);
char TokenCursor_current_char(TokenCursor *cursor);
char TokenCursor_peek(TokenCursor *cursor);

// Parsing:

TokenParseResult token_parse(TokenVector *stream, AsciiStr input);
TokenParseResult token_parse_str(TokenVector *stream, TokenCursor *cursor);
TokenParseResult token_parse_alnum(TokenVector *stream, TokenCursor *cursor);
TokenParseResult token_parse_digit(TokenVector *stream, TokenCursor *cursor);
TokenParseResult token_parse_alpha(TokenVector *stream, TokenCursor *cursor);

// Validation:

bool token_isalnumlit(char c);
TokenIsKeywordResult token_iskeyword(AsciiStr *str);

#endif /* ifndef TOKEN_H */
