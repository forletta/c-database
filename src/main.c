#include "ast.h"
#include "command.h"
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

        printf("\n");

        AstParseResult ast_parse_result = Ast_parse(&input);

        if (ast_parse_result.type != AST_PARSE_OK) {
            printf("Failed to parse token stream: %.*s\n", (int)input.len,
                   input.ptr);
            continue;
        }

        Ast ast = ast_parse_result.ast.ok;

        Ast_print(&ast);

        printf("\n");

        CommandArray commands = {};

        if (Command_parse(&commands, &ast) == COMMAND_PARSE_ERR)
            printf("Failed to parse ast: %.*s\n", (int)input.len, input.ptr);

        for (size_t i = 0; i < commands.len; i++) {
            Command *command = CommandArray_get(&commands, i);
            Command_print(command);
            printf("\n");
        }

        printf("\n");
    }

    return 0;
}
