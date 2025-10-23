/**
 * @file cli.c
 * @author Killian Baillifard
 * @date 23.10.2025
 * @brief KLI parsing implementation.
 */

// Includes

#include <stdlib.h>
#include <string.h>
#include "kli_parse.h"
#include "kli_config.h"

// Definitions

int kli_parse_line(char *line, char **argv) {
    int argc = 0;
    char *token = strtok(line, " ");
    while(token != NULL && argc < KLI_MAX_ARGC) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    return argc;
}

bool kli_parse_long(const char *string, long *value) {
    char *end;
    *value = strtol(string, &end, 10);
    return end != string;
}

bool kli_parse_float(const char *string, float *value) {
    char *end;
    *value = strtof(string, &end);
    return end != string;
}
