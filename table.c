#include "table.h"

const char cond_jmp[16][5] = {
    "jo\0", "jno\0", "jb\0", "jnb\0", "je\0", "jne\0", "jbe\0", "jnbe\0",
    "js\0", "jns\0", "jp\0", "jnp\0", "jl\0", "jnl\0", "jle\0", "jnle\0"};

const char loop_jmp[4][7] = {"loopnz\0", "loopz\0", "loop\0", "jcxz\0"};

const char reg_name[13][3] = {"\0",   "a\0",  "c\0",  "d\0",  "b\0",
                              "sp\0", "bp\0", "si\0", "di\0", "es\0",
                              "cs\0", "ss\0", "ds\0"};

const reg_t eff_addr_exprs[8][2] = {
    {{REG_B, BIT_SIZE_0, BIT_SIZE_16}, {REG_SI, BIT_SIZE_0, BIT_SIZE_16}},
    {{REG_B, BIT_SIZE_0, BIT_SIZE_16}, {REG_DI, BIT_SIZE_0, BIT_SIZE_16}},
    {{REG_BP, BIT_SIZE_0, BIT_SIZE_16}, {REG_SI, BIT_SIZE_0, BIT_SIZE_16}},
    {{REG_BP, BIT_SIZE_0, BIT_SIZE_16}, {REG_DI, BIT_SIZE_0, BIT_SIZE_16}},
    {{REG_SI, BIT_SIZE_0, BIT_SIZE_16}, {REG_NONE, BIT_SIZE_0, BIT_SIZE_0}},
    {{REG_DI, BIT_SIZE_0, BIT_SIZE_16}, {REG_NONE, BIT_SIZE_0, BIT_SIZE_0}},
    // exception when mod = 00
    {{REG_BP, BIT_SIZE_0, BIT_SIZE_16}, {REG_NONE, BIT_SIZE_0, BIT_SIZE_0}},
    {{REG_B, BIT_SIZE_0, BIT_SIZE_16}, {REG_NONE, BIT_SIZE_0, BIT_SIZE_0}},
};

const reg_t regs[8][2] = {
    {{REG_A, BIT_SIZE_0, BIT_SIZE_8}, {REG_A, BIT_SIZE_0, BIT_SIZE_16}},
    {{REG_C, BIT_SIZE_0, BIT_SIZE_8}, {REG_C, BIT_SIZE_0, BIT_SIZE_16}},
    {{REG_D, BIT_SIZE_0, BIT_SIZE_8}, {REG_D, BIT_SIZE_0, BIT_SIZE_16}},
    {{REG_B, BIT_SIZE_0, BIT_SIZE_8}, {REG_B, BIT_SIZE_0, BIT_SIZE_16}},
    {{REG_A, BIT_SIZE_8, BIT_SIZE_8}, {REG_SP, BIT_SIZE_0, BIT_SIZE_16}},
    {{REG_C, BIT_SIZE_8, BIT_SIZE_8}, {REG_BP, BIT_SIZE_0, BIT_SIZE_16}},
    {{REG_D, BIT_SIZE_8, BIT_SIZE_8}, {REG_SI, BIT_SIZE_0, BIT_SIZE_16}},
    {{REG_B, BIT_SIZE_8, BIT_SIZE_8}, {REG_DI, BIT_SIZE_0, BIT_SIZE_16}},
};

const reg_t seg_regs[4] = {{REG_ES, BIT_SIZE_0, BIT_SIZE_16},
                           {REG_CS, BIT_SIZE_0, BIT_SIZE_16},
                           {REG_SS, BIT_SIZE_0, BIT_SIZE_16},
                           {REG_DS, BIT_SIZE_0, BIT_SIZE_16}};

const char *get_reg_name(u8 i) { return reg_name[i]; }

const char *get_cond_jmp(u8 i) { return cond_jmp[i]; }

const char *get_loop_jmp(u8 i) { return loop_jmp[i]; }

const reg_t *get_regs(u8 i, u8 w) { return &regs[i][w]; }

const reg_t *get_eff_addr_exprs(u8 i, u8 w) { return &eff_addr_exprs[i][w]; }

const reg_t *get_seg_regs(u8 i) { return &seg_regs[i]; }
