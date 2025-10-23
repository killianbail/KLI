/**
 * @file kli_platform.h
 * @author Killian Baillifard
 * @date 23.10.2025
 * @brief KLI platform interface.
 */

#ifndef KLI_PLATFORM_H
#define KLI_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif

// Prototypes

/**
 * @brief KLI output function, must be implemented by user.
 * @param string Pointer to a null terminated string.
 * @param length Length of the string without null termination.
 */
void kli_out(const char *string, int length);

#ifdef __cplusplus
}
#endif

#endif /* KLI_PLATFORM_H */
