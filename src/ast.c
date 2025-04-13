#include "ast.h"
#include "token.h"
#include "void_vector.h"

// StatementVector:

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

    while (TokenVectorIter_peek(&iter) != NULL) {
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

    if ((select_statement.select_token = *TokenVectorIter_next(iter)).type !=
        TOKEN_TYPE_KW_SELECT) {
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
