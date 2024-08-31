#ifndef SIM_8086_H
#define SIM_8086_H

#include <stdio.h>

#include "arithmetic.h"
#include "cpu.h"
#include "hashtable.h"
#include "memory.h"

typedef struct sim_ctx_t sim_ctx_t;

i32 simulate(hashmap_instr_t *src, memory_t *mem, u8 interactive, FILE *wf);

#endif
