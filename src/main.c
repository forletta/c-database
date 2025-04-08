#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *buffer;
    size_t length;
    size_t capacity;
} InputBuffer;

typedef enum {
    META_COMAND_SUCCESS,
    META_COMAND_UNRECOGNIZED_COMAND,
} MetaCommandResult;

void close_input_buffer(InputBuffer *input_buffer) {
    free(input_buffer->buffer);
}

void print_prompt() { printf("db > "); }

MetaCommandResult exec_meta_comand(InputBuffer *input_buffer) {
    if (strcmp(input_buffer->buffer, ".exit") == 0) {
        close_input_buffer(input_buffer);
        exit(EXIT_SUCCESS);
    }

    return META_COMAND_UNRECOGNIZED_COMAND;
}

void read_input(InputBuffer *input_buffer) {
    ssize_t bytes_read =
        getline(&input_buffer->buffer, &input_buffer->capacity, stdin);

    if (bytes_read < 0) {
        exit(EXIT_FAILURE);
    }

    // Ignore newline
    input_buffer->length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;
}

int main(int argc, char *argv[]) {
    InputBuffer input_buffer = {
        .buffer = NULL,
        .length = 0,
        .capacity = 0,
    };

    while (true) {
        print_prompt();
        read_input(&input_buffer);

        if (input_buffer.buffer[0] == '.') {
            switch (exec_meta_comand(&input_buffer)) {
            case META_COMAND_SUCCESS:
                continue;
            case META_COMAND_UNRECOGNIZED_COMAND:
                printf("Unrecognized comand: %s\n", input_buffer.buffer);
                continue;
            }
        }
    }

    return 0;
}
