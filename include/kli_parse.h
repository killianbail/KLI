/**
 * @file kli_parse.h
 * @author Killian Baillifard
 * @date 23.10.2025
 * @brief KLI parsing interface.
 */

#ifndef KLI_PARSE_H
#define KLI_PARSE_H

#ifdef __cplusplus
extern "C" {
#endif

// Includes

#include <stdbool.h>

// Prototypes

/**
 * @brief Split line into argument values.
 * @param line Null terminated string.
 * @param argv Array of pointers to argument values.
 * @return Argument count (argc).
 */
int kli_parse_line(char *line, char **argv);

/**
 * @brief Try to parse string into a long value.
 * @param string Null terminated string.
 * @param value Pointer to a long variable. Parse output will be stored here.
 * @return True if value was succesfully parsed, false otherwise. 
 */
bool kli_parse_long(const char *string, long *value);

/**
 * @brief Try to parse string into a float value.
 * @param string Null terminated string.
 * @param value Pointer to a float variable. Parse output will be stored here.
 * @return True if value was succesfully parsed, false otherwise. 
 */
bool kli_parse_float(const char *string, float *value);

#ifdef __cplusplus
}
#endif

#endif /* KLI_PARSE_H */
