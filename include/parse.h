#ifndef PARSE_H
#define PARSE_H

#define TRY_PARSE(expr)                                                        \
    if (!expr)                                                                 \
        return PARSE_ERR;

typedef enum {
    PARSE_ERR,
    PARSE_OK,
} ParseResult;

#endif // !PARSE_H
