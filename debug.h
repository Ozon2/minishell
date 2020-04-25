/*
 * Some macros to help debugging
 */

#ifndef __DEBUG_H
#define __DEBUG_H

#define DEBUG 1 // (0/1) to hide/show debug information

#include <stdio.h>

#if defined(DEBUG) && DEBUG > 0
#define DEBUG_PRINTF(fmt, ...)                                                                     \
    fprintf(stderr, "\033[0;31m[DEBUG]\033[0m: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__,    \
            __VA_ARGS__)
#define DEBUG_PRINT(fmt)                                                                           \
    fprintf(stderr, "\033[0;31m[DEBUG]\033[0m: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__)
#else
#define DEBUG_PRINTF(fmt, ...) // Don't do anything if not in debug mode
#define DEBUG_PRINT(fmt)
#endif

void *safe_malloc(size_t n);

#endif