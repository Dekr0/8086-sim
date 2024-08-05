#include "table.h"


const char cond_jmp[16][5] = {
    "jo\0", "jno\0", "jb\0", "jnb\0", "je\0", "jne\0", "jbe\0", "jnbe\0", 
    "js\0", "jns\0", "jp\0", "jnp\0", "jl\0", "jnl\0", "jle\0", "jnle\0"
};

const char loop_jmp[4][7] = {
    "loopnz\0", "loopz\0", "loop\0", "jcxz\0"
};

const char reg_name[13][3] = {
    "\0",   "a\0",  "c\0", "d\0", "b\0", "sp\0", "bp\0", "si\0", "di\0", "es\0",
    "cs\0", "ss\0", "ds\0"
};

const reg_t eff_addr_exprs[8][2] = {
    { {REG_B,  0, 2}, {REG_SI,   0, 2} },
    { {REG_B,  0, 2}, {REG_DI,   0, 2} },
    { {REG_BP, 0, 2}, {REG_SI,   0, 2} },
    { {REG_BP, 0, 2}, {REG_DI,   0, 2} },
    { {REG_SI, 0, 2}, {REG_NONE, 0, 0} },
    { {REG_DI, 0, 2}, {REG_NONE, 0, 0} },
    { {REG_BP, 0, 2}, {REG_NONE, 0, 0} }, // exception when mod = 00
    { {REG_B,  0, 2}, {REG_NONE, 0, 0} }, 
};

const reg_t regs[8][2] = {
    { {REG_A, 0, 1}, { REG_A,  0, 2} },
    { {REG_C, 0, 1}, { REG_C,  0, 2} },
    { {REG_D, 0, 1}, { REG_D,  0, 2} },
    { {REG_B, 0, 1}, { REG_B,  0, 2} },
    { {REG_A, 1, 1}, { REG_SP, 0, 2} },
    { {REG_C, 1, 1}, { REG_BP, 0, 2} },
    { {REG_D, 1, 1}, { REG_SI, 0, 2} },
    { {REG_B, 1, 1}, { REG_DI, 0, 2} },
};

const reg_t seg_regs[4] = {
    { REG_ES, 0, 2 },
    { REG_CS, 0, 2 },
    { REG_SS, 0, 2 },
    { REG_DS, 0, 2 }
};
