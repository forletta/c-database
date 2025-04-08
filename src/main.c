#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *buffer;
    size_t length;
    size_t capacity;
} InputBuffer;

typedef enum {
    STATEMENT_TYPE_INSERT,
    STATEMENT_TYPE_SELECT,
} StatementType;

typedef struct {
    StatementType type;
} Statement;

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND,
} MetaCommandResult;

typedef enum {
    PARSE_STATEMENT_SUCCESS,
    PARSE_STATEMTNT_UNRECOGNIZED_STATEMENT,
} ParseStatementResult;

typedef struct {
    enum {
        FIELD_DATA_TYPE_INT,
        FIELD_DATA_TYPE_CHAR,
    } type;
    union {
        struct {
        } int_type;
        struct {
            size_t capacity;
        } char_type;
    } metadata;

} FieldDataType;

typedef struct {
    size_t fieldc;
    char **field_names;
    FieldDataType *field_data_types;
} TableSchema;

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

void close_input_buffer(InputBuffer *input_buffer) {
    free(input_buffer->buffer);
}

void print_prompt() { printf("db > "); }

MetaCommandResult exec_meta_command(InputBuffer *input_buffer) {
    if (strcmp(input_buffer->buffer, ".exit") == 0) {
        close_input_buffer(input_buffer);
        exit(EXIT_SUCCESS);
    }

    return META_COMMAND_UNRECOGNIZED_COMMAND;
}

ParseStatementResult parse_statement(InputBuffer *input_buffer,
                                     Statement *statement) {
    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_TYPE_INSERT;
        return PARSE_STATEMENT_SUCCESS;
    }

    if (strncmp(input_buffer->buffer, "select", 6) == 0) {
        statement->type = STATEMENT_TYPE_SELECT;
        return PARSE_STATEMENT_SUCCESS;
    }

    return PARSE_STATEMTNT_UNRECOGNIZED_STATEMENT;
}

void exec_statement(Statement *statement) {
    switch (statement->type) {
    case STATEMENT_TYPE_INSERT:
        printf("exec insert\n");
        break;
    case STATEMENT_TYPE_SELECT:
        printf("exec select\n");
        break;
    }
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

int main(int argc, char *argv[]) {
    InputBuffer input_buffer = {
        .buffer = NULL,
        .length = 0,
        .capacity = 0,
    };

    size_t fieldc = 2;
    char *field_names[2] = {"Name", "Age"};
    FieldDataType field_data_types[2] = {
        {.type = FIELD_DATA_TYPE_CHAR,
         .metadata = {.char_type =
                          {
                              .capacity = 20,
                          }}},
        {.type = FIELD_DATA_TYPE_INT, .metadata = {}}};
    TableSchema example_schema = {
        .fieldc = fieldc,
        .field_names = field_names,
        .field_data_types = field_data_types,
    };

    print_schema(&example_schema);
    printf("\n");

    while (true) {
        print_prompt();
        read_input(&input_buffer);

        if (input_buffer.buffer[0] == '.') {
            switch (exec_meta_command(&input_buffer)) {
            case META_COMMAND_SUCCESS:
                continue;
            case META_COMMAND_UNRECOGNIZED_COMMAND:
                printf("Unrecognized command: %s\n", input_buffer.buffer);
                continue;
            }
        }

        Statement statement;
        switch (parse_statement(&input_buffer, &statement)) {
        case PARSE_STATEMENT_SUCCESS:
            break;
        case PARSE_STATEMTNT_UNRECOGNIZED_STATEMENT:
            printf("Failed to parse statement: %s\n", input_buffer.buffer);
            continue;
        }

        exec_statement(&statement);
    }

    return 0;
}
