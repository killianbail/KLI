/**
 * @file kli_print.c
 * @author Killian Baillifard
 * @date 23.10.2025
 * @brief KLI print implementation.
 */

// Includes

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "kli_print.h"
#include "kli_config.h"
#include "kli_platform.h"

// Static variables

static char string[KLI_MAX_PRINT_SIZE] = {0};
static int size = 0;

// Definitions

void kli_print(const char *format, ...) {

    // Compute space left in the buffer
    const size_t LEFT_PRINT_SPACE = KLI_MAX_PRINT_SIZE - size;

    // Format string
    va_list args;
    va_start(args, format);
    int formattedLength = vsnprintf(&string[size], LEFT_PRINT_SPACE, format, args);
    va_end(args);

    // Handle case where KLI_MAX_PRINT_SIZE is reached, display warning
    if(formattedLength > LEFT_PRINT_SPACE - 1) {
        kli_flush();
        kli_print("\n\n\t\tWARNING - KLI_MAX_PRINT_SIZE REACHED\n\n");
        kli_flush();
    }

    // Increment print buffer character count
    else
        size += strnlen(&string[size], LEFT_PRINT_SPACE - 1);
}

void kli_flush(void) {
    kli_out(string);
    size = 0;
}
