#ifndef COMMAND_H
#define COMMAND_H

#include "input.h"

typedef enum {
    STATEMENT_TYPE_INSERT,
    STATEMENT_TYPE_SELECT,
} StatementType;

typedef struct {
    StatementType type;
} Statement;

typedef enum {
    PARSE_STATEMENT_SUCCESS,
    PARSE_STATEMTNT_UNRECOGNIZED_STATEMENT,
} ParseStatementResult;

ParseStatementResult parse_statement(InputBuffer *input_buffer,
                                     Statement *statement);

void exec_statement(Statement *statement);

#endif // !COMMAND_H
