#ifndef MEM_8086_SIM_H
#define MEM_8086_SIM_H

#include "type.h"

#define MEM_SIZE_8086 1024 * 1024

typedef struct memory_t {
    u32 src_size;
    u8 mem[MEM_SIZE_8086];
} memory_t;

memory_t *init_memory_t();

int load_memory(int fd, memory_t *mem);

void free_memory(memory_t *mem);

#endif
