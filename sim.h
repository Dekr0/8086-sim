#ifndef SIM_8086_H
#define SIM_8086_H

#include <stdio.h>

#include "cpu.h"
#include "hashtable.h"
#include "memory.h"
#include "opt.h"

typedef struct sim_opts_t {
    u8 interact;
    u8 auto_run;
    u8 op_mode;
    u8 is8088;
    u64 auto_run_timeout;
    FILE *f;
} sim_opts_t;

/**
 * This current clock cycle estimation only account for execution unit. It does
 * not include bus interface unit. All clock cycle estimation for execution unit
 * assume EU is execute in a non fast series executing manner, and assume there
 * are always instruction bytes object (prefetched) in the instruction queue.
 */
typedef struct clock_counter_t {
    u8 base;
    u8 ea;
    u8 transfer;
    u8 odd_transfer_penalty; // 8086 odd memory transfer penalty
    u8 ref_penalty;          // 16 bits memory reference penalty in 8088
} clock_counter_t;

typedef struct sim_ctx_t {
    u64 clock_count;
    u64 memory_viewer_addr;
    cpu_t *cpu;
    clock_counter_t cc;
    memory_t *mem;
    const hashmap_instr_t *stream;
    const instr_t *asm_viewer_instr;
    const instr_t *curr_instr;
    FILE *f;
} sim_ctx_t;

i32 simulate(const hashmap_instr_t *stream, memory_t *mem,
             const sim_opts_t *opts);

void init_sim_opts(sim_opts_t *sim_opts, const opts_t *opts);

#endif
