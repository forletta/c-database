#include "command.h"
#include "token.h"
#include <stdio.h>
#include <string.h>

ParseStatementResult parse_statement(AsciiStr input, Statement *statement) {
    TokenVector stream = {};

    token_parse(&stream, input);

    for (size_t i = 0; i < stream.len; i++) {
        Token_print(TokenVector_get(&stream, i));
        printf("\n");
    }

    return PARSE_STATEMTNT_UNRECOGNIZED_STATEMENT;
}

void exec_statement(Statement *statement) {
    switch (statement->type) {
    case STATEMENT_TYPE_INSERT:
        printf("exec insert\n");
        break;
    case STATEMENT_TYPE_SELECT:
        printf("exec select\n");
        break;
    }
}
