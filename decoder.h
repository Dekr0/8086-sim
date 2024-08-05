#ifndef DECODER_8086_SIM_H
#define DECODER_8086_SIM_H

#include "instruction.h"
#include "memory.h"


typedef struct decoder_context_t {
    u32      *a; // Keep track of where decoder is at in the memory region
    memory_t *mem;
} decoder_context_t;

static opcode_e decode_opcode(u8 b1, u8 b2);

static i32 decode_word(decoder_context_t *dc, word_t *w);

static i32 decode_imm(const u8 c, decoder_context_t *dc, operand_t *o);

static i32 decode_eff_addr_expr(const u8 c, decoder_context_t *dc, operand_t *o);

static i32 decode_mod_rsm(const u8 c, decoder_context_t *dc, operand_t *o);

static i32 decode_mod_reg_rsm(decoder_context_t *dc, instruction_t *i);

static i32 decode_imm_mod_reg_mem(decoder_context_t *dc, instruction_t *i);

static i32 decode_imm_reg(decoder_context_t *dc, instruction_t *i);

static i32 decode_acc_mem(decoder_context_t *dc, instruction_t *i);

static i32 decode_acc_imm(decoder_context_t *dc, instruction_t *i);

static i32 label_jmp(u32 jmp, instruction_t *i_arr, const u32 size);

src_instructions_t *load_instruction(memory_t *mem_t);

static i32 print_word(word_t *w);

static i32 print_eff_addr_expr(eff_addr_expr_t *e);

static i32 print_reg(reg_t *r);

static i32 print_operand(operand_t *o);

static void print_jmp(instruction_t *in);

void print_instruction(instruction_t *in);


#endif