/**
 * @file kli_dispatch.c
 * @author Killian Baillifard
 * @date 23.10.2025
 * @brief KLI command dispatch implementation.
 */

// Includes

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "kli_dispatch.h"
#include "kli_config.h"
#include "kli_print.h"

// Static prototypes

static void reset_caches(void);
static void call_handler(const KliCommand * const entry, int argc, char **argv);
static bool find_entry(const KliCommand table[], int argc, char **argv);
static int get_table_padding(const KliCommand table[]);
static int get_optargs_padding(const KliCommand * const entry);
static void print_table(const KliCommand table[], int padding);
static void print_entry(const KliCommand * const entry);

// Built in commands

static const char *help_handler(void);
static const char *clear_handler(void);

KLI_BEGIN_COMMAND_TABLE(KLI_BUILTIN)
KLI_ADD_COMMAND_HANDLER("help", "Show commands. Use <command> -h or --help to show (sub)command help.", NULL, NULL, help_handler)
KLI_ADD_COMMAND_HANDLER("clear", "Clear screen and home cursor through VT100 codes.", NULL, NULL, clear_handler)
KLI_END_COMMAND_TABLE

// Static constants

static const char COMMAND_BEACON[]      = "[COMMAND]";
static const char COMMANDS_BEACON[]     = "[COMMAND(S)]";
static const char SUBCOMMANDS_BEACON[]  = "[SUBCOMMAND(S)]";
static const char USAGE_BEACON[]        = "[USAGE]";
static const char OPTIONS_BEACON[]      = "[OPTION(S)]";
static const char ARGUMENTS_BEACON[]    = "[ARGUMENT(S)]";
static const char SUBCOMMANDS_TAG[]     = "<subcommand(s)>";
static const char OPTIONS_TAG[]         = "<option(s)>";
static const char ARGUMENTS_TAG[]       = "<argument(s)>";

// Static variables

static const KliCommand * topTable      = NULL;
static char command[KLI_MAX_LINE_SIZE]  = {0};
static size_t commandEndIndex           = 0;

// Implementations

void kli_dispatch(const KliCommand table[], int argc, char **argv) {
    
    // No argument(s) -> early return
    if(!argc)
        goto KLI_DISPATCH_END;

    // Lookup for entry in builts-in, if found -> early return
    topTable = table;
    if(find_entry(KLI_BUILTIN, argc, argv))
        goto KLI_DISPATCH_END;

    // Lookup for entry in commands, if found -> early return
    if(find_entry(table, argc, argv))
        goto KLI_DISPATCH_END;

    // Unknown command, display help
    kli_print("\t'%s' - unknown command.\n\n", argv[0]);
    help_handler();
KLI_DISPATCH_END:
    reset_caches();
}

// Static definitions

static void reset_caches(void) {
    topTable = NULL;
    memset(command, 0, KLI_MAX_LINE_SIZE);
    commandEndIndex = 0;
}

static bool find_entry(const KliCommand table[], int argc, char **argv) {

    // Check all entries
    for(const KliCommand *entry = table; entry->name; entry++) {

        // Name does not match -> check next entry
        if(strcmp(argv[0], entry->name))
            continue;
        
        // Name found -> next argument
        --argc;
        argv = &argv[1];

        // Save parsed name to decoded command
        if(commandEndIndex)
            command[commandEndIndex++] = ' ';
        strcpy(&command[commandEndIndex], entry->name);
        commandEndIndex += strlen(entry->name);

        // Check if command has subcommand or handler
        bool haveSubcommands = entry->subcommands != NULL;
        bool haveHandler = entry->handler != NULL;
        bool isImplemented = haveSubcommands || haveHandler;

        // User asked help, display subcommand help
        if(isImplemented && argc >= 1 && (!strcmp(argv[0], "-h") || !strcmp(argv[0], "--help")))
            print_entry(entry);

        // Entry has subcommands -> find handler recusively
        else if(haveSubcommands) {

            // Missing argument for subcommands
            if(!argc) {
                kli_print("\t'%s' - expected subcommand.\n\n", command);
                print_entry(entry);
            }

            // Subcommand not found -> print help
            else if(!find_entry(entry->subcommands, argc, argv)) {
                kli_print("\t'%s %s' - unknown subcommand.\n\n", command, argv[0]);
                print_entry(entry);
            }
        }

        // Entry has a handler -> call it with arguments
        else if(haveHandler)
            call_handler(entry, argc, argv);

        // No subcommands or handler -> not implemented
        else
            kli_print("\t'%s' - Not implemented.\n", command);

        // Entry found
        return true;
    }

    // Entry not found
    return false;
}

static void call_handler(const KliCommand * const entry, int argc, char **argv) {

    // Parse options and arguments
    bool parsed = kli_optargs(entry->options, entry->arguments, argc, argv);

    // Parsing failed
    if(!parsed)
        print_entry(entry);


    // Call handler
    const char *error = NULL;
    if(parsed)
        error = entry->handler();

    // Failed parsing or handling -> show help
    if(error) {
        kli_print("\t'%s' - %s\n\n", command, error);
        print_entry(entry);
    }
}

static int get_optargs_padding(const KliCommand * const entry) {

    // Initialize alignement padding
    int padding = 0;
    for(const KliOption *option = entry->options; option && option->longName; option++) {
        
        // Add single dash length with short option length
        int length = 0;
        if(option->shortName)
            length += 2;

        // Add space length between short and long option name
        if(option->shortName && option->longName)
            length += 1;

        // Add double dash length with long name length
        if(option->longName)
            length += 2 + strlen(option->longName);

        // Add space length between long name and option argument tag
        if(option->longName && option->argc)
            length += 1;

        // Add option argument tag length, minus end char plus first digit plus space
        if(option->argc) {
            length += sizeof(ARGUMENTS_TAG) + 1;

            // Add additionnal digits length
            for(int x = option->argc; x >= 10; x /= 10)
                ++length;
        }

        // Update padding if this option is longest
        padding = length > padding ? length : padding;
    }

    for(const KliArgument *argument = entry->arguments; argument && argument->name; argument++) {
        
        // Add argument length padding
        int length = strlen(argument->name);

        // Update padding if this argument is longest
        padding = length > padding ? length : padding;
    }

    // Return longest computed padding
    return padding;
}

static int get_table_padding(const KliCommand table[]) {

    // Initialize alignement padding
    int padding = 0;
    for(const KliCommand *entry = table; entry->name; entry++) {

        // Add command name length
        int length = strlen(entry->name);

        // If entry has subcommand, add space plus subcommand tag length
        if(entry->subcommands)
            length += sizeof(SUBCOMMANDS_TAG);

        // If entry has optargs
        else {

            // Add space before any tags
            if(entry->options || entry->arguments)
                length += 1;

            // Add option tag length
            if(entry->options)
                length += sizeof(OPTIONS_TAG) - 1;

            // Add space between option and argument tag length
            if(entry->options && entry->arguments)
                length += 1;

            // Add argument tag length
            if(entry->arguments)
                length += sizeof(ARGUMENTS_TAG) - 1;
        }

        // Update padding if this entry is longest
        padding = length > padding ? length : padding;
    }

    // Return longest computed padding
    return padding;
}

static void print_table(const KliCommand table[], int padding) {

    // Print commands
    for(const KliCommand *entry = table; entry->name; entry++) {
        char buffer[KLI_MAX_PADDING] = {0};
        kli_print("\t\t");

        // Print already parsed command tokens
        if(commandEndIndex)
            kli_print("%s ", command);

        // Concatenate name
        strcat(buffer, entry->name);

        // Concatenate subcommand tag
        if(entry->subcommands) {
            strcat(buffer, " ");
            strcat(buffer, SUBCOMMANDS_TAG);
        }

        // Concatenate optargs tags
        else {
            if(entry->options) {
                strcat(buffer, " ");
                strcat(buffer, OPTIONS_TAG);
            }
            if(entry->arguments) {
                strcat(buffer, " ");
                strcat(buffer, ARGUMENTS_TAG);
            }
        }

        // Print concatenated fields with padded description
        kli_print("%-*s - %s\n", padding, buffer, entry->description);
    }
}

static void print_entry(const KliCommand * const entry) {

    // Print command name and description
    kli_print("\t%s\n\n", COMMAND_BEACON);
    kli_print("\t\t'%s' - %s\n", command, entry->description);

    // Entry has subcommands -> print subcommands
    if(entry->subcommands) {
        kli_print("\n\t%s\n\n", SUBCOMMANDS_BEACON);
        print_table(entry->subcommands, get_table_padding(entry->subcommands));
        return;
    }

    // Entry has no handler -> not implemented
    if(!entry->handler) {
        kli_print("\t'%s' - Not implemented\n\n", command);
        return;
    }

    // Print usage
    kli_print("\n\t%s\n\n", USAGE_BEACON);
    kli_print("\t\t%s", command);
    if(entry->options)
        kli_print(" %s", OPTIONS_TAG);
    if(entry->arguments)
        kli_print(" %s", ARGUMENTS_TAG);
    kli_print("\n");

    // Find optargs padding
    int padding = get_optargs_padding(entry);

    // Entry has options -> print options
    if(entry->options) {
        kli_print("\n\t%s\n\n", OPTIONS_BEACON);
        for(const KliOption *option = entry->options; option->shortName || option->longName; option++) {
            char buffer[KLI_MAX_PADDING] = {0};
            kli_print("\t\t");

            // Concatenate short name
            if(option->shortName) {
                buffer[0] = '-';
                buffer[1] = option->shortName;
            }

            // Concatenate space between short and long names
            if(option->shortName && option->longName)
                buffer[2] = ' ';

            // Concatenate long name
            if(option->longName) {
                strcat(buffer, "--");
                strcat(buffer, option->longName);
            }

            // Concatenate option arguments
            if(option->argc) {
                char digits[KLI_INT32_MAX_DIGITS] = {0};
                snprintf(digits, KLI_INT32_MAX_DIGITS, "%d", option->argc);
                strcat(buffer, " <");
                strcat(buffer, digits);
                strcat(buffer, " ");
                strcat(buffer, &ARGUMENTS_TAG[1]);
            }
                
            // Print option fields with concatenated description
            kli_print("%-*s - %s\n", padding, buffer, option->description);
        }
    }

    // Entry has arguments -> print arguments
    if(entry->arguments) {
        kli_print("\n\t%s\n\n", ARGUMENTS_BEACON);
        for(const KliArgument *argument = entry->arguments; argument->name; argument++)
            kli_print("\t\t%-*s - %s\n", padding, argument->name, argument->description);
    }
}

static const char *help_handler(void) {
    const int BUILTIN_PADDING = get_table_padding(KLI_BUILTIN);
    const int TOP_TABLE_PADDING = get_table_padding(topTable);
    const int MAX_PADDING = TOP_TABLE_PADDING > BUILTIN_PADDING ? TOP_TABLE_PADDING : BUILTIN_PADDING;
    kli_print("\t%s\n\n", COMMANDS_BEACON);
    commandEndIndex = 0;
    print_table(KLI_BUILTIN, MAX_PADDING);
    print_table(topTable, MAX_PADDING);
    return NULL;
}

static const char *clear_handler(void) {
    kli_print("\033[2J\033[H");
    return NULL;
}
