#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string.h"
#include "type.h"

#define SUCCESS 0
#define FAILURE 1

#define INIT_SIZE 128

typedef struct str_builder_t {
    u64   size;
    u64   cap;
    char *buf;
} str_builder_t;

static i8 str_builder_resize(str_builder_t *b, u64 new_cap);

static i8 str_builder_resize(str_builder_t *b, u64 new_cap) {
    if (b == NULL)
        return -1;
    char *old_buf = b->buf;
    if ((b->buf = realloc(b->buf, new_cap * sizeof(char))) == NULL) {
        b->buf = old_buf;
        return FAILURE;
    }
    b->cap < new_cap && memset(b->buf + b->cap, 0, sizeof(char) * (new_cap - b->cap));
    b->cap = new_cap;
    return SUCCESS;
}

str_builder_t *init_str_builder_t() {
    str_builder_t *b = calloc(1, sizeof(str_builder_t));
    if (b == NULL) return NULL;

    if ((b->buf = calloc(INIT_SIZE, sizeof(char))) == NULL) {
        free(b);
        return NULL;
    }

    b->cap = INIT_SIZE;

    return b;
}

i8 put_char(str_builder_t *b, const char c) {
    if (b == NULL) return FAILURE;

    if (b->size + 1 >= b->cap && str_builder_resize(b, b->cap * 2) == FAILURE)
        return FAILURE;

    b->buf[b->size++] = c;

    return SUCCESS;
}

i8 put_str(str_builder_t *b, const char *s, const u64 len) {
    if (b == NULL) return FAILURE;

    if (b->size + len >= b->cap && str_builder_resize(b, b->cap * 2 < len ? len : b->cap * 2) == FAILURE)
        return FAILURE;

    memcpy(b->buf + b->size, s, len);

    b->size += len;

    return SUCCESS;
}

i8 put_fmt(str_builder_t *b, const char *fmt, ...) {
    if (b == NULL) return FAILURE;

    va_list va;
    va_start(va, fmt);
    i64 r = 0;
    r = vsnprintf(b->buf + b->size, b->cap - b->size, fmt, va);
    if (r < -1) return FAILURE;

    if (r > b->cap - b->size) {
        va_end(va);
        if (str_builder_resize(b, b->cap * 2 < r ? r : b->cap * 2) == FAILURE)
            return FAILURE;
        va_start(va, fmt);
        r = vsnprintf(b->buf + b->size, b->cap - b->size, fmt, va);
    }
    b->size += r;
    va_end(va);

    return SUCCESS;
}

const char *build(str_builder_t *b, u64 *len) {
    if (b == NULL) return NULL;
    if (len != NULL)
        *len = b->size;
    return b->buf;
}

i8 reset(str_builder_t *b) {
    if (b == NULL) return FAILURE;
    if (b->cap > INIT_SIZE && str_builder_resize(b, INIT_SIZE) == FAILURE)
        return FAILURE;
    memset(b->buf, 0, b->cap * sizeof(char));
    b->size = 0;
    return SUCCESS;
}

void free_str_builder(str_builder_t *b) {
    if (b == NULL) return;
    if (b->buf != NULL)
        free(b->buf);
    free(b);
}
