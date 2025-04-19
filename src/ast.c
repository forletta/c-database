#include "ast.h"
#include "token.h"
#include <stdio.h>

ARRAY_IMPL(Statement);

// Helper macros:

#define AST_TRY_PARSE_TOKEN(target, token_type)                                \
    if (!ast_try_parse_token(iter, target, token_type))                        \
        return PARSE_ERR;

#define AST_TRY_PARSE_PUNCTUATED(target, token_type)                           \
    if (Punctuated_parse(iter, target, token_type) == PARSE_ERR)               \
        return PARSE_ERR;

// Parsing:

typedef ParseResult (*AstStatementParser)(TokenArrayIter *, StatementArray *);

AstParseResult Ast_parse(charArray *input) {
    static const AstStatementParser STATEMENT_PARSERS[2] = {
        AstStatementSelect_parse,
        AstStatementInsert_parse,
    };

    TokenArray stream = {};
    if (token_parse(&stream, input) == PARSE_ERR)
        return (AstParseResult){};
    TokenArrayIter iter = TokenArrayIter_create(&stream);

    AstParseResult result = {.type = PARSE_OK};

    while (TokenArrayIter_peek(&iter) != NULL) {
        ParseResult statement_parse_result = {};
        size_t current_token_index = iter.cursor;

        for (size_t i = 0; i < 2; i++) {
            iter.cursor = current_token_index;

            statement_parse_result =
                STATEMENT_PARSERS[i](&iter, &result.ast.ok.statements);

            if (statement_parse_result == PARSE_OK)
                break;
        }

        if (statement_parse_result == PARSE_ERR)
            return (AstParseResult){};
    }

    return result;
}

ParseResult AstStatementSelect_parse(TokenArrayIter *iter,
                                     StatementArray *statements) {
    StatementSelect select_statement = {};

    AST_TRY_PARSE_TOKEN(&select_statement.select_token, TOKEN_TYPE_KW_SELECT);
    AST_TRY_PARSE_PUNCTUATED(&select_statement.fields, TOKEN_TYPE_IDENT);
    AST_TRY_PARSE_TOKEN(&select_statement.from_token, TOKEN_TYPE_KW_FROM);
    AST_TRY_PARSE_TOKEN(&select_statement.table, TOKEN_TYPE_IDENT);
    AST_TRY_PARSE_TOKEN(&select_statement.semi_token, TOKEN_TYPE_SEMI);

    Statement statement = {
        .type = STATEMENT_TYPE_SELECT,
        .statement.select = select_statement,
    };
    StatementArray_push(statements, &statement);

    return PARSE_OK;
}

ParseResult AstStatementInsert_parse(TokenArrayIter *iter,
                                     StatementArray *statements) {
    StatementInsert insert_statement = {};

    AST_TRY_PARSE_TOKEN(&insert_statement.insert_token, TOKEN_TYPE_KW_INSERT);
    AST_TRY_PARSE_TOKEN(&insert_statement.into_token, TOKEN_TYPE_KW_INTO);
    AST_TRY_PARSE_TOKEN(&insert_statement.table, TOKEN_TYPE_IDENT);
    AST_TRY_PARSE_PUNCTUATED(&insert_statement.fields, TOKEN_TYPE_IDENT);
    AST_TRY_PARSE_TOKEN(&insert_statement.values_token, TOKEN_TYPE_KW_VALUES);
    AST_TRY_PARSE_PUNCTUATED(&insert_statement.values, TOKEN_TYPE_LIT);
    AST_TRY_PARSE_TOKEN(&insert_statement.semi_token, TOKEN_TYPE_SEMI);

    Statement statement = {
        .type = STATEMENT_TYPE_INSERT,
        .statement.insert = insert_statement,
    };
    StatementArray_push(statements, &statement);

    return PARSE_OK;
}

ParseResult Punctuated_parse(TokenArrayIter *iter, Punctuated *punctuated,
                             TokenType token_type) {
    Token *token;

    bool parenthesized = false;
    bool parentheses_closed = false;

    if ((token = TokenArrayIter_peek(iter)) == NULL)
        return PARSE_ERR;

    if (TokenType_cmp(token->type, TOKEN_TYPE_L_PAREN)) {
        punctuated->prens[0] = *token;
        parenthesized = true;

        TokenArrayIter_next(iter);
    }

    AST_TRY_PARSE_TOKEN(token, token_type);
    TokenArray_push(&punctuated->tokens, token);

    bool expecting_ident = false;

    while ((token = TokenArrayIter_peek(iter)) != NULL) {
        if (expecting_ident && TokenType_cmp(token->type, token_type)) {
            TokenArray_push(&punctuated->tokens, token);
            expecting_ident = false;

            TokenArrayIter_next(iter);
            continue;
        }

        if (!expecting_ident && TokenType_cmp(token->type, TOKEN_TYPE_COMMA)) {
            TokenArray_push(&punctuated->commas, token);
            expecting_ident = true;

            TokenArrayIter_next(iter);
            continue;
        }

        if (!expecting_ident &&
            TokenType_cmp(token->type, TOKEN_TYPE_R_PAREN)) {
            if (!parenthesized) {
                return PARSE_ERR;
            }

            punctuated->prens[1] = *token;
            parentheses_closed = true;

            TokenArrayIter_next(iter);
            break;
        }

        break;
    }

    return parenthesized && !parentheses_closed ? PARSE_ERR : PARSE_OK;
}

bool ast_try_parse_token(TokenArrayIter *iter, Token *target,
                         TokenType token_type) {
    Token *token;
    if ((token = TokenArrayIter_next(iter)) == NULL)
        return false;

    *target = *token;

    return TokenType_cmp(target->type, token_type);
}

// Printing:

void Ast_print(Ast *ast) {
    printf("Ast {statements = [");

    for (size_t i = 0; i < ast->statements.len; i++) {
        Statement_print(StatementArray_get(&ast->statements, i));
        if (i < ast->statements.len - 1)
            printf(", ");
    }

    printf("]}\n");
}

void Statement_print(Statement *statement) {
    printf("Statement {type = %s, statement = ",
           STATEMENT_TYPES[statement->type]);

    switch (statement->type) {
    case STATEMENT_TYPE_SELECT:
        StatementSelect_print(&statement->statement.select);
        break;
    case STATEMENT_TYPE_INSERT:
        StatementInsert_print(&statement->statement.insert);
        break;
    }

    printf("}");
}

void StatementSelect_print(StatementSelect *statement) {
    printf("StatementSelect {select_token = ");
    Token_print(&statement->select_token);

    printf(", fields = ");
    Punctuated_print(&statement->fields);

    printf(", from_token = ");
    Token_print(&statement->from_token);

    printf(", table = ");
    Token_print(&statement->table);

    printf(", semi_token = ");
    Token_print(&statement->semi_token);

    printf("}");
}

void StatementInsert_print(StatementInsert *statement) {
    printf("StatementInsert {insert_token = ");
    Token_print(&statement->insert_token);

    printf(", into_token = ");
    Token_print(&statement->into_token);

    printf(", table = ");
    Token_print(&statement->table);

    printf(", fields = ");
    Punctuated_print(&statement->fields);

    printf(", values_token = ");
    Token_print(&statement->values_token);

    printf(", values = ");
    Punctuated_print(&statement->values);

    printf(", semi_token = ");
    Token_print(&statement->semi_token);

    printf("}");
}

void Punctuated_print(Punctuated *punctuated) {
    printf("Punctuated {parens = [");

    Token_print(&punctuated->prens[0]);
    Token_print(&punctuated->prens[1]);

    printf("], tokens = [");
    for (size_t i = 0; i < punctuated->tokens.len; i++) {
        Token_print(TokenArray_get(&punctuated->tokens, i));
        if (i < punctuated->tokens.len - 1)
            printf(", ");
    }

    printf("]}");
}
