# KLI

Killian's (Command) Line Interface.

Multiplatform CLI library for the C programming language.

## Overview

### Features

This library allows **quick creation** of **command line interfaces** by declaring :
- Commands (e.g. `echo`, `cd`, `ls`)
- Positional arguments (e.g. `add <a> <b>`)
- Optional arguments (e.g. `time --hours <value> -m <value>`)

These are **statically allocated** using tables :
- Command tables can be nested to create **subcommands**.
- A command **handler** can be associated with an **argument** and / or an **option** table.

### Commands

Commands are defined by their `name` and a `description` :
- Both `name` and `description` are **null terminated** strings **without spaces**.
- Each entry points either to a **subcommand** table or a **function handler**.
- There is **no limit** on subcommand **nesting**.
- From the **top level** command table, commands follow a **tree structure** which will always ends on **handlers** at the **leafs** of the tree.

### Options and arguments

A command entry which points to a handler can also be associated with an **optional argument** table and a **positional argument** table :
- Optional arguments can be passed in **any order**, even in between positional arguments.
- Optional arguments can take zero or more arguments in a contiguous sequence, given the value of `argc` during declaration.
- Optional arguments are identified by :
    - A `shortName` which is a **single character** like `o`, or $0$ for no short name.
    - A `longName` which is a null terminated string **without spaces** like `option`, or `NULL` for no long name.
    - At least one of `shortName` or `longName` must be provided.
    - The option can then be called by using `-o` or `--option`.
    - A `description` which is a null terminated string used for help messages.
- Positional arguments are declared with a `name` and a `description`, for use in help messages.
- Positional arguments are assumed to follow the order of declaration of the table they're in.

### Builts-in

Two **built-in commands** are and always displayed with the top level command table :
- `help`, which print the top level command table.
- `clear`, which sends two **VT-100** codes to clear the screen and home the cursor. 

There is also the **help option**, built-in with all commands :
- This option can be called with `-h` or `--help` after any command.
- If the entry points to a subcommand table, this option will print this table.
- If the entry points to a handler, this option will print the options and arguments tables associated with the handler.

## API

### Tables

The following macros are used to declare tables. Documentation over the usage of these macros is directily included in the header files [kli_dispatch.h](./include/kli_dispatch.h) and [kli_optargs.h](./include/kli_optargs.h).

| Command table macros | Option table macros | Argument table macros |
| - | - | - |
| `KLI_BEGIN_COMMAND_TABLE` | `KLI_BEGIN_OPTION_TABLE` | `KLI_BEGIN_ARGUMENT_TABLE` |
| `KLI_ADD_SUBCOMMAND_TABLE` | `KLI_ADD_OPTION` | `KLI_ADD_ARGUMENT` |
| `KLI_ADD_COMMAND_HANDLER` | `KLI_END_OPTION_TABLE` | `KLI_END_ARGUMENT_TABLE` |
| `KLI_END_COMMAND_TABLE` |
| `KLI_EXPORT_COMMAND_TABLE` |

### Handlers

The `KLI_ADD_COMMAND_HANDLER` macro takes as final argument a function pointer of signature `const char *handler(void)` :
- Options and arguments aren't directly passed to handlers. Instead, the handler can query both through two functions `kli_get_arg` and `kli_get_opt`.
- Then the handler returns `NULL` if the operation was successful, or an error message otherwise.

### Functions

Once the program is compiled, a set of functions will handle the command routing and the argument parsing :
- [kli_dispatch.h](./include/kli_dispatch.h)
    - `kli_parse_line` which tokenize an input string into `argc` and `argv`.
    - `kli_dispatch` which route `argc` and `argv` through the previously defined **command**, **argument** and **option** tables.
- [kli_optargs.h](./include/kli_optargs.h)
    - `kli_get_opt`, return `true` if the option at the given `index` of the option table was found, and write the value in `argv`.
    - `kli_get_arg`, return `true` if the argument at the given `index` of the argument table was found, and write the value in `optv`.
- [kli_print.h](./include/kli_print.h)
    - `kli_print` which format and add text to the output buffer.
    - `kli_flush` which flush the whole output buffer to the standard output, to be implemented by `kli_out` with the signature defined in [kli_platform.h](./include/kli_platform.h).
- [kli_parse.h](./include/kli_parse.h)
    - `kli_parse_long` can be used to parse integer values from strings.
    - `kli_parse_float` can be used to parse floating point values from strings.

## Example

### Implementation example

Here is a full example with the following commands :
- `echo <echo options (1)>`
- `math add <math arguments (2)>`
- `math sub <math arguments (2)>`
- `math mul <math arguments (2)>`
- `math div <math arguments (2)>`
- `todo`

```c
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "kli.h"

// Declare argument table for math command

KLI_BEGIN_ARGUMENT_TABLE(MATH_ARGUMENTS)
KLI_ADD_ARGUMENT("a", "First operand")
KLI_ADD_ARGUMENT("b", "Second operand")
KLI_END_ARGUMENT_TABLE

// Declare math commands handlers

static const char *math_add_handler(void);
static const char *math_sub_handler(void);
static const char *math_mul_handler(void);
static const char *math_div_handler(void);

// Declare math commands table

KLI_BEGIN_COMMAND_TABLE(MATH_COMMANDS)
KLI_ADD_SUBCOMMAND_TABLE("add", "Compute a + b", NULL, MATH_ARGUMENTS, math_add_handler)
KLI_ADD_SUBCOMMAND_TABLE("sub", "Compute a - b", NULL, MATH_ARGUMENTS, math_sub_handler)
KLI_ADD_SUBCOMMAND_TABLE("add", "Compute a * b", NULL, MATH_ARGUMENTS, math_mul_handler)
KLI_ADD_SUBCOMMAND_TABLE("add", "Compute a / b", NULL, MATH_ARGUMENTS, math_div_handler)
KLI_END_COMMAND_TABLE

// Decalre echo options table

KLI_BEGIN_OPTION_TABLE(ECHO_OPTION)
KLI_ADD_OPTION('s', "string", 1, "String to echo back")
KLI_END_OPTION_TABLE

// Declare echo handler

static const char *echo_handler(void);

// Declare top level commands table

KLI_BEGIN_COMMAND_TABLE(COMMANDS)
KLI_ADD_SUBCOMMAND_TABLE("echo", "Exemple handler.", ECHO_OPTION, NULL, echo_handler)
KLI_ADD_COMMAND_SUBTABLE("math", "Motor control and display commands.", MATH_COMMANDS)
KLI_ADD_SUBCOMMAND_TABLE("todo", "Not implemented yet.", NULL, NULL, NULL)
KLI_END_COMMAND_TABLE

// Program entry point

void main(void) {
    
    // Show command input
    static char buffer[256] = {0};
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

// Set standard output for KLI

void kli_out(const char *string) {
    printf(string);
}

// Math argument parser used in math handlers

static const char *math_parse_operands(float *a, float *b) {

    // Get arguments
    char *aString = NULL, *bString = NULL;
    if(!kli_get_arg(0, &aString) || !kli_get_arg(0, &bString))
        return "Missing operands";
    
    // Parse arguments
    if(!kli_parse_float(aString, a) || !kli_parse_float(bString, b))
        return "Failed to parse operands";

    // Parse sucess
    return NULL;
}

// Math commands handler implementations

static const char *math_add_handler(void) {
    float a = 0, b = 0;
    const char *error = math_parse_operands(&a, &b);
    if(error)
        return error;
    else
        kli_print("%3.f", a + b);
}

static const char *math_sub_handler(void) {
    float a = 0, b = 0;
    const char *error = math_parse_operands(&a, &b);
    if(error)
        return error;
    else
        kli_print("%3.f", a - b);
}

static const char *math_mul_handler(void) {
    float a = 0, b = 0;
    const char *error = math_parse_operands(&a, &b);
    if(error)
        return error;
    else
        kli_print("%3.f", a * b);
}

static const char *math_div_handler(void) {
    float a = 0, b = 0;
    const char *error = math_parse_operands(&a, &b);
    if(error)
        return error;
    else if(b == 0)
        return "Cannot divide by 0";
    else
        kli_print("%3.f", a / b);
}

// Echo command handler implementation

static const char *echo_handler(void) {
    char *string = NULL;
    if(kli_get_opt(0, &string))
        kli_print("echo %s", string);
    else
        kli_print("echo");
}
```

### Output

Output of the previous implementation look as follows :
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

        math add <argument(s)> - Compute a + b
        math sub <argument(s)> - Compute a - b
        math add <argument(s)> - Compute a * b
        math add <argument(s)> - Compute a / b

>
```

## Credits

- Copyright (c) 2025, Killian Baillifard, Licensed under the MIT License. See the LICENSE file in the project root for full license information.
