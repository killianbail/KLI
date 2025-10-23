/**
 * @file kli_print.h
 * @author Killian Baillifard
 * @date 23.10.2025
 * @brief KLI print interface.
 */

#ifndef KLI_PRINT_H
#define KLI_PRINT_H

#ifdef __cplusplus
extern "C" {
#endif

// Prototypes

/**
 * @brief Format string in KLI output buffer.
 * @param format Format string.
 * @param ... Additional arguments.
 * @note Formatted string is not printed until 'cli_flush' is called.
 */
void kli_print(const char *format, ...);

/**
 * @brief Flush the print buffer through the platform 'kli_out' function.
 */
void kli_flush(void);

#ifdef __cplusplus
}
#endif

#endif /* KLI_PRINT_H */
