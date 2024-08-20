#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "decoder.h"
#include "instruction.h"
#include "sim.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }

    u8 exec = 0;
    if (argc == 3) {
        exec = !strcmp(argv[1], "--exec");
    }

    memory_t *mem = init_memory_t();
    if (mem == NULL) {
        return 1;
    }

    int fd = open(argv[argc - 1], O_RDONLY);
    if (fd == -1) {
        perror("open");
    }

    if (load_memory(fd, mem) == -1) {
        return 1;
    }

    instr_stream_t *stream_t = load_instr_stream(mem);
    if (stream_t == NULL) {
        free_memory(mem);
        return 1;
    }

    for (u32 i = 0; i < stream_t->count; i++) {
        print_instr(stream_t->stream + i, 1, 0);
        printf("\n");
    }
    printf("\n");

    if (exec) {
        simulate(stream_t, mem);
    }

    free_instr_stream(stream_t);
    free_memory(mem);
}
