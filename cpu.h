#ifndef CPU_SIM_8086_H
#define CPU_SIM_8086_H

#include "instruction.h"

#define set_cf(flag_reg, v) flag_reg = (flag_reg & 0b111111110) | (v)
#define set_pf(flag_reg, v) flag_reg = (flag_reg & 0b111111101) | ((v) << 1)
#define set_af(flag_reg, v) flag_reg = (flag_reg & 0b111111011) | ((v) << 2)
#define set_zf(flag_reg, v) flag_reg = (flag_reg & 0b111110111) | ((v) << 3)
#define set_sf(flag_reg, v) flag_reg = (flag_reg & 0b111101111) | ((v) << 4)
#define set_of(flag_reg, v) flag_reg = (flag_reg & 0b111011111) | ((v) << 5)
#define set_if(flag_reg, v) flag_reg = (flag_reg & 0b110111111) | ((v) << 6)
#define set_df(flag_reg, v) flag_reg = (flag_reg & 0b101111111) | ((v) << 7)
#define set_tf(flag_reg, v) flag_reg = (flag_reg & 0b011111111) | ((v) << 8)

#define get_cf(flag_reg) flag_reg & 1
#define get_pf(flag_reg) (flag_reg >> 1) & 1
#define get_af(flag_reg) (flag_reg >> 2) & 1
#define get_zf(flag_reg) (flag_reg >> 3) & 1
#define get_sf(flag_reg) (flag_reg >> 4) & 1
#define get_of(flag_reg) (flag_reg >> 5) & 1
#define get_if(flag_reg) (flag_reg >> 6) & 1
#define get_df(flag_reg) (flag_reg >> 7) & 1
#define get_tf(flag_reg) (flag_reg >> 8) & 1

typedef struct cpu_t {
    i32 ip;
    u16 flags;
    u16 regs[REG_DS];
} cpu_t;

#endif
