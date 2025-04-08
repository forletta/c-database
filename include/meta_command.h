#ifndef META_COMMAND_H
#define META_COMMAND_H

#include "input.h"

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND,
} MetaCommandResult;

MetaCommandResult exec_meta_command(InputBuffer *input_buffer);

#endif // !META_COMMAND_H
