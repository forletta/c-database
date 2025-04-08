#include "command.h"
#include "input.h"
#include "meta_command.h"
#include "table.h"
#include <stdbool.h>
#include <stdio.h>

void print_prompt() { printf("db > "); }

int main(int argc, char *argv[]) {
    InputBuffer input_buffer = {
        .buffer = NULL,
        .length = 0,
        .capacity = 0,
    };

    size_t fieldc = 2;
    char *field_names[2] = {"Name", "Age"};
    FieldDataType field_data_types[2] = {
        {.type = FIELD_DATA_TYPE_CHAR,
         .metadata = {.char_type =
                          {
                              .capacity = 20,
                          }}},
        {.type = FIELD_DATA_TYPE_INT, .metadata = {}}};
    TableSchema example_schema = {
        .fieldc = fieldc,
        .field_names = field_names,
        .field_data_types = field_data_types,
    };

    print_schema(&example_schema);
    printf("\n");

    while (true) {
        print_prompt();
        read_input(&input_buffer);

        if (input_buffer.buffer[0] == '.') {
            switch (exec_meta_command(&input_buffer)) {
            case META_COMMAND_SUCCESS:
                continue;
            case META_COMMAND_UNRECOGNIZED_COMMAND:
                printf("Unrecognized command: %s\n", input_buffer.buffer);
                continue;
            }
        }

        Statement statement;
        switch (parse_statement(&input_buffer, &statement)) {
        case PARSE_STATEMENT_SUCCESS:
            break;
        case PARSE_STATEMTNT_UNRECOGNIZED_STATEMENT:
            printf("Failed to parse statement: %s\n", input_buffer.buffer);
            continue;
        }

        exec_statement(&statement);
    }

    return 0;
}
