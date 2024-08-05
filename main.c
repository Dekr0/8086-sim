#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "decoder.h"
#include "sim.h"


int main(int argc, char **argv) {
    if (argc < 2) { return 1; }

    u8 exec = 0;
    if (argc == 3) {
        exec = !strcmp(argv[1], "--exec");
    }

    memory_t *mem = init_memory_t();
    if (mem == NULL) { return 1; }

    int fd = open(argv[argc - 1], O_RDONLY);
    if (fd == -1) { perror("open"); }

    if (load_memory(fd, mem) == -1) { return 1; }

    src_instructions_t *src = load_instruction(mem);
    if (src == NULL) { 
        free_memory(mem);
        return 1; 
    }

    for (u32 i = 0; i < src->count; i++) {
        print_instruction(src->i_arr + i);
    }

    if (exec) {
        simulate(src, mem);
    }

    free_src_instruction(src);
    free_memory(mem);
}
