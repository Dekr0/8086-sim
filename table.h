#ifndef DECODER_8086_SIM_H
#define DECODER_8086_SIM_H

#include "instruction.h"


extern const char  reg_name[13][3];
extern const char  cond_jmp[16][5];
extern const char  loop_jmp[4][7];
extern const reg_t regs[8][2];
extern const reg_t eff_addr_exprs[8][2];


#endif
