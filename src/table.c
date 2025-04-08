#include "table.h"
#include <stdio.h>

void print_schema(TableSchema *schema) {
    printf("TableSchema { .fieldc = %zu, .field_names = [", schema->fieldc);

    if (schema->fieldc > 0) {
        for (size_t i = 0; i < schema->fieldc; i++) {
            printf("\"%s\"%s", schema->field_names[i],
                   i != schema->fieldc - 1 ? ", " : "");
        }
    }

    printf("] .field_data_types = [");

    if (schema->fieldc > 0) {
        for (size_t i = 0; i < schema->fieldc; i++) {
            FieldDataType field_data_type = schema->field_data_types[i];
            switch (field_data_type.type) {
            case FIELD_DATA_TYPE_INT:
                printf("INT");
                break;
            case FIELD_DATA_TYPE_CHAR:
                printf("CHAR(%zu)",
                       field_data_type.metadata.char_type.capacity);
                break;
            }
            printf("%s", i != schema->fieldc - 1 ? ", " : "");
        }
    }

    printf("] }");
}
