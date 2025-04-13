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

    static const AstStatementParser STATEMENT_PARSERS[1] = {
        AstStatementSelect_parse,
    };

    while (TokenVectorIter_peek(&iter).type != TOKEN_TYPE_NULL) {
        AstParseResultType statement_parse_result = {};

        for (size_t i = 0; i < 1; i++) {
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

    if ((select_statement.select_token = TokenVectorIter_next(iter)).type !=
        TOKEN_TYPE_KW_SELECT) {
        TokenVectorIter_context_exit(iter);
        return AST_PARSE_ERR;
    }

    if ((select_statement.semi_token = TokenVectorIter_next(iter)).type !=
        TOKEN_TYPE_SEMI) {
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

// Printing:

void Ast_print(Ast *ast) {
    printf("Ast { .statements = [");

    for (size_t i = 0; i < ast->statements.len; i++) {
        Statement_print(StatementVector_get(&ast->statements, i));
        if (i < ast->statements.len - 1)
            printf(", ");
    }

    printf("] }\n");
}

void Statement_print(Statement *statement) {
    printf("Statement { .type = %s, .statement = ", STATEMENT_TYPES[statement->type]);

    switch (statement->type) {
        case STATEMENT_TYPE_SELECT:
            StatementSelect_print(&statement->statement.select);
    }

    printf(" }");
}

void StatementSelect_print(StatementSelect *statement) {
    printf("StatementSelect { .select_token = ");

    Token_print(&statement->select_token);

    printf(" }");
}
