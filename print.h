#ifndef PRINT_8086_SIM_H
#define PRINT_8086_SIM_H

#include <stdio.h>

#include "cpu.h"
#include "instruction.h"

#define watch_cpu_ip(cpu, f, wf)                                               \
    fprintf(wf, "IP:%#04x->", cpu->ip);                                        \
    f;                                                                         \
    fprintf(wf, "%#04x", cpu->ip);                                             \
    fprintf(wf, ",");

#define watch_reg_state(reg, cpu, f, wf)                                       \
    print_cpu_reg(cpu, reg, wf);                                               \
    f;                                                                         \
    fprintf(wf, "->");                                                         \
    print_cpu_reg(cpu, reg, wf);                                               \
    fprintf(wf, ",");

#define watch_cpu_flags(flags, f, wf)                                          \
    fprintf(wf, "flags:");                                                     \
    print_cpu_flags(flags, wf);                                                \
    f;                                                                         \
    fprintf(wf, "->");                                                         \
    print_cpu_flags(flags, wf);                                                \
    fprintf(wf, ",");

i32 print_word(word_t *w, FILE *wf);

i32 print_eff_addr_expr(eff_addr_expr_t *e, FILE *wf);

i32 print_reg(const reg_t *r, FILE *wf);

i32 print_operand(operand_t *o, FILE *wf);

void print_jmp_instr(instr_t *in, FILE *wf);

void print_instr(instr_t *instr_t, u8 show_base_addr, u8 as_comment, FILE *wf);

void print_cpu(cpu_t *cpu, FILE *wf);

void print_cpu_flags(u16 *flag_reg, FILE *wf);

void print_cpu_reg(cpu_t *cpu, const reg_t *reg_t, FILE *wf);

#endif
