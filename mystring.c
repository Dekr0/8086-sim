#include <assert.h>
#include <string.h>

#include "mystring.h"


str_builder_t *new_str_builder() {
    str_builder_t *b = calloc(1, sizeof( str_builder_t ));
    assert(b);
    return b; 
}

void concat(str_builder_t *b, char *dest, u32 dest_len) {
    assert(dest);
    if (dest_len + b->size > b->capc) {
        b = realloc(b, sizeof( str_builder_t ) + CAPC);
        b->capc += CAPC;
        assert(b);
    }
    for (u32 i = 0; i < dest_len; i++) {
        b->str[b->size++] = dest[i];
    }
}

void clear(str_builder_t *b) {
    assert(b);
    memset(b->str, 0, b->capc * sizeof( char ));
}

void del_str_builder(str_builder_t *b) {
    assert(b);
    free(b);
}

