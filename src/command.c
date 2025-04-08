#include "command.h"
#include <stdio.h>
#include <string.h>

ParseStatementResult parse_statement(InputBuffer *input_buffer,
                                     Statement *statement) {
    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_TYPE_INSERT;
        return PARSE_STATEMENT_SUCCESS;
    }

    if (strncmp(input_buffer->buffer, "select", 6) == 0) {
        statement->type = STATEMENT_TYPE_SELECT;
        return PARSE_STATEMENT_SUCCESS;
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
