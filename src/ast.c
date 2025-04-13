#include "ast.h"
#include "c_core_error.h"
#include "token.h"
#include "void_vector.h"
#include <stdio.h>

// StatementVector:

Statement *StatementVector_get(const StatementVector *v, size_t i) {
    if (i < v->len)
        return v->ptr + i;

    out_of_bounds();
}

void StatementVector_push(StatementVector *v, Statement *statement) {

    VoidVector_ensure_capacity((VoidVector *)v, sizeof(Token), 1);

    v->ptr[v->len++] = *statement;
}

// Parsing:

typedef AstParseResultType (*AstStatementParser)(TokenVectorIter *,
                                                 StatementVector *);

AstParseResult Ast_parse(AsciiStr input) {
    AstParseResult result = {};

    TokenVector stream = {};
    if (token_parse(&stream, input) == TOKENIZE_FAILURE)
        return result;
    TokenVectorIter iter = {
        .stream = &stream,
        .cursor = 0,
    };

    static const AstStatementParser STATEMENT_PARSERS[2] = {
        AstStatementSelect_parse,
        AstStatementInsert_parse,
    };

    while (TokenVectorIter_peek(&iter).type != TOKEN_TYPE_NULL) {
        AstParseResultType statement_parse_result = {};

        for (size_t i = 0; i < 2; i++) {
            statement_parse_result =
                STATEMENT_PARSERS[i](&iter, &result.ast.ok.statements);

            if (statement_parse_result == AST_PARSE_OK)
                break;
        }

        if (statement_parse_result == AST_PARSE_ERR)
            return result;
    }

    result.type = AST_PARSE_OK;
    return result;
}

AstParseResultType AstStatementSelect_parse(TokenVectorIter *iter,
                                            StatementVector *statements) {
    TokenVectorIter_context_enter(iter);
    StatementSelect select_statement = {};

    if (!ast_try_parse_token(iter, &select_statement.select_token,
                             TOKEN_TYPE_KW_SELECT)) {
        return AST_PARSE_ERR;
    }

    if (Punctuated_parse(iter, &select_statement.fields, TOKEN_TYPE_IDENT) ==
        AST_PARSE_ERR)
        return AST_PARSE_ERR;

    if (!ast_try_parse_token(iter, &select_statement.from_token,
                             TOKEN_TYPE_KW_FROM)) {
        TokenVectorIter_context_exit(iter);
        return AST_PARSE_ERR;
    }

    if (!ast_try_parse_token(iter, &select_statement.table, TOKEN_TYPE_IDENT)) {
        TokenVectorIter_context_exit(iter);
        return AST_PARSE_ERR;
    }

    if (!ast_try_parse_token(iter, &select_statement.semi_token,
                             TOKEN_TYPE_SEMI)) {
        TokenVectorIter_context_exit(iter);
        return AST_PARSE_ERR;
    }

    Statement statement = {
        .type = STATEMENT_TYPE_SELECT,
        .statement.select = select_statement,
    };
    StatementVector_push(statements, &statement);
    return AST_PARSE_OK;
}

AstParseResultType AstStatementInsert_parse(TokenVectorIter *iter,
                                            StatementVector *statements) {
    TokenVectorIter_context_enter(iter);
    StatementInsert insert_statement = {};

    if (!ast_try_parse_token(iter, &insert_statement.insert_token,
                             TOKEN_TYPE_KW_INSERT)) {
        return AST_PARSE_ERR;
    }

    if (!ast_try_parse_token(iter, &insert_statement.into_token,
                             TOKEN_TYPE_KW_INTO)) {
        return AST_PARSE_ERR;
    }

    if (!ast_try_parse_token(iter, &insert_statement.table, TOKEN_TYPE_IDENT)) {
        return AST_PARSE_ERR;
    }

    if (Punctuated_parse(iter, &insert_statement.fields, TOKEN_TYPE_IDENT) ==
        AST_PARSE_ERR)
        return AST_PARSE_ERR;

    if (!ast_try_parse_token(iter, &insert_statement.values_token,
                             TOKEN_TYPE_KW_VALUES)) {
        TokenVectorIter_context_exit(iter);
        return AST_PARSE_ERR;
    }

    if (Punctuated_parse(iter, &insert_statement.values, TOKEN_TYPE_LIT) ==
        AST_PARSE_ERR)
        return AST_PARSE_ERR;

    if (!ast_try_parse_token(iter, &insert_statement.semi_token,
                             TOKEN_TYPE_SEMI)) {
        TokenVectorIter_context_exit(iter);
        return AST_PARSE_ERR;
    }

    Statement statement = {
        .type = STATEMENT_TYPE_INSERT,
        .statement.insert = insert_statement,
    };
    StatementVector_push(statements, &statement);
    return AST_PARSE_OK;
}

AstParseResultType Punctuated_parse(TokenVectorIter *iter,
                                    Punctuated *punctuated,
                                    TokenType token_type) {
    Token token;

    bool parenthesized = false;
    bool parentheses_closed = false;

    if (TokenType_cmp((token = TokenVectorIter_peek(iter)).type,
                      TOKEN_TYPE_L_PAREN)) {
        punctuated->prens[0] = token;
        parenthesized = true;
        TokenVectorIter_next(iter);
    }

    if (!ast_try_parse_token(iter, &token, token_type)) {
        return AST_PARSE_ERR;
    }

    TokenVector_push(&punctuated->tokens, &token);
    bool expecting_ident = false;

    while ((token = TokenVectorIter_peek(iter)).type != TOKEN_TYPE_NULL) {
        if (expecting_ident && TokenType_cmp(token.type, token_type)) {
            TokenVector_push(&punctuated->tokens, &token);
            expecting_ident = false;

            TokenVectorIter_next(iter);
            continue;
        }

        if (!expecting_ident && TokenType_cmp(token.type, TOKEN_TYPE_COMMA)) {
            TokenVector_push(&punctuated->commas, &token);
            expecting_ident = true;

            TokenVectorIter_next(iter);
            continue;
        }

        if (!expecting_ident && TokenType_cmp(token.type, TOKEN_TYPE_R_PAREN)) {
            if (!parenthesized) {
                TokenVectorIter_context_exit(iter);
                return AST_PARSE_ERR;
            }

            punctuated->prens[1] = token;
            parentheses_closed = true;

            TokenVectorIter_next(iter);
            break;
        }

        break;
    }

    return parenthesized && !parentheses_closed ? AST_PARSE_ERR : AST_PARSE_OK;
}

bool ast_try_parse_token(TokenVectorIter *iter, Token *target,
                         TokenType token_type) {
    if (!TokenType_cmp((*target = TokenVectorIter_next(iter)).type,
                       token_type)) {
        TokenVectorIter_context_exit(iter);
        return false;
    }

    return true;
}

// Printing:

void Ast_print(Ast *ast) {
    printf("Ast {.statements = [");

    for (size_t i = 0; i < ast->statements.len; i++) {
        Statement_print(StatementVector_get(&ast->statements, i));
        if (i < ast->statements.len - 1)
            printf(", ");
    }

    printf("]}\n");
}

void Statement_print(Statement *statement) {
    printf("Statement {.type = %s, .statement = ",
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
    printf("StatementSelect {.select_token = ");
    Token_print(&statement->select_token);

    printf(", .fields = ");
    Punctuated_print(&statement->fields);

    printf(", .from_token = ");
    Token_print(&statement->from_token);

    printf(", .table = ");
    Token_print(&statement->table);

    printf(", .semi_token = ");
    Token_print(&statement->semi_token);

    printf("}");
}

void StatementInsert_print(StatementInsert *statement) {
    printf("StatementSelect {.insert_token = ");
    Token_print(&statement->insert_token);

    printf(", .into_token = ");
    Token_print(&statement->into_token);

    printf(", .table = ");
    Token_print(&statement->table);

    printf(", .fields = ");
    Punctuated_print(&statement->fields);

    printf(", .values_token = ");
    Token_print(&statement->values_token);

    printf(", .values = ");
    Punctuated_print(&statement->values);

    printf(", .semi_token = ");
    Token_print(&statement->semi_token);

    printf("}");
}

void Punctuated_print(Punctuated *punctuated) {
    printf("Punctuated {.parens = [");

    Token_print(&punctuated->prens[0]);
    Token_print(&punctuated->prens[1]);

    printf("], .tokens = [");
    for (size_t i = 0; i < punctuated->tokens.len; i++) {
        Token_print(TokenVector_get(&punctuated->tokens, i));
        if (i < punctuated->tokens.len - 1)
            printf(", ");
    }

    printf("]}");
}
