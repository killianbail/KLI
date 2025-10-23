/**
 * @file kli_config.h
 * @author Killian Baillifard
 * @date 23.10.2025
 * @brief KLI configuration.
 */

#ifndef KLI_CONFIG_H
#define KLI_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

// Definitions

#define KLI_MAX_LINE_SIZE               256     // 'cli_parse_line' maximum line size.
#define KLI_MAX_ARGC                    64      // 'cli_parse_line' maximum number of arguments.
#define KLI_MAX_PADDING                 128     // Maximum padding used for help messages printing.
#define KLI_INT32_MAX_DIGITS            12      // Maximum number of digits needed to represent a 32 bit signed integer in base 10.
#define KLI_MAX_PRINT_SIZE              1024    // Maximum amout of characters print output can hold before a flush.

#ifdef __cplusplus
}
#endif

#endif /* KLI_CONFIG_H */
