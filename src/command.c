#include "command.h"
#include <stdio.h>
#include <string.h>

ARRAY_IMPL(charArray);
ARRAY_IMPL(Command);

// Command:

ParseResult command_parse_input(CommandArray *commands, charArray *input) {
    TokenArray stream = {};
    TRY_PARSE(token_parse(&stream, input));

    StatementArray statements = {};
    TRY_PARSE(statement_parse(&statements, &stream));

    TRY_PARSE(command_parse(commands, &statements));

    return PARSE_OK;
}

ParseResult command_parse(CommandArray *commands, StatementArray *statements) {
    for (size_t i = 0; i < statements->len; i++) {
        Statement *statement = StatementArray_get(statements, i);

        switch (statement->type) {
        case STATEMENT_TYPE_SELECT:
            return CommandSelect_parse(commands, &statement->statement.select);
        case STATEMENT_TYPE_INSERT:
            return CommandInsert_parse(commands, &statement->statement.insert);
        }
    }

    return PARSE_ERR;
}

ParseResult CommandSelect_parse(CommandArray *commands,
                                StatementSelect *statement) {
    CommandSelect select_command = {};

    select_command.table = charArray_copy(&statement->table.slice);

    for (size_t i = 0; i < statement->fields.tokens.len; i++) {
        Token *token = TokenArray_get(&statement->fields.tokens, i);
        charArray field = charArray_copy(&token->slice);
        charArrayArray_push(&select_command.fields, &field);
    }

    CommandArray_push(commands, &(Command){.type = STATEMENT_TYPE_SELECT,
                                           .command.select = select_command});

    return PARSE_OK;
}

ParseResult CommandInsert_parse(CommandArray *commands,
                                StatementInsert *statement) {

    CommandInsert insert_command = {};

    insert_command.table = charArray_copy(&statement->table.slice);

    for (size_t i = 0; i < statement->fields.tokens.len; i++) {
        Token *token = TokenArray_get(&statement->fields.tokens, i);
        charArray field = charArray_copy(&token->slice);
        charArrayArray_push(&insert_command.fields, &field);
    }

    for (size_t i = 0; i < statement->values.tokens.len; i++) {
        Token *token = TokenArray_get(&statement->values.tokens, i);

        ValueParseResult result;
        TRY_PARSE((result = Value_parse(token)).err);

        ValueArray_push(&insert_command.values, &result.value.ok);
    }

    CommandArray_push(commands, &(Command){.type = STATEMENT_TYPE_INSERT,
                                           .command.insert = insert_command});

    return PARSE_OK;
}

// Printing:

void Command_print(Command *command) {
    switch (command->type) {
    case STATEMENT_TYPE_SELECT:
        CommandSelect_print(&command->command.select);
        break;
    case STATEMENT_TYPE_INSERT:
        CommandInsert_print(&command->command.insert);
        break;
    }
}

void CommandSelect_print(CommandSelect *command) {
    printf("CommandSelect {table = \"%.*s\", fields = [",
           (int)command->table.len, command->table.ptr);

    for (size_t i = 0; i < command->fields.len; i++) {
        charArray *field = charArrayArray_get(&command->fields, i);
        printf("\"%.*s\"", (int)field->len, field->ptr);

        if (i < command->fields.len - 1)
            printf(", ");
    }

    printf("]}");
}

void CommandInsert_print(CommandInsert *command) {
    printf("CommandInsert {table = \"%.*s\", fields = [",
           (int)command->table.len, command->table.ptr);

    for (size_t i = 0; i < command->fields.len; i++) {
        charArray *field = charArrayArray_get(&command->fields, i);
        printf("\"%.*s\"", (int)field->len, field->ptr);

        if (i < command->fields.len - 1)
            printf(", ");
    }

    printf("], values = [");

    for (size_t i = 0; i < command->values.len; i++) {
        Value *field = ValueArray_get(&command->values, i);
        Value_print(field);

        if (i < command->values.len - 1)
            printf(", ");
    }

    printf("]}");
}
