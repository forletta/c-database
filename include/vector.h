#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t *ptr;
    size_t len;
    size_t capacity;
} Vector;

void vector_push(Vector *vector, void *item, size_t size);

#endif // !VECTOR_H
