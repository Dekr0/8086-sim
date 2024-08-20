#ifndef MY_STRING_H
#define MY_STRING_H

#include <stdlib.h>

#include "type.h"

#define CAPC 2048

typedef struct str_builder_t {
    u32 size;
    u32 capc;
    char str[CAPC];
} str_builder_t;

str_builder_t *new_str_builder();

void concat(str_builder_t *b, char *dest, u32 dest_len);

void clear(str_builder_t *b);

void del_str_builder(str_builder_t *b);

#endif
