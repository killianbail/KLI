# KLI

Killian's (Command) Line Interface.

Multiplatform CLI library for the C programming language

## Builts-in

Two commands are built in and always displayed with the top level command table :
- `help`
- `clear`

Furthermore, all command support by default option `-h` or `--help`.

## Output exemple

Output look as follows :
```
> help

    [COMMAND(S)]

        help                 - Show commands. Use <command> -h or --help to show (sub)command help.
        clear                - Clear screen and home cursor through VT100 codes.
        echo <option(s)>     - Exemple handler.
        math <subcommand(s)> - Motor control and display commands.
        todo                 - Not implemented yet.

> todo

    'todo' - Not implemented.

> math --help

    [COMMAND]

        'math' - Motor control and display commands.

    [SUBCOMMAND(S)]

        math add <argument(s)> - Compute x + y
        math sub <argument(s)> - Compute x - y
        math add <argument(s)> - Compute x * y
        math add <argument(s)> - Compute x / y

>
```

## API exemple

Here is an exemple implementation with a few commands.

```c
/**
 * @file cli.c
 * @author Killian Baillifard
 * @date 29.09.2025
 * @brief Command Line Interface (CLI) : wait, decode and apply commands.
 */

// Includes

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "kli.h"

// Commands declaration

KLI_BEGIN_ARGUMENT_TABLE(MATH_ARGUMENTS)
KLI_ADD_ARGUMENT("x", "First operand")
KLI_ADD_ARGUMENT("y", "First operand")
KLI_END_ARGUMENT_TABLE

static const char *math_add_handler(void);
static const char *math_sub_handler(void);
static const char *math_mul_handler(void);
static const char *math_div_handler(void);

KLI_BEGIN_COMMAND_TABLE(MATH_COMMANDS)
KLI_ADD_COMMAND_HANDLER("add", "Compute x + y", NULL, MATH_ARGUMENTS, math_add_handler)
KLI_ADD_COMMAND_HANDLER("sub", "Compute x - y", NULL, MATH_ARGUMENTS, math_sub_handler)
KLI_ADD_COMMAND_HANDLER("add", "Compute x * y", NULL, MATH_ARGUMENTS, math_mul_handler)
KLI_ADD_COMMAND_HANDLER("add", "Compute x / y", NULL, MATH_ARGUMENTS, math_div_handler)
KLI_END_COMMAND_TABLE

KLI_BEGIN_OPTION_TABLE(ECHO_OPTION)
KLI_ADD_OPTION('s', "string", 1, "String to echo back")
KLI_END_OPTION_TABLE

static const char *echo_handler(void);

KLI_BEGIN_COMMAND_TABLE(COMMANDS)
KLI_ADD_COMMAND_HANDLER("echo", "Exemple handler.", ECHO_OPTION, NULL, echo_handler)
KLI_ADD_COMMAND_SUBTABLE("math", "Motor control and display commands.", MATH_COMMANDS)
KLI_ADD_COMMAND_HANDLER("todo", "Not implemented yet.", NULL, NULL, NULL)
KLI_END_COMMAND_TABLE

// Program entry point

void main(void) {

    static char buffer[256] = {0};
    
    // Show command input
    kli_print("\n > ");
    kli_flush();

    while(true) {

        // Wait for new command
        memset(buffer, 0, 256);
        fflush(stdin);
        int count = scanf("%255[^\n]", buffer);
        kli_print("\n");

        // Parse and execute command
        char *argv[KLI_MAX_ARGC] = {NULL};
        int argc = kli_parse_line(buffer, argv);
        kli_dispatch(COMMANDS, argc, argv);

        // Show command input
        kli_print("\n > ");
        kli_flush();
    }
}

// Platform implementation

void kli_out(const char *string) {
    printf(string);
}

// Math parsers and handlers 

static const char *math_parse_operands(float *x, float *y) {

    // Get arguments
    char *xString = NULL, *yString = NULL;
    if(!kli_get_arg(0, &xString) || !kli_get_arg(0, &yString))
        return "Missing operands";
    
    // Parse arguments
    if(!kli_parse_float(xString, x) || !kli_parse_float(yString, y))
        return "Failed to parse operands";

    // Parse sucess
    return NULL;
}

static const char *math_add_handler(void) {
    float x = 0, y = 0;
    const char *error = math_parse_operands(&x, &y);
    if(error)
        return error;
    else
        kli_print("%3.f", x + y);
}

static const char *math_sub_handler(void) {
    float x = 0, y = 0;
    const char *error = math_parse_operands(&x, &y);
    if(error)
        return error;
    else
        kli_print("%3.f", x - y);
}

static const char *math_mul_handler(void) {
    float x = 0, y = 0;
    const char *error = math_parse_operands(&x, &y);
    if(error)
        return error;
    else
        kli_print("%3.f", x * y);
}

static const char *math_div_handler(void) {
    float x = 0, y = 0;
    const char *error = math_parse_operands(&x, &y);
    if(error)
        return error;
    else if(y == 0)
        return "Cannot divide by 0";
    else
        kli_print("%3.f", x / y);
}

// Math handler

static const char *echo_handler(void) {
    char *string = NULL;
    if(kli_get_opt(0, &string))
        kli_print("echo %s", string);
    else
        kli_print("echo");
}
```
