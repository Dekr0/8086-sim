#ifndef PRINT_8086_SIM_H
#define PRINT_8086_SIM_H

#include <stdio.h>

#include "cpu.h"
#include "instruction.h"

#define xfprintf(f, ...)                                                       \
    if (f != NULL) {                                                           \
        fprintf(f, __VA_ARGS__);                                               \
    }

#define watch_cpu_ip(cpu, effect, f)                                           \
    xfprintf(f, "IP:%#04x->", cpu->BIU.IP);                                    \
    effect;                                                                    \
    xfprintf(f, "%#04x", cpu->BIU.IP);                                         \
    xfprintf(f, ",");

#define watch_reg_state(reg, cpu, effect, f)                                   \
    print_cpu_reg(cpu, reg, f);                                                \
    effect;                                                                    \
    xfprintf(f, "->");                                                         \
    print_cpu_reg(cpu, reg, f);                                                \
    xfprintf(f, ",");

#define watch_cpu_flags(flags, effect, f)                                      \
    xfprintf(f, "flags:");                                                     \
    print_cpu_flags(flags, f);                                                 \
    effect;                                                                    \
    xfprintf(f, "->");                                                         \
    print_cpu_flags(flags, f);                                                 \
    xfprintf(f, ",");

u32 print_word(const word_t *w, FILE *wf);

u32 print_eff_addr_expr(const eff_addr_expr_t *e, FILE *wf);

u32 print_reg(const reg_t *r, FILE *wf);

u32 print_operand(const operand_t *o, FILE *wf);

u32 print_jmp_instr(const instr_t *in, FILE *wf);

u32 print_instr(const instr_t *instr_t, u8 show_base_addr, u8 as_comment,
                FILE *wf);

void print_cpu(const cpu_t *cpu, FILE *wf);

void print_cpu_flags(const u16 *flag_reg, FILE *wf);

void print_cpu_reg(const cpu_t *cpu, const reg_t *reg_t, FILE *wf);

#endif
