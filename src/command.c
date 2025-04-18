#include "command.h"
#include "c_core_error.h"
#include "void_vector.h"
#include <stdio.h>
#include <string.h>

// CommandVector:

VoidVector CommandVector_as_void_vector(CommandVector *v) {
    VoidVector void_v = {
        .ptr = (void **)&v->ptr,
        .len = &v->len,
        .cap = &v->cap,
        .element_size = sizeof(Statement),
    };

    return void_v;
}

Command *CommandVector_get(CommandVector *v, size_t i) {
    VoidVector void_v = CommandVector_as_void_vector(v);

    return VoidVector_get(&void_v, i);
}

void CommandVector_push(CommandVector *v, Command *command) {
    VoidVector void_v = CommandVector_as_void_vector(v);

    *(Command *)VoidVector_push(&void_v) = *command;
}

// void CommandVector_free(CommandVector *v) { VoidVector_free((VoidVector *)v); }

// Command:

CommandParseResult Command_parse(CommandVector *commands, Ast *ast) {
    for (size_t i = 0; i < ast->statements.len; i++) {
        Statement *statement = StatementVector_get(&ast->statements, i);

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

CommandParseResult CommandSelect_parse(CommandVector *commands,
                                       StatementSelect *statement) {
    CommandSelect select_command = {};

    select_command.table = AsciiString_copy_from_str(&statement->table.token);

    for (size_t i = 0; i < statement->fields.tokens.len; i++) {
        AsciiString field = AsciiString_copy_from_str(
            &TokenVector_get(&statement->fields.tokens, i)->token);
        AsciiStringVector_push(&select_command.fields, &field);
    }

    Command command = {
        .type = STATEMENT_TYPE_SELECT,
        .command.select = select_command,
    };
    CommandVector_push(commands, &command);

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
        AsciiString *field = AsciiStringVector_get(&command->fields, i);
        printf("\"%.*s\"", (int)field->len, field->ptr);
        if (i < command->fields.len - 1)
            printf(", ");
    }

    printf("]}");
}
