#include "input.h"
#include <stdio.h>
#include <stdlib.h>


void close_input_buffer(InputBuffer *input_buffer) {
    free(input_buffer->buffer);
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
