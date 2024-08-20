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
        mem->src_size += nread;
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
