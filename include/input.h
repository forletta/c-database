#ifndef INPUT_H
#define INPUT_H

#include <stddef.h>

typedef struct {
    char *buffer;
    size_t length;
    size_t capacity;
} InputBuffer;

void close_input_buffer(InputBuffer *input_buffer);

void read_input(InputBuffer *input_buffer);


#endif // !INPUT_H
