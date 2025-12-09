/**
 * @file kli_optargs.h
 * @author Killian Baillifard
 * @date 23.10.2025
 * @brief KLI options and arguments interface.
 */

#ifndef KLI_OPTARGS_H
#define KLI_OPTARGS_H

#ifdef __cplusplus
extern "C" {
#endif

// Includes

#include <stdbool.h>
#include <stddef.h>

// Definitions


/**
 * @brief Macro to define an option table.
 * @param name Name of the option table.
 * @note Each option table must be terminated with KLI_END_OPTION_TABLE.
 */
#define KLI_BEGIN_OPTION_TABLE(name)                            const KliOption name[] = {

/**
 * @brief Macro to add an option entry to the current table.
 * @param shortName Short name of the option. Use 0 if no short name is needed.
 * @param longName Long name of the option. Use NULL if no long name is needed.
 * @param argc Number of expected arguments for this option.
 * @param description Description of the option as a null terminated string.
 * @note Option names must not contain spaces.
 */
#define KLI_ADD_OPTION(shortName, longName, argc, description)  {shortName, longName, argc, description},

/**
 * @brief Macro to end the current option table.
 * @note Must be used to terminate each option table.
 * @note The terminating entry is filled with 0 and NULL values.
 */
#define KLI_END_OPTION_TABLE                                    {0, NULL, 0, NULL}};

/**
 * @brief Macro to define an argument table.
 * @param name Name of the argument table.
 * @note Each argument table must be terminated with KLI_END_ARGUMENT_TABLE.
 */
#define KLI_BEGIN_ARGUMENT_TABLE(name)                          const KliArgument name[] = {

/**
 * @brief Macro to add an argument entry to the current table.
 * @param name Name of the argument as a null terminated string.
 * @param description Description of the argument as a null terminated string.
 * @note Argument names must not contain spaces.
 */
#define KLI_ADD_ARGUMENT(name, description)                     {name, description},

/**
 * @brief Macro to end the current argument table.
 * @note Must be used to terminate each argument table.
 * @note The terminating entry is filled with NULL values.
 */
#define KLI_END_ARGUMENT_TABLE                                  {NULL, NULL}};

// Structures

/**
 * @brief KLI option structure.
 */
typedef struct KliOption {

    // Short name of the option. No short name if set to 0.
    char shortName;

    // Long name of the option. Must no contain spaces. No long name if set to 0.
    const char* longName;

    // Number of expected arguments.
    int argc;
    
    // Description of the option.
    const char* description;

} KliOption;

/**
 * @brief KLI argument structure.
 */
typedef struct KliArgument {

    // Name of the argument
    const char* name;

    // Description of the argument.
    const char* description;

} KliArgument;

// Prototypes

/**
 * @brief Called by the dispatch function to parse remaining argc and argv into options and arguments.
 * @param options Array of options to find.
 * @param arguments Array of arguments to find.
 * @param argc Argument count.
 * @param argv Argument values.
 * @return True if all argument values could be parsed, false otherwise.
 */
bool kli_optargs(const KliOption options[], const KliArgument arguments[], int argc, char **argv);

/**
 * @brief Called by user to get parsed option.
 * @param index Index of the option in its option array.
 * @param optv Option value return pointer. Expect the address of an array of const char pointers, or NULL if the option doesn't take any argument.
 * @return True if the option was found during parsing, false otherwise.
 * @note Must be called inside command handler.
 */
bool kli_get_opt(int index, char **optv);

/**
 * @brief Called by user to get parsed argument.
 * @param index Index of the option in its argument array.
 * @param argv Argument value return pointer. Expect the address of a const char pointer.
 * @return True if the argument was found during parsing, false otherwise.
 * @note Must be called inside command handler.
 */
bool kli_get_arg(int index, char **argv);

#ifdef __cplusplus
}
#endif

#endif /* KLI_OPTARGS_H */
