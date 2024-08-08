#ifndef CPU_SIM_8086_H
#define CPU_SIM_8086_H


#include "instruction.h"

#define set_cf(f, v) f = f & 0b111111110 | v
#define set_pf(f, v) f = f & 0b111111101 | v << 1
#define set_af(f, v) f = f & 0b111111011 | v << 2
#define set_zf(f, v) f = f & 0b111110111 | v << 3
#define set_sf(f, v) f = f & 0b111101111 | v << 4
#define set_of(f, v) f = f & 0b111011111 | v << 5
#define set_if(f, v) f = f & 0b110111111 | v << 6
#define set_df(f, v) f = f & 0b101111111 | v << 7
#define set_tf(f, v) f = f & 0b011111111 | v << 8

#define get_cf(f) f & 1
#define get_pf(f) (f >> 1) & 1
#define get_af(f) (f >> 2) & 1
#define get_zf(f) (f >> 3) & 1
#define get_sf(f) (f >> 4) & 1
#define get_of(f) (f >> 5) & 1
#define get_if(f) (f >> 6) & 1
#define get_df(f) (f >> 7) & 1
#define get_tf(f) (f >> 8) & 1


typedef struct cpu_t {
    u16 ip;
    u16 flags;
    u16 regs[REG_DS];
} cpu_t;


#endif
