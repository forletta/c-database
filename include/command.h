#ifndef COMMAND_H
#define COMMAND_H

#include "array.h"
#include "ascii_string.h"
#include "ast.h"

ARRAY(charArray);

typedef struct {
    charArray table;
    charArrayArray fields;
} CommandSelect;

typedef struct {
    StatementType type;
    union {
        CommandSelect select;
        // CommandInsert insert;
    } command;
} Command;

ARRAY(Command);

typedef enum {
    COMMAND_PARSE_ERR,
    COMMAND_PARSE_OK,
} CommandParseResult;

// Command:

CommandParseResult Command_parse(CommandArray *commands, Ast *ast);
CommandParseResult CommandSelect_parse(CommandArray *commands,
                                       StatementSelect *statement);

// Printing:

void Command_print(Command *command);
void CommandSelect_print(CommandSelect *command);

#endif // !COMMAND_H
