#ifndef SIM_8086_H
#define SIM_8086_H

#include <stdio.h>

#include "arithmetic.h"
#include "cpu.h"
#include "memory.h"

typedef struct sim_ctx_t {
    cpu_t *cpu;
    memory_t *mem;
    instr_stream_t *stream;
    instr_t *curr_instr;
    i32 *stream_cursor;
    FILE *wf;
} sim_ctx_t;

i32 simulate(instr_stream_t *src, memory_t *mem, u8 interactive, FILE *wf);

static void imm_reg_mem_ops_assert(instr_t *instr_t, operand_t *from,
                                   operand_t *to);

static void imm_reg_ops_assert(instr_t *instr_t, operand_t *from,
                               operand_t *to);

static void imm_acc_ops_assert(instr_t *instr_t, operand_t *reg_a_t,
                               operand_t *imm);

static void sim_mov_reg_mem(sim_ctx_t *ctx);

static void sim_mov_imm_reg_mem(sim_ctx_t *ctx);

static void sim_mov_imm_reg(sim_ctx_t *ctx);

static void sim_mov_acc(sim_ctx_t *ctx);

static void sim_arithmetic_reg_mem(sim_ctx_t *ctx, arithmetic arithmetic_func,
                                   i8 write_back);

static void sim_arithmetic_imm_reg_mem(sim_ctx_t *ctx,
                                       arithmetic arithmetic_func,
                                       i8 write_back);

static void sim_arithmetic_imm_acc(sim_ctx_t *ctx, arithmetic arithmetic_func,
                                   i8 write_back);

static void sim_cond_jmp(sim_ctx_t *ctx);

#endif
