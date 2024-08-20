#include <stdio.h>

#include "util.h"

void print_binary(i32 n, u32 bit_size) {
    printf("Binary form of %d: ", n);
    for (i32 i = bit_size - 1; i >= 0; i--) {
        printf("%d", n >> i & 1);
    }
    printf("\n");
}
