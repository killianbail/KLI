/**
 * @file kli_dispatch.h
 * @author Killian Baillifard
 * @date 23.10.2025
 * @brief KLI command dispatch interface.
 */

#ifndef KLI_DISPATCH_H
#define KLI_DISPATCH_H

#ifdef __cplusplus
extern "C" {
#endif

// Includes

#include "kli_optargs.h"

// Definitions

/**
 * @brief Macros to define command tables.
 * @param name Name of the command table.
 * @note Each command table must be terminated with KLI_END_COMMAND_TABLE.
 * @note Use KLI_EXPORT_COMMAND_TABLE to declare external command tables.
 */
#define KLI_BEGIN_COMMAND_TABLE(name)                                           const KliCommand name[] = {

/**
 * @brief Macros to add a subcommand entry to the current table.
 * @param name Name of the subcommand as a null terminated string.
 * @param description Description of the subcommand as a null terminated string.
 * @param subcommand Pointer to the subcommand table.
 * @note Subcommands take precedence over command handlers.
 * @note Command name must not contain spaces.
 */
#define KLI_ADD_SUBCOMMAND_TABLE(name, description, subcommand)                 {name, description, subcommand, NULL, NULL, NULL},

/**
 * @brief Macros to add a command handler entry to the current table.
 * @param name Name of the command as a null terminated string.
 * @param description Description of the command as a null terminated string.
 * @param options Pointer to the options table.
 * @param arguments Pointer to the positional arguments table.
 * @param handler Pointer to the command handler function.
 * @note Options and arguments can be set to NULL if not needed.
 * @note Command name must not contain spaces.
 */
#define KLI_ADD_COMMAND_HANDLER(name, description, options, arguments, handler) {name, description, NULL, options, arguments, handler},

/**
 * @brief Macros to end the current command table.
 * @note Must be used to terminate each command table.
 * @note The terminating entry is filled with NULL values.
 */
#define KLI_END_COMMAND_TABLE                                                   {NULL, NULL, NULL, NULL, NULL, NULL}};

/**
 * @brief Macros to declare an external command table.
 * @param name Name of the command table.
 * @note Use this macro in header files to declare command tables defined elsewhere.
 */
#define KLI_EXPORT_COMMAND_TABLE(name)                                          extern const KliCommand name[]

// Structures

/**
 * @brief KLI command structure.
 */
typedef struct KliCommand {
    
    // Name of the command, must not contain any spaces.
    const char *name;

    // Command description.
    const char *description;

    // Subcommand table, if provided, all the following arguments are ignored.
    const struct KliCommand * const subcommands;

    // Options table
    const KliOption * const options;

    // Postionnal arguments table
    const KliArgument * const arguments;

    /**
     * @brief Command handler signature.
     * @return NULL if command succeded, an error message otherwise.
     * @note Will never be called if a subtable is provided.
     */
    const char *(*handler)(void);

} KliCommand;

// Prototypes

/**
 * @brief Look for a table entry that match the given arguments.
 * @param table Top level table containing subtables and handlers.
 * @param argc Argument count.
 * @param argv Argument values.
 */
void kli_dispatch(const KliCommand table[], int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif /* KLI_DISPATCH_H */
