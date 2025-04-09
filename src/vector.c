#include "vector.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void vector_ensure_available_capacity(Vector *vector, size_t requested_capacity,
                                      size_t element_size) {
    size_t required_size = vector->len + (requested_capacity * element_size);
    size_t new_capacity;

    if (required_size <= vector->capacity) {
        return;
    }

    if (vector->capacity * 2 < required_size) {
        new_capacity = required_size;
    } else {
        new_capacity = vector->capacity * 2;
    }

    uint8_t *new_ptr = malloc(new_capacity);

    for (int i = 0; i < vector->len; i++) {
        new_ptr[i] = vector->ptr[i];
    }

    free(vector->ptr);

    vector->ptr = new_ptr;
    vector->capacity = new_capacity;
}

void vector_push(Vector *vector, void *item, size_t element_size) {
    vector_ensure_available_capacity(vector, 1, element_size);

    for (int i = 0; i < element_size; i++) {
        vector->ptr[vector->len + i] = ((uint8_t *)item)[i];
    }

    vector->len += element_size;
}
