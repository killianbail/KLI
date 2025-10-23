/**
 * @file kli_optargs.h
 * @author Killian Baillifard
 * @date 23.10.2025
 * @brief KLI options and arguments implementation.
 */

// Includes

#include <string.h>
#include "kli_parse.h"
#include "kli_config.h"
#include "kli_print.h"
#include "kli_optargs.h"

// Static variables

static bool tokenParsed[KLI_MAX_ARGC] = {false};
static char argFound[KLI_MAX_ARGC] = {false};
static char *argvBuffer[KLI_MAX_ARGC] = {NULL};
static char optFound[KLI_MAX_ARGC] = {false};
static int optvLength[KLI_MAX_ARGC] = {0};
static char *optvBuffer[KLI_MAX_ARGC][KLI_MAX_ARGC] = {NULL};

// Static prototypes

/**
 * @brief Check if the argument is an alphabetic character (A-Z or a-z).
 * @param c Character to verify.
 * @return True if the character is alphabetic, false otherwise.
 */
static inline bool is_alphabetic(char c);

/**
 * @brief Check if string is a short option (e.g. -a).
 * @param string Null terminated string.
 * @return True is string is a short option, false otherwise.
 */
static inline bool is_short_option(const char *string);

/**
 * @brief Check if string is a long option (e.g. --all).
 * @param string Null terminated string.
 * @return True is string is a short option, false otherwise.
 */
static inline bool is_long_option(const char *string);

// Definitions

bool kli_optargs(const KliOption options[], const KliArgument arguments[], int argc, char **argv) {

    // Reset caches
    for(int i = 0; i < KLI_MAX_ARGC; i++) {
        tokenParsed[i] = false;
        argFound[i] = false;
        argvBuffer[i] = NULL;
        optFound[i] = false;
        optvLength[i] = 0;
        for(int j = 0; j < KLI_MAX_ARGC; j++)
            optvBuffer[i][j] = NULL;
    }

    // Search options
    if(options) {
        int opti = 0;
        for(const KliOption *option = options; option->shortName || option->longName; option++) {
            for(int i = 0; i < argc;) {
                
                // Token already parsed, skip
                if(tokenParsed[i]) {
                    ++i;
                    continue;
                }
                
                // Skip if not the searched option
                bool shortNameFound = is_short_option(argv[i]) && argv[i][1] == option->shortName && !argv[i][2];
                bool longNameFound = is_long_option(argv[i]) && !strcmp(&argv[i][2], option->longName);
                if(!shortNameFound && !longNameFound) {
                    ++i;
                    continue;
                }

                // Ensure option was not already specified
                if(optFound[opti]) {
                    if(shortNameFound)
                        kli_print("\t'-%c' option - already specified\n\n", option->shortName);
                    if(longNameFound)
                        kli_print("\t'--%s' option - already specified\n\n", option->longName);
                    return false;
                }
                
                // Mark option as found
                tokenParsed[i++] = true;
                optFound[opti] = true;
                optvLength[opti] = option->argc;
                if(!option->argc)
                    break;

                // Count number of valid tokens following the option.
                int remainingUnparsedTokens = 0;
                for(int j = 0; (j < option->argc) && (i + j < argc); j++) {
                    if(tokenParsed[i + j] || is_short_option(argv[i + j]) || is_long_option(argv[i + j]))
                        break;
                    else
                        ++remainingUnparsedTokens;
                }

                // Ensure enough arguments follow the option
                if(remainingUnparsedTokens < option->argc) {
                    if(shortNameFound)
                        kli_print("\t'-%c' option - not enough arguments\n\n", option->shortName);
                    if(longNameFound)
                        kli_print("\t'--%s' option - not enough arguments\n\n", option->longName);
                    return false;
                }

                // Save option values
                for(int j = 0; j < option->argc; j++) {
                    optvBuffer[opti][j] = argv[i];
                    tokenParsed[i++] = true;
                }
            }
            ++opti;
        }
    }

    // Search arguments
    if(arguments) {
        int argi = 0;
        for(const KliArgument *argument = arguments; argument->name; argument++) {
            for(int i = 0; i < argc; i++) {

                // Token already parsed, skip
                if(tokenParsed[i])
                    continue;

                // Encoutered an unmarked option
                if(is_short_option(argv[i])) {
                    kli_print("\t'-%c' - unrecognized option\n\n", argv[i][1]);
                    return false;
                }
                if(is_long_option(argv[i])) {
                    kli_print("\t'--%s' - unrecognized option\n\n", &argv[i][1]);
                    return false;
                }

                // Save argument value
                argFound[argi] = true;
                argvBuffer[argi] = argv[i];
                tokenParsed[i] = true;
            }
            ++argi;
        }
    }

    // Check for any unparsed argument
    for(int i = 0; i < argc; i++) {
        if(!tokenParsed[i]) {
            kli_print("\t'%s' - unrecognized argument\n\n", argv[i]);
            return false;
        }
    }

    // All argument succesfully parsed
    return true;
}

bool kli_get_opt(int opti, char **optv) {

    // Option was not found
    if(!optFound[opti])
        return false;

    // Return option without value(s) found
    if(!optvLength[opti])
        return true;
    
    // Return option with value(s) found
    for(int i = 0; i < optvLength[opti]; i++)
        optv[i] = optvBuffer[opti][i];
    return true;
}

bool kli_get_arg(int argi, char **argv) {
    
    // Argument was not found
    if(!argFound[argi])
        return false;
    
    // Write output value
    *argv = argvBuffer[argi];
    return true;
}

// Static definitions

static inline bool is_alphabetic(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

static inline bool is_short_option(const char *string) {
    return string[0] == '-' && is_alphabetic(string[1]) && string[2] == '\0';
}

static inline bool is_long_option(const char *string) {
    return string[0] == '-' && string[1] == '-' && is_alphabetic(string[2]);
}
