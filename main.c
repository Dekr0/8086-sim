#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "decoder.h"
#include "print.h"
#include "sim.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }

    u8 argp = 1;
    u8 exec = 0;
    u8 interactive = 0;
    i8 rf = -1;
    FILE *wf = NULL;
    while (argp < argc) {
        if (argv[argp][0] != '-')
            break;
        if (!strcmp(argv[argp], "-e")) {
            exec = 1;
        } else if (!strcmp(argv[argp], "-i")) {
            interactive = 1;
        } else if (!strcmp(argv[argp], "-stdout")) {
            wf = stdout;
        } else {
            printf("%s is not valid option\n", argv[argp]);
            return EXIT_FAILURE;
        }
        argp++;
    }
    if (argp >= argc) {
        printf("Missing binary assembly input file\n");
        return EXIT_FAILURE;
    }
    if (wf != stdout && argp + 1 >= argc) {
        printf("Missing disassemble destination. Specify -stdout to output to "
               "stdout\n");
        return EXIT_FAILURE;
    }
    rf = open(argv[argp++], O_RDONLY);
    if (rf == -1) {
        perror("fopen");
        return EXIT_FAILURE;
    }
    if (wf != stdout) {
        wf = fopen(argv[argp], "w");
        if (wf == NULL) {
            perror("fopen");
            return EXIT_FAILURE;
        }
    }

    memory_t *mem = init_memory_t();
    if (mem == NULL) {
        return EXIT_FAILURE;
    }

    if (load_memory(rf, mem) == -1) {
        return EXIT_FAILURE;
    }

    instr_stream_t *stream_t = load_instr_stream(mem, wf);
    if (stream_t == NULL) {
        free_memory(mem);
        return EXIT_FAILURE;
    }

    fprintf(wf, "bits 16\n");
    for (u32 i = 0; i < stream_t->count; i++) {
        print_instr(stream_t->stream + i, 1, 0, wf);
        fprintf(wf, "\n");
    }
    fprintf(wf, "\n");

    if (exec) {
        simulate(stream_t, mem, interactive, wf);
    }

    free_instr_stream(stream_t);
    free_memory(mem);
}
