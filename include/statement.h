#ifndef AST_H
#define AST_H

#include "array.h"
#include "token.h"
#include <stdbool.h>

typedef struct {
    Token prens[2];
    TokenArray tokens;
    TokenArray commas;
} Punctuated;

typedef Punctuated PunctuatedIdents;
typedef Punctuated PunctuatedLiterals;
typedef Token Keyword;
typedef Token Semi;
typedef Token Ident;

typedef struct {
    Keyword select_token;
    PunctuatedIdents fields;
    Keyword from_token;
    Ident table;
    Semi semi_token;
} StatementSelect;

typedef struct {
    Keyword insert_token;
    Keyword into_token;
    Ident table;
    PunctuatedIdents fields;
    Keyword values_token;
    PunctuatedLiterals values;
    Semi semi_token;
} StatementInsert;

typedef enum {
    STATEMENT_TYPE_SELECT,
    STATEMENT_TYPE_INSERT,
} StatementType;

typedef struct {
    StatementType type;
    union {
        StatementSelect select;
        StatementInsert insert;
    } statement;
} Statement;

ARRAY(Statement);

#define STATEMENT_TYPES_LEN 2
static const char *STATEMENT_TYPES[STATEMENT_TYPES_LEN] = {
    "STATEMENT_TYPE_SELECT",
    "STATEMENT_TYPE_INSERT",
};

// Parsing:

ParseResult statement_parse(StatementArray *statements, TokenArray *stream);
ParseResult StatementSelect_parse(TokenArrayIter *iter,
                                  StatementArray *statements);
ParseResult StatementInsert_parse(TokenArrayIter *iter,
                                  StatementArray *statements);
ParseResult Punctuated_parse(TokenArrayIter *iter, Punctuated *punctuated,
                             TokenType token_type);

bool ast_try_parse_token(TokenArrayIter *iter, Token *target,
                         TokenType token_type);

// Printing:

void StatementArray_print(StatementArray *statements);
void Statement_print(Statement *statement);
void StatementSelect_print(StatementSelect *statement);
void StatementInsert_print(StatementInsert *statement);
void Punctuated_print(Punctuated *punctuated);

#endif // !AST_H
