#include <stdio.h>
#include <stdlib.h>

#include "debug.h"

void *safe_malloc(size_t n) {
    void *p = malloc(n);
    if (p == NULL) {
        fprintf(stderr, "Fatal: failed to allocate %zu bytes.\n", n);
        exit(1);
    }
    return p;
}