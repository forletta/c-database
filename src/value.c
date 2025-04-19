#include "value.h"
#include <stdio.h>
#include <stdlib.h>

ARRAY_IMPL(Value);

ValueParseResult Value_parse(Token *token) {
    switch (token->type) {
    case TOKEN_TYPE_NUM:
        return Value_parse_num(&token->slice);
    case TOKEN_TYPE_STR:
        return Value_parse_str(&token->slice);
    default:
        break;
    }

    return (ValueParseResult){};
}

ValueParseResult Value_parse_num(charArray *str) {
    int32_t num = 0;

    for (size_t i = 0; i < str->len; i++) {
        num = num * 10 + (*charArray_get(str, i) - 0x30);
    }

    return (ValueParseResult){
        .err = VALUE_PARSE_OK,
        .value.ok.type =
            {
                .type = VALUE_TYPE_INT,
            },
        .value.ok.data.int_type = num,
    };
}

ValueParseResult Value_parse_str(charArray *str) {
    charArray parsed = {};
    charArray_reserve(&parsed, str->len);

    charArrayIter iter = charArrayIter_create(str);

    char *c;

    if ((c = charArrayIter_next(&iter)) == NULL)
        return (ValueParseResult){};

    switch (*c) {
    case '"':
    case '\'':
        break;
    default:
        return (ValueParseResult){};
    }

    char quote_type = *c;
    bool escaped = false;

    while ((c = charArrayIter_next(&iter)) != NULL) {
        if (escaped) {
            switch (*c) {
            case '\'':
            case '"':
            case '\\':
                escaped = false;
                charArray_push(&parsed, c);
                continue;
            default:
                return (ValueParseResult){};
            }
        }

        if (*c == '\\') {
            escaped = true;
            continue;
        }

        if (*c == quote_type)
            return (ValueParseResult){
                .err = VALUE_PARSE_OK,
                .value.ok.type =
                    {
                        .type = VALUE_TYPE_CHAR,
                        .metadata.capacity = 0,
                    },
                .value.ok.data.char_type = parsed,
            };

        charArray_push(&parsed, c);
        continue;
    }

    return (ValueParseResult){};
}

void Value_print(Value *value) {
    printf("Value {type = ");

    ValueType_print(value->type);
    printf(", data = ");

    switch (value->type.type) {
    case VALUE_TYPE_CHAR:
        printf("\"%.*s\"", (int)value->data.char_type.len,
               value->data.char_type.ptr);
        break;
    case VALUE_TYPE_INT:
        printf("%d", value->data.int_type);
        break;
    }

    printf("}");
}

void ValueType_print(ValueType value_type) {
    printf("ValueType {type = %s", VALUE_TYPES[value_type.type]);

    if (value_type.type == VALUE_TYPE_CHAR) {
        printf(", metadata.capacity = %zu", value_type.metadata.capacity);
    }

    printf("}");
}
