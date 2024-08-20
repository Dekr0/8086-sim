#include <stdio.h>

#include "sim.h"
#include "util.h"

int main() {
    u32 a = 0x1000;
    u32 b = 0x9000;
    u16 flag = 0;
    bit_width_e w = BIT_SIZE_16;

    u16 r = (int)binary_sub(&flag, a, b, 0, w);
    printf("flag: %d (%#04x)\n", flag, flag);
    print_binary(flag, 9);
    printf("result: %d (%#04x)", r, r);
}
