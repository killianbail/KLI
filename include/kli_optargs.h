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

#define KLI_BEGIN_OPTION_TABLE(name)                            const KliOption name[] = {
#define KLI_ADD_OPTION(shortName, longName, argc, description)  {shortName, longName, argc, description},
#define KLI_END_OPTION_TABLE                                    {0, NULL, 0, NULL}};
#define KLI_BEGIN_ARGUMENT_TABLE(name)                          const KliArgument name[] = {
#define KLI_ADD_ARGUMENT(name, description)                     {name, description},
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
bool cli_get_opt(int index, char **optv);

/**
 * @brief Called by user to get parsed argument.
 * @param index Index of the option in its argument array.
 * @param argv Argument value return pointer. Expect the address of a const char pointer.
 * @return True if the argument was found during parsing, false otherwise.
 * @note Must be called inside command handler.
 */
bool cli_get_arg(int index, char **argv);

#ifdef __cplusplus
}
#endif

#endif /* KLI_OPTARGS_H */
