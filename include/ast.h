#ifndef AST_H
#define AST_H

#include "ascii_string.h"
#include "token.h"
#include <stdbool.h>

typedef struct {
    Token prens[2];
    TokenVector tokens;
    TokenVector commas;
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

typedef struct {
    Statement *ptr;
    size_t len;
    size_t cap;
} StatementVector;

typedef struct {
    AsciiStr input;
    StatementVector statements;
} Ast;

typedef enum {
    AST_PARSE_ERR,
    AST_PARSE_OK,
} AstParseResultType;

typedef struct {
    AstParseResultType type;
    union {
        struct {
        } err;
        Ast ok;
    } ast;
} AstParseResult;

// typedef struct {
//     enum {
//         AST_PARSE_STATEMENT_ERR,
//         AST_PARSE_STATEMENT_OK,
//     } type;
//     union {
//         struct {
//         } err;
//         Statement ok;
//     } statement;
// } AstParseStatementResult;

// typedef struct {
//     enum {
//         AST_PARSE_PUNCTUATED_ERR,
//         AST_PARSE_PUNCTUATED_OK,
//     } type;
//     union {
//         struct {
//         } err;
//         Punctuated ok;
//     } punctuated;
// } AstParsePunctuatedResult;

#define STATEMENT_TYPES_LEN 1
static const char *STATEMENT_TYPES[STATEMENT_TYPES_LEN] = {
    "STATEMENT_TYPE_SELECT",
};
// StatementVector:

Statement *StatementVector_get(StatementVector *v, size_t i);
void StatementVector_push(StatementVector *v, Statement *statement);
void StatementVector_free(StatementVector *v);

// Parsing:

AstParseResult Ast_parse(AsciiStr input);
AstParseResultType AstStatementSelect_parse(TokenVectorIter *iter,
                                            StatementVector *statements);
AstParseResultType AstStatementInsert_parse(TokenVectorIter *iter,
                                            StatementVector *statements);
AstParseResultType Punctuated_parse(TokenVectorIter *iter,
                                    Punctuated *punctuated,
                                    TokenType token_type);
// AstParseStatementResult ast_parse_statement_insert(TokenVectorIter *iter);
// AstParsePunctuatedResult ast_parse_punctuated_idents(TokenVectorIter *iter);

bool ast_try_parse_token(TokenVectorIter *iter, Token *target,
                         TokenType token_type);

// Printing:

void Ast_print(Ast *ast);
void Statement_print(Statement *statement);
void StatementSelect_print(StatementSelect *statement);
void StatementInsert_print(StatementInsert *statement);
void Punctuated_print(Punctuated *punctuated);

#endif // !AST_H
