#ifndef INSTRUCTION_8086_SIM_H
#define INSTRUCTION_8086_SIM_H

#include "type.h"

#define get_d(c) (c >> 7)
#define get_s(c) (c >> 6 & 1)
#define get_w(c) (c >> 4 & 1)
#define get_m(c) (c & 3)

#define set_d(c, b) c |= (b) << 7
#define set_s(c, b) c |= (b) << 6
#define set_w(c, b) c |= (b) << 4
#define set_m(c, b) c |= b 

#define BIT_MASK_8_LO 0x00ff
#define BIT_MASK_8_HI 0xff00
#define BIT_MASK_16   0xffff

#define MAX_OPERAND     2
#define BYTE_BIT_LENGTH 8

typedef enum mod_e: u8 {
    MOD_00,
    MOD_01,
    MOD_10,
    MOD_11
} mod_e;

typedef enum opcode_e: u32 {
    OP_NONE,
    MOV_REG_MEM_REG,
    MOV_IMM_REG_MEM,
    MOV_IMM_REG,
    MOV_ACC,
    MOV_REG_MEM_SEG,
    ADD_REG_MEM_REG,
    ADD_IMM_REG_MEM,
    ADD_IMM_ACC,
    SUB_REG_MEM_REG,
    SUB_IMM_REG_MEM,
    SUB_IMM_ACC,
    CMP_REG_MEM_REG,
    CMP_IMM_REG_MEM,
    CMP_IMM_ACC,
    COND_JMP,
    LOOP_JMP
} opcode_e;

typedef enum prefix_e: u8 {
    PREFIX_NONE,
    PREFIX_EXPLICIT_SIZE
} prefix_e;

typedef enum operand_type_e: u8 {
    OPERAND_NONE,
    OPERAND_REG,
    OPERAND_EFF_ADDR_EXPR,
    OPERAND_WORD
} operand_type_e;

typedef enum reg_e: u8 {
    REG_NONE, REG_A,  REG_C,  REG_D, REG_B, REG_SP, REG_BP, REG_SI, REG_DI,
    REG_ES,   REG_CS, REG_SS, REG_DS
} reg_e;

typedef enum word_type_e: u8 {
    WORD_TYPE_NONE,
    WORD_TYPE_ADDR,
    WORD_TYPE_DISP,
    WORD_TYPE_IMM
} word_type_e;

typedef struct reg_t {
    reg_e       reg;
    bit_width_e offset; // low / high byte
    bit_width_e length; // measured in byte
} reg_t;

typedef struct word_t {
    i32         value;
    word_type_e type;
    bit_width_e width;
} word_t;

typedef struct eff_addr_expr_t {
    u8            da;
    word_t        disp;
    const reg_t  *terms[2];
} eff_addr_expr_t;


typedef struct operand_t {
    operand_type_e type;
    union {
        eff_addr_expr_t expr;
        word_t          word;
        const reg_t     *reg;
    };
} operand_t;

typedef struct instruction_t {
    // 0 1 2 3 4 5 6 7
    // d s v w z m o d
    u8        ctrl_bits; 
    u8        raw_ctrl_bits1;
    u8        raw_ctrl_bits2;
    u8        is_jmp_dest;
    u32       base_addr;
    u32       size;
    prefix_e  prefix;
    opcode_e  opcode;
    operand_t operands[MAX_OPERAND];
} instruction_t;

typedef struct src_instructions_t {
    u32           count;
    instruction_t *i_arr;
} src_instructions_t;

src_instructions_t *init_src_instruction(instruction_t *i, u32 count);

void free_src_instruction(src_instructions_t *src);


#endif
