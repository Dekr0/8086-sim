#ifndef CPU_SIM_8086_H
#define CPU_SIM_8086_H


#include "instruction.h"


typedef struct cpu_t {
    u16 ip;
    u16 flags;
    u16 regs[REG_DS];
} cpu_t;


#endif
