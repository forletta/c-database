#include "io.h"
#include "meta_command.h"
#include "token.h"
#include <stdio.h>

void print_prompt() { printf("db > "); }

int main(int argc, char *argv[]) {
    charArray input = {};

    while (true) {
        print_prompt();
        io_getline(&input, stdin);

        if (*charArray_get(&input, 0) == '.') {
            switch (exec_meta_command(input)) {
            case META_COMMAND_SUCCESS:
                continue;
            case META_COMMAND_UNRECOGNIZED_COMMAND:
                printf("Unrecognized command: %.*s\n", (int)input.len,
                       input.ptr);
                continue;
            }
        }

        TokenArray stream = {};

        if (token_parse(&stream, &input) == TOKENIZE_FAILURE)
            printf("Failed to tokenize input: %.*s\n", (int)input.len,
                   input.ptr);

        TokenArrayIter stream_iter = TokenArrayIter_create(&stream);
        Token *t;
        while ((t = TokenArrayIter_next(&stream_iter)) != NULL) {
            Token_print(t);
            printf("\n");
        }

        // AstParseResult ast_parse_result =
        // Ast_parse(AsciiString_asstr(&input));
        //
        // if (ast_parse_result.type != AST_PARSE_OK) {
        //     printf("Failed to parse statement: %.*s\n", (int)input.len,
        //            input.ptr);
        //     continue;
        // }
        //
        // Ast ast = ast_parse_result.ast.ok;
        //
        // Ast_print(&ast);
        //
        // printf("\n");
        //
        // CommandVector commands = {};
        //
        // Command_parse(&commands, &ast);
        //
        // for (size_t i = 0; i < commands.len; i++) {
        //     Command *command = CommandVector_get(&commands, i);
        //     Command_print(command);
        //     printf("\n");
        // }

        // printf("%s\n", result.type == AST_PARSE_OK ? "ok" : "err");
        // if (result.type == AST_PARSE_OK)
        //     Ast_print(&result.ast.ok);

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
