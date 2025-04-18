// #ifndef COMMAND_H
// #define COMMAND_H
//
// #include "ascii_string.h"
// #include "ast.h"
//
// typedef struct {
//     AsciiString table;
//     AsciiStringVector fields;
// } CommandSelect;
//
// typedef struct {
//     StatementType type;
//     union {
//         CommandSelect select;
//         // CommandInsert insert;
//     } command;
// } Command;
//
// typedef struct {
//     Command *ptr;
//     size_t len;
//     size_t cap;
// } CommandVector;
//
// typedef enum {
//     COMMAND_PARSE_ERR,
//     COMMAND_PARSE_OK,
// } CommandParseResult;
//
// // CommandVector:
//
// Command *CommandVector_get(CommandVector *v, size_t i);
// void CommandVector_push(CommandVector *v, Command *command);
// void CommandVector_free(CommandVector *v);
//
// // Command:
//
// CommandParseResult Command_parse(CommandVector *commands, Ast *ast);
// CommandParseResult CommandSelect_parse(CommandVector *commands, StatementSelect *statement);
//
// // Printing:
//
// void Command_print(Command *command);
// void CommandSelect_print(CommandSelect *command);
//
// #endif // !COMMAND_H
