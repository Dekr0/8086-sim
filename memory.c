#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "memory.h"

memory_t *init_memory_t() {
    memory_t *mem = calloc(1, sizeof(memory_t));
    if (mem == NULL) {
        fprintf(stderr, "memory allocation error\n");
    }
    return mem;
}

int load_memory(int fd, memory_t *mem) {
    assert(mem != NULL);
    int nread = 0;
    while ((nread = read(fd, mem->mem, MEM_SIZE_8086)) > 0) {
        mem->source_end += nread;
    }
    if (nread == 0) {
        return 0;
    }
    if (nread == -1) {
        perror("read");
        return -1;
    }
    return 0;
}

void free_memory(memory_t *mem) {
    assert(mem != NULL);
    free(mem);
}

u32 read_mem(memory_t *mem, u32 addr, const bit_width_e bit_width_e) {
    assert(addr >= mem->source_end);
    u32 v = 0;
    switch (bit_width_e) {
    case BIT_SIZE_0:
        return 0;
    case BIT_SIZE_8:
        return mem->mem[addr];
    case BIT_SIZE_16:
        v |= mem->mem[addr];
        v |= mem->mem[addr + 1] << 8;
        return v;
    }
    return 0;
}

void write_mem(memory_t *mem, u32 addr, u32 val,
               const bit_width_e bit_width_e) {
    assert(addr >= mem->source_end);
    switch (bit_width_e) {
    case BIT_SIZE_0:
        return;
    case BIT_SIZE_8:
        assert(val <= 0xff);
        mem->mem[addr] = val;
        break;
    case BIT_SIZE_16:
        assert(val <= 0xffff);
        mem->mem[addr] = val & 0xff;
        mem->mem[addr + 1] = val >> 8 & 0xff;
        break;
    }
}
