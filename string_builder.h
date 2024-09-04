#ifndef STRING_8086_SIM_H
#define STRING_8086_SIM_H

#include "type.h"

typedef struct str_builder_t str_builder_t;

str_builder_t *init_str_builder_t();

i8 put_char(str_builder_t *b, const char c);

i8 put_str(str_builder_t *b, const char *s, const u64 len);

i8 put_fmt(str_builder_t *b, const char *fmt, ...);

const char *build(str_builder_t *b, u64 *len);

i8 reset(str_builder_t *b);

void free_str_builder(str_builder_t *b);

#endif // !STRING_8086_SIM_H
