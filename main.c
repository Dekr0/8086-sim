#define _GNU_SOURCE
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "decoder.h"
#include "hashtable.h"
#include "opt.h"
#include "print.h"
#include "sim.h"

#define MIN_AUTO_RUN_TIMEOUT 50000

void init_opts(opts_t *opts) {
    opts->argp = 1;

    opts->auto_run = 0;
    opts->auto_run_timeout = MIN_AUTO_RUN_TIMEOUT;
    opts->exec = 0;
    opts->interact = 0;

    opts->binary_i = -1;

    opts->decode_o = NULL;
    opts->memory_o = NULL;
}

i8 argparse(opts_t *opts, const int argc, char **argv) {
    u8 *argp = &opts->argp;
    while (*argp < argc) {
        if (argv[opts->argp][0] != '-')
            break;
        if (!strcmp(argv[*argp], "-e"))
            opts->exec = 1;
        else if (!strcmp(argv[*argp], "-i"))
            opts->interact = 1;
        else if (!strcmp(argv[*argp], "--auto-run")) {
            if (!opts->interact) {
                fprintf(stderr, "Auto run mode must be ran under interactive. Specify --auto-run [timeout in us] right after -i\n");
                return EXIT_FAILURE;
            }
            opts->auto_run = 1;
            u64 r = strtoul(argv[*argp], NULL, 10);
            if (r == ULONG_MAX && errno == ERANGE) {
                fprintf(stderr, "The timout value read is out of range\n");
                return EXIT_FAILURE;
            }
            *argp += 1;
            if (r > MIN_AUTO_RUN_TIMEOUT)
                opts->auto_run_timeout = r;
        } else if (!strcmp(argv[*argp], "-stdout"))
            opts->decode_o = stdout;
        else if (!strcmp(argv[*argp], "-mdump")) {
            opts->argp += 1;
            if (opts->argp >= argc) {
                fprintf(stderr, "Missing memory dump output destination\n");
                return EXIT_FAILURE;
            }
            opts->memory_o = fopen(argv[*argp], "w");
            if (opts->memory_o == NULL) {
                perror("fopen");
                return EXIT_FAILURE;
            }
        } else {
            fprintf(stderr, "%s is not valid option\n", argv[*argp]);
            return EXIT_FAILURE;
        }
        *argp += 1;
    }

    if (!opts->exec && opts->interact) {
        fprintf(stderr, "Interative mode only available when -e is enabled");
        return EXIT_FAILURE;
    }

    if (opts->decode_o == stdout && opts->interact) {
        fprintf(stderr, "Remove -stdout when running in interactive mode");
        return EXIT_FAILURE;
    }

    if (opts->argp >= argc) {
        fprintf(stderr, "Missing binary assembly input file\n");
        return EXIT_FAILURE;
    }

    if (!opts->interact &&
        (opts->decode_o != stdout && opts->argp + 1 >= argc)) {
        fprintf(stderr,
                "Missing disassemble destination. Specify -stdout to output to "
                "stdout\n");
        return EXIT_FAILURE;
    }

    if ((opts->binary_i = open(argv[opts->argp++], O_RDONLY)) == -1) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    if (opts->decode_o != stdout) {
        opts->decode_o = fopen(argv[opts->argp], "w");
        if (opts->decode_o == NULL && !opts->interact) {
            perror("fopen");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    if (argc < 2)
        return EXIT_FAILURE;

    i8 exit = EXIT_FAILURE;
    opts_t opts = {0};
    init_opts(&opts);
    argparse(&opts, argc, argv);

    memory_t *mem = init_memory_t();
    if (mem == NULL)
        goto exit;

    if (load_memory(opts.binary_i, mem) == -1)
        goto exit;

    hashmap_instr_t *instr_stream = load_instr_stream(mem);
    if (instr_stream == NULL)
        goto exit;

    xfprintf(opts.decode_o, "bits 16\n");
    u32 IP = 0;
    instr_t *prev = NULL;
    instr_t *instr = NULL;

    for (u32 instr_counts = 0; instr_counts < hashmap_instr_size(instr_stream);
         instr_counts++) {
        assert((instr = hashmap_instr_get(instr_stream, IP)) != NULL);
        print_instr(instr, 1, 0, opts.decode_o);
        IP += instr->size;
        if (prev != NULL)
            prev->next = instr;
        instr->prev = prev;
        prev = instr;
        xfprintf(opts.decode_o, "\n");
    }

    if (opts.exec) {
        struct sim_opts_t sim_opts = {0};
        init_sim_opts(&sim_opts, &opts);
        simulate(instr_stream, mem, &sim_opts);
    }

    if (opts.memory_o != NULL) {
        if (fwrite(mem->mem, sizeof(char), MEM_SIZE_8086, opts.memory_o) <
            MEM_SIZE_8086) {
            xfprintf(stderr, "fwrite error: write less than 1MB\n");
            goto exit;
        }
    }
    exit = EXIT_SUCCESS;
    goto exit;
exit:
    close(opts.binary_i);

    if (opts.memory_o != NULL)
        fclose(opts.memory_o);
    if (opts.decode_o != NULL)
        fclose(opts.decode_o);

    free_hashmap_instr(instr_stream);
    free_memory(mem);

    return exit;
}
