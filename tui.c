#include <stdio.h>
#include <unistd.h>

/* STDOUT is readable */
int main() {
    printf("\x1b[2J");
    fflush(stdout);
    for (unsigned int i = 0; i < 100; i++) {
        printf("\x1b[H%d", i);
        fflush(stdout);
        usleep(50000);
    }
}
