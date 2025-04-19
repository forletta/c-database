#include "command.h"
#include <stdio.h>
#include <string.h>

ARRAY_IMPL(charArray);
ARRAY_IMPL(Command);

// Command:

CommandParseResult Command_parse(CommandArray *commands, Ast *ast) {
    for (size_t i = 0; i < ast->statements.len; i++) {
        Statement *statement = StatementArray_get(&ast->statements, i);

        switch (statement->type) {
        case STATEMENT_TYPE_SELECT:
            return CommandSelect_parse(commands, &statement->statement.select);
        default:
            printf("Command type unimplemented");
            return COMMAND_PARSE_ERR;
        }
    }

    return COMMAND_PARSE_ERR;
}

CommandParseResult CommandSelect_parse(CommandArray *commands,
                                       StatementSelect *statement) {
    CommandSelect select_command = {};

    select_command.table = charArray_copy(&statement->table.token);

    for (size_t i = 0; i < statement->fields.tokens.len; i++) {
        charArray field = charArray_copy(
            &TokenArray_get(&statement->fields.tokens, i)->token);
        charArrayArray_push(&select_command.fields, &field);
    }

    Command command = {
        .type = STATEMENT_TYPE_SELECT,
        .command.select = select_command,
    };
    CommandArray_push(commands, &command);

    return COMMAND_PARSE_OK;
}

// Printing:

void Command_print(Command *command) {
    switch (command->type) {
    case STATEMENT_TYPE_SELECT:
        CommandSelect_print(&command->command.select);
        break;
    default:
        printf("Command statement type not implemented");
    }
}

void CommandSelect_print(CommandSelect *command) {
    printf("CommandSelect {.table = \"%.*s\", .fields = [",
           (int)command->table.len, command->table.ptr);

    for (size_t i = 0; i < command->fields.len; i++) {
        charArray *field = charArrayArray_get(&command->fields, i);
        printf("\"%.*s\"", (int)field->len, field->ptr);
        if (i < command->fields.len - 1)
            printf(", ");
    }

    printf("]}");
}
