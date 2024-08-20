#ifndef SIM_8086_H
#define SIM_8086_H

#include "cpu.h"
#include "memory.h"
#include "type.h"

typedef u32 (*arithmetic)(u16 *, u32, u32, const bit_width_e);

i32 simulate(instr_stream_t *src, memory_t *mem);

u32 binary_add(u16 *flags_reg, u32 a, u32 b, const bit_width_e width_e);

u32 binary_sub(u16 *flags_reg, u32 a, u32 b, const bit_width_e bit_width_e);

u32 read_reg(cpu_t *cpu, const reg_t *reg_t);

void write_reg_val(cpu_t *cpu, const reg_t *reg_t, u32 val);

void write_reg_reg(cpu_t *cpu, const reg_t *dest_reg_t, const reg_t *src_reg_t);

void print_cpu_state(cpu_t *cpu);

void print_cpu_flags_state(u16 *flag_reg);

void print_reg_state(cpu_t *cpu, const reg_t *reg_t);

static void imm_reg_mem_ops_assert(instr_t *instr_t, operand_t *from,
                                   operand_t *to);

static void imm_reg_ops_assert(instr_t *instr_t, operand_t *from,
                               operand_t *to);

static void imm_acc_ops_assert(instr_t *instr_t, operand_t *reg_a_t,
                               operand_t *imm);

static void sim_mov_reg_mem(cpu_t *cpu, instr_t *instr_t);

static void sim_mov_imm_reg_mem(cpu_t *cpu, instr_t *instr_t);

static void sim_mov_imm_reg(cpu_t *cpu, instr_t *instr_t);

static void sim_mov_acc(cpu_t *cpu, instr_t *instr_t);

static void sim_arithmetic_reg_mem(cpu_t *cpu, instr_t *instr_t,
                                   arithmetic arithmetic_func, i8 write_back);

static void sim_arithmetic_imm_reg_mem(cpu_t *cpu, instr_t *instr_t,
                                       arithmetic arithmetic_func,
                                       i8 write_back);

static void sim_arithmetic_imm_acc(cpu_t *cpu, instr_t *instr_t,
                                   arithmetic arithmetic_func, i8 write_back);

static void sim_cond_jmp(cpu_t *cpu, instr_t *i, instr_stream_t *stream_t,
                         i32 *stream_cursor);

#endif
