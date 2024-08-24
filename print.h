#ifndef PRINT_8086_SIM_H
#define PRINT_8086_SIM_H

#include <stdio.h>

#include "cpu.h"
#include "instruction.h"

#define xfprintf(f, ...)                                                       \
    if (f != NULL) {                                                           \
        fprintf(f, __VA_ARGS__);                                               \
    }

#define watch_cpu_ip(cpu, effect, wf)                                          \
    xfprintf(wf, "IP:%#04x->", cpu->ip);                                       \
    effect;                                                                    \
    xfprintf(wf, "%#04x", cpu->ip);                                            \
    xfprintf(wf, ",");

#define watch_reg_state(reg, cpu, effect, wf)                                  \
    print_cpu_reg(cpu, reg, wf);                                               \
    effect;                                                                    \
    xfprintf(wf, "->");                                                        \
    print_cpu_reg(cpu, reg, wf);                                               \
    xfprintf(wf, ",");

#define watch_cpu_flags(flags, effect, wf)                                     \
    xfprintf(wf, "flags:");                                                    \
    print_cpu_flags(flags, wf);                                                \
    effect;                                                                    \
    xfprintf(wf, "->");                                                        \
    print_cpu_flags(flags, wf);                                                \
    xfprintf(wf, ",");

u32 print_word(word_t *w, FILE *wf);

u32 print_eff_addr_expr(eff_addr_expr_t *e, FILE *wf);

u32 print_reg(const reg_t *r, FILE *wf);

u32 print_operand(operand_t *o, FILE *wf);

u32 print_jmp_instr(instr_t *in, FILE *wf);

u32 print_instr(instr_t *instr_t, u8 show_base_addr, u8 as_comment, FILE *wf);

void print_cpu(cpu_t *cpu, FILE *wf);

void print_cpu_flags(u16 *flag_reg, FILE *wf);

void print_cpu_reg(cpu_t *cpu, const reg_t *reg_t, FILE *wf);

#endif
