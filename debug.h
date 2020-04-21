/*
 * Some macros to help debugging
 */

#ifndef __DEBUG_H
#define __DEBUG_H

#include <stdio.h>

#define DEBUG 1

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

#endif