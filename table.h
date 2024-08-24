#ifndef TABLE_8086_SIM_H
#define TABLE_8086_SIM_H

#include "instruction.h"

const char *get_reg_name(u8 i);

const char *get_cond_jmp(u8 i);

const char *get_loop_jmp(u8 i);

const char *get_flags_names(u8 i);

const reg_t *get_regs(u8 i, u8 w);

const reg_t *get_eff_addr_exprs(u8 i, u8 w);

const reg_t *get_seg_regs(u8 i);

#endif
