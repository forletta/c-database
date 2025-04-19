#include "command.h"
#include "io.h"
#include "meta_command.h"
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

        CommandArray commands = {};

        if (!command_parse_input(&commands, &input)) {
            printf("Failed to parse input\n");
            continue;
        }

        for (size_t i = 0; i < commands.len; i++) {
            Command *command = CommandArray_get(&commands, i);
            Command_print(command);
            printf("\n");
        }
    }

    return 0;
}
