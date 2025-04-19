#ifndef VALUE_H
#define VALUE_H

#include "array.h"
#include "char_array.h"
#include "token.h"
#include <stddef.h>
#include <stdint.h>

#define VALUE_TYPES_LEN 2
static char *VALUE_TYPES[VALUE_TYPES_LEN] = {
    "VALUE_TYPE_CHAR",
    "VALUE_TYPE_INT",
};

typedef struct {
    enum {
        VALUE_TYPE_CHAR,
        VALUE_TYPE_INT,
    } type;
    union {
        size_t capacity;
    } metadata;
} ValueType;

typedef struct {
    ValueType type;
    union {
        int32_t int_type;
        charArray char_type;
    } data;
} Value;

ARRAY(Value);

typedef struct {
    enum {
        VALUE_PARSE_ERR,
        VALUE_PARSE_OK,
    } err;
    union {
        struct {
        } err;
        Value ok;
    } value;
} ValueParseResult;

ValueParseResult Value_parse(Token *token);
ValueParseResult Value_parse_num(charArray *num);
ValueParseResult Value_parse_str(charArray *str);

void Value_print(Value *value);
void ValueType_print(ValueType value_type);

#endif // !VALUE_H
