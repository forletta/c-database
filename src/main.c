#include "ascii_string.h"
// #include "command.h"
#include "ast.h"
#include "io.h"
#include "meta_command.h"
#include "table.h"
#include <stdbool.h>
#include <stdio.h>

void print_prompt() { printf("db > "); }

int main(int argc, char *argv[]) {
    AsciiString input = {};

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
        io_getline(&input, stdin);

        if (*AsciiString_get(&input, 0) == '.') {
            switch (exec_meta_command(AsciiString_asstr(&input))) {
            case META_COMMAND_SUCCESS:
                continue;
            case META_COMMAND_UNRECOGNIZED_COMMAND:
                printf("Unrecognized command: %.*s\n", (int)input.len,
                       input.ptr);
                continue;
            }
        }

        AstParseResult result = Ast_parse(AsciiString_asstr(&input));

        printf("%s\n", result.type == AST_PARSE_OK ? "ok" : "err");
        if (result.type == AST_PARSE_OK)
            Ast_print(&result.ast.ok);

        // Statement statement;
        // switch (parse_statement(AsciiString_asstr(&input), &statement)) {
        // case PARSE_STATEMENT_SUCCESS:
        //     break;
        // case PARSE_STATEMTNT_UNRECOGNIZED_STATEMENT:
        //     printf("Failed to parse statement: %.*s\n", (int)input.len,
        //            input.ptr);
        //     continue;
        // }
        //
        // exec_statement(&statement);
    }

    return 0;
}
