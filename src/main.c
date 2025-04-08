#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *buffer;
    size_t length;
    size_t capacity;
} InputBuffer;

void close_input_buffer(InputBuffer *input_buffer) {
    free(input_buffer->buffer);
}

void print_prompt() {
    printf("db > ");
}

void read_input(InputBuffer *input_buffer) {
    ssize_t bytes_read = getline(&input_buffer->buffer, &input_buffer->capacity, stdin);

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

        if (strcmp(input_buffer.buffer, ".exit") == 0) {
            close_input_buffer(&input_buffer);
            exit(EXIT_SUCCESS);
        } else {
            printf("Unrecognized comand: %s\n", input_buffer.buffer);
        }
    }

    return 0;
}
