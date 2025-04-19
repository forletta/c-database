// #ifndef TABLE_H
// #define TABLE_H
//
// #include "char_array.h"
// #include <stddef.h>
//
// typedef struct {
//     enum {
//         FIELD_DATA_TYPE_INT,
//         FIELD_DATA_TYPE_CHAR,
//     } type;
//     union {
//         struct {
//         } int_type;
//         struct {
//             size_t capacity;
//         } char_type;
//     } metadata;
// } FieldDataType;
//
// typedef struct {
//     FieldDataType data_type;
//     union {
//         int int_type;
//         charArray char_type;
//     } data;
// } Field;
//
// typedef struct {
//     size_t fieldc;
//     char **field_names;
//     FieldDataType *field_data_types;
// } TableSchema;
//
// void print_schema(TableSchema *schema);
//
// #endif // !TABLE_H
