#include "meta_command.h"
#include "ascii_string.h"
#include <stdlib.h>
#include <string.h>

MetaCommandResult exec_meta_command(AsciiStr input) {
    if (strncmp(input.ptr, ".exit", input.len) == 0) {
        exit(EXIT_SUCCESS);
    }

    return META_COMMAND_UNRECOGNIZED_COMMAND;
}
