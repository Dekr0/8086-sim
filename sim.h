#ifndef SIM_8086_H
#define SIM_8086_H

#include "cpu.h"
#include "memory.h"
#include "type.h"

i32 read_reg(reg_t *r);

i32 write_reg(reg_t *r);

i32 simulate(src_instructions_t *src, memory_t *mem);

void print_regs_state(cpu_t *c);


#endif
