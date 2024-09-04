#define _GNU_SOURCE
#include <notcurses/notcurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cpu.h"
#include "tui.h"

int main() {
    cpu_t c = {0};
    c.EU.regs[0] = 1;
    c.EU.regs[1] = 2;
    c.EU.regs[2] = 3;
    c.EU.regs[3] = 4;
    c.EU.regs[4] = 5;
    c.EU.regs[5] = 6;
    c.EU.regs[6] = 7;
    c.EU.regs[7] = 8;
    tui_t *t = init_tui();
    if (t == NULL)
        return EXIT_FAILURE;

    FILE *log = fopen("log.txt", "w");

    struct ncinput ni = { 0 };

    warmup_input(t);

    while (1) {
        ni.evtype = NCTYPE_PRESS;
        if (render(t, &c, NULL)) {
            fprintf(log, "Renering failed\n");
            fflush(log);
        }
        u32 id = get_input(t);
        if (id == 'q') break;
        usleep(100000);
    }

    free_tui(t);

    return EXIT_SUCCESS;
}
