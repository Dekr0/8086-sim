#ifndef SIM_8086_H
#define SIM_8086_H

#include "cpu.h"
#include "memory.h"
#include "type.h"


u32 read_reg(cpu_t *c, const reg_t *r);

void write_reg_val(cpu_t *c, const reg_t *r, u32 v);

void write_reg_reg(cpu_t *c, const reg_t *dest, const reg_t *src);

u32 binary_add(u16 *reg_flag, u32 a, u32 b, const bit_width_e bit_width_e);

u32 binary_sub(u16 *reg_flag, u32 b, u32 a, const bit_width_e bit_width_e);

i32 simulate(src_instructions_t *src, memory_t *mem);

static void print_reg_state(cpu_t *c, const reg_t *r);

void print_cpu_state(cpu_t *c);


#endif
