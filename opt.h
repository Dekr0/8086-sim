#ifndef OPT_SIM_8086_H
#define OPT_SIM_8086_H

#include <stdio.h>

#include "type.h"

typedef struct opts_t {
    u8 argp;
    u8 auto_run;
    u64 auto_run_timeout;
    u8 exec;
    u8 interact;
    u8 is8088;
    u8 op_mode;
    i8 binary_i;
    FILE *decode_o;
    FILE *memory_o;
} opts_t;

#endif
