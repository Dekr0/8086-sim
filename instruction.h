#ifndef INSTRUCTION_8086_SIM_H
#define INSTRUCTION_8086_SIM_H

#include "type.h"

#define get_d(c) (c >> 6)
#define get_s(c) (c >> 5 & 1)
#define get_v(c) (c >> 4 & 1)
#define get_w(c) (c >> 3 & 1)
#define get_z(c) (c >> 2 & 1)
#define get_m(c) (c & 0b11)

#define set_d(c, b) c |= (b) << 6
#define set_s(c, b) c |= (b) << 5
#define set_v(c, b) c |= (b) << 4
#define set_w(c, b) c |= (b) << 3
#define set_z(c, b) c |= (b) << 2
#define set_m(c, b) c |= (b) & 0b11

#define BIT_MASK_8_LO 0x00ff
#define BIT_MASK_8_HI 0xff00
#define BIT_MASK_16 0xffff

#define MAX_OPERAND 2
#define BYTE_BIT_LENGTH 8

#define decode_cond_jump_variant(raw_ctrl_bits) raw_ctrl_bits & 0b1111
#define decode_loop_variant(raw_ctrl_bits) raw_ctrl_bits & 0b11

typedef enum mod_e : u8 { MOD_00, MOD_01, MOD_10, MOD_11 } mod_e;

typedef enum opcode_e : u32 {
    OPCODE_NONE,
    OPCODE_MOV_REG_MEM_REG,
    OPCODE_MOV_IMM_REG_MEM,
    OPCODE_MOV_IMM_REG,
    OPCODE_MOV_ACC,
    OPCODE_MOV_REG_MEM_SEG,
    OPCODE_ADD_REG_MEM_REG,
    OPCODE_ADD_IMM_REG_MEM,
    OPCODE_ADD_IMM_ACC,
    OPCODE_SUB_REG_MEM_REG,
    OPCODE_SUB_IMM_REG_MEM,
    OPCODE_SUB_IMM_ACC,
    OPCODE_CMP_REG_MEM_REG,
    OPCODE_CMP_IMM_REG_MEM,
    OPCODE_CMP_IMM_ACC,
    OPCODE_COND_JMP,
    OPCODE_LOOP_JMP
} opcode_e;

typedef enum cond_jmp_e : u8 {
    COND_JMP_O,
    COND_JMP_NO,
    COND_JMP_B,
    COND_JMP_NB,
    COND_JMP_E,
    COND_JMP_NE,
    COND_JMP_BE,
    COND_JMP_NBE,
    COND_JMP_S,
    COND_JMP_NS,
    COND_JMP_P,
    COND_JMP_NP,
    COND_JMP_L,
    COND_JMP_NL,
    COND_JMP_LE,
    COND_JMP_NLE
} cond_jmp_e;

typedef enum prefix_e : u8 { PREFIX_NONE, PREFIX_EXPLICIT_SIZE } prefix_e;

typedef enum operand_type_e : u8 {
    OPERAND_NONE,
    OPERAND_REG,
    OPERAND_EFF_ADDR_EXPR,
    OPERAND_WORD
} operand_type_e;

typedef enum reg_e : u8 {
    REG_NONE,
    REG_A,
    REG_C,
    REG_D,
    REG_B,
    REG_SP,
    REG_BP,
    REG_SI,
    REG_DI,
    REG_ES,
    REG_CS,
    REG_SS,
    REG_DS,
    REG_IP
} reg_e;

typedef enum word_type_e : u8 {
    WORD_TYPE_NONE,
    WORD_TYPE_ADDR,
    WORD_TYPE_DISP,
    WORD_TYPE_IMM
} word_type_e;

typedef struct reg_t {
    reg_e reg;
    bit_width_e offset; // low / high byte
    bit_width_e length; // measured in byte
} reg_t;

typedef struct word_t {
    u8 sign;
    u32 value;
    word_type_e type;
    bit_width_e width;
} word_t;

typedef struct eff_addr_expr_t {
    u8 da;
    word_t disp;
    const reg_t *terms[2];
} eff_addr_expr_t;

typedef struct operand_t {
    operand_type_e type;
    union {
        eff_addr_expr_t expr;
        word_t word;
        const reg_t *reg;
    };
} operand_t;

typedef struct instr_t {
    // 6 5 4 3 2 1   0
    // d s v w z m o d
    u8 ctrl_bits;
    u8 raw_ctrl_bits1;
    u8 raw_ctrl_bits2;
    u8 is_jmp_dest;
    u32 base_addr;
    u32 size;
    u32 machine_code;
    struct instr_t *prev;
    struct instr_t *next;
    prefix_e prefix;
    opcode_e opcode;
    operand_t operands[MAX_OPERAND];
} instr_t;

void deep_copy_instr(instr_t *dest, const instr_t *src);

void deep_copy_operand(operand_t *dest, const operand_t *src);

i8 deep_cmp_instr(const instr_t *dest, const instr_t *src);

i8 deep_cmp_operand(const operand_t *dest, const operand_t *src);

void clear_instr(instr_t *i);

void clear_operand(operand_t *o);

#endif
