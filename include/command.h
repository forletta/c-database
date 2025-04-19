#ifndef COMMAND_H
#define COMMAND_H

#include "array.h"
#include "statement.h"
#include "value.h"

ARRAY(charArray);

typedef struct {
    charArray table;
    charArrayArray fields;
} CommandSelect;

typedef struct {
    charArray table;
    charArrayArray fields;
    ValueArray values;
} CommandInsert;

typedef struct {
    StatementType type;
    union {
        CommandSelect select;
        CommandInsert insert;
    } command;
} Command;

ARRAY(Command);

// Command:

ParseResult command_parse_input(CommandArray *commands, charArray *input);
ParseResult command_parse(CommandArray *commands, StatementArray *statements);
ParseResult CommandSelect_parse(CommandArray *commands,
                                StatementSelect *statement);
ParseResult CommandInsert_parse(CommandArray *commands,
                                StatementInsert *statement);

// Printing:

void Command_print(Command *command);
void CommandSelect_print(CommandSelect *command);
void CommandInsert_print(CommandInsert *command);

#endif // !COMMAND_H
