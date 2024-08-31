#include <assert.h>
#include <stdlib.h>

#include "decoder.h"
#include "hashtable.h"
#include "instruction.h"
#include "table.h"
#include "type.h"
#include "vector.h"

#define unpack_b16_to_int(lo, hi) (i32) hi << 8 | (i32)lo

typedef struct decoder_context_t {
    u32 *IP; // Keep track of where decoder is at in the memory region
    memory_t *mem;
} decoder_context_t;

static opcode_e decode_opcode(u8 b1, u8 b2);

static i32 decode_word(decoder_context_t *dc, word_t *w);

static i32 decode_imm(const u8 c, decoder_context_t *dc, operand_t *o);

static i32 decode_eff_addr_expr(const u8 c, decoder_context_t *dc,
                                operand_t *o);

static i32 decode_mod_rsm(const u8 c, decoder_context_t *dc, operand_t *o);

static i32 decode_mod_reg_rsm(decoder_context_t *dc, instr_t *instr_t);

static i32 decode_imm_mod_reg_mem(decoder_context_t *dc, instr_t *instr_t);

static i32 decode_imm_reg(decoder_context_t *dc, instr_t *instr_t);

static i32 decode_acc_mem(decoder_context_t *dc, instr_t *instr_t);

static i32 decode_acc_imm(decoder_context_t *dc, instr_t *instr_t);

static i32 label_jmp(u32 jmp, instr_t *i_arr, const u32 size);

static opcode_e decode_opcode(u8 b1, u8 b2) {
    if (b1 >> 2 == 0b100010)
        return OPCODE_MOV_REG_MEM_REG;
    if (b1 >> 1 == 0b1100011)
        return OPCODE_MOV_IMM_REG_MEM;
    if (b1 >> 4 == 0b1011)
        return OPCODE_MOV_IMM_REG;
    if (b1 >> 1 == 0b1010000 || b1 >> 1 == 0b1010001)
        return OPCODE_MOV_ACC;
    if (b1 == 0b10001110 || b1 == 0b10001100)
        return OPCODE_MOV_REG_MEM_SEG;
    if (b1 >> 2 == 0b000000)
        return OPCODE_ADD_REG_MEM_REG;
    if (b1 >> 2 == 0b100000 && (b2 >> 3 & 0b111) == 0)
        return OPCODE_ADD_IMM_REG_MEM;
    if (b1 >> 1 == 0b0000010)
        return OPCODE_ADD_IMM_ACC;
    if (b1 >> 2 == 0b001010)
        return OPCODE_SUB_REG_MEM_REG;
    if (b1 >> 2 == 0b100000 && (b2 >> 3 & 0b111) == 0b101)
        return OPCODE_SUB_IMM_REG_MEM;
    if (b1 >> 1 == 0b0010110)
        return OPCODE_SUB_IMM_ACC;
    if (b1 >> 2 == 0b001110)
        return OPCODE_CMP_REG_MEM_REG;
    if (b1 >> 2 == 0b100000 && (b2 >> 3 & 0b111) == 0b111)
        return OPCODE_CMP_IMM_REG_MEM;
    if (b1 >> 1 == 0b0011110)
        return OPCODE_CMP_IMM_ACC;
    if ((b1 & 0b11110000) == 0b01110000)
        return OPCODE_COND_JMP;
    if ((b1 & 0b11110000) == 0b11100000)
        return OPCODE_LOOP_JMP;
    return OPCODE_NONE;
}

static i32 decode_word(decoder_context_t *dc, word_t *w) {
    u8 *mem = dc->mem->mem;
    u32 *IP = dc->IP;
    switch (w->width) {
    case BIT_SIZE_16:
        if (*IP + 1 > dc->mem->source_end) {
            fprintf(stderr,
                    "decode_word: decoder is out of source code bound\n");
            return -1;
        }
        w->value = unpack_b16_to_int(mem[*IP], mem[*IP + 1]);
        w->sign = w->value >> 15;
        *IP += 2;
        return 0;
    case BIT_SIZE_8:
        if (*IP > dc->mem->source_end) {
            fprintf(stderr,
                    "decode_word: decoder is out of source code bound\n");
            return -1;
        }
        w->value = (u8)mem[*IP];
        w->sign = w->value >> 7;
        *IP += 1;
        return 0;
    case BIT_SIZE_0:
        return 0;
    }
}

static i32 decode_imm(const u8 c, decoder_context_t *dc, operand_t *o) {
    const u8 s = get_s(c);
    const u8 w = get_w(c);
    u8 *mem = dc->mem->mem;
    u32 *IP = dc->IP;
    o->type = OPERAND_WORD;
    o->word.type = WORD_TYPE_IMM;
    if (!s && w)
        o->word.width = BIT_SIZE_16;
    else
        o->word.width = BIT_SIZE_8;
    return decode_word(dc, &o->word);
}

static i32 decode_eff_addr_expr(const u8 c, decoder_context_t *dc,
                                operand_t *o) {
    u8 *mem = dc->mem->mem;
    u32 *IP = dc->IP;
    const u8 m = get_m(c);
    const u8 rsm = mem[*IP + 1] & 7;
    o->type = OPERAND_EFF_ADDR_EXPR;
    o->expr.terms[0] = get_eff_addr_exprs(rsm, 0);
    o->expr.terms[1] = get_eff_addr_exprs(rsm, 1);
    *IP += 2;
    switch (m) {
    case MOD_11:
        fprintf(stderr,
                "decode_eff_add_expr: mod = 11 in effective address expression\
                decoding?\n");
        return -1;
    case MOD_10:
        o->expr.disp.type = WORD_TYPE_DISP;
        o->expr.disp.width = BIT_SIZE_16;
        break;
    case MOD_01:
        o->expr.disp.type = WORD_TYPE_DISP;
        o->expr.disp.width = BIT_SIZE_8;
        break;
    case MOD_00:
        if (rsm != 6)
            break;
        o->expr.da = 1;
        o->expr.disp.type = WORD_TYPE_DISP;
        o->expr.disp.width = BIT_SIZE_16;
        break;
    }
    return decode_word(dc, &o->expr.disp);
}

static i32 decode_mod_rsm(const u8 c, decoder_context_t *dc, operand_t *o) {
    const u8 m = get_m(c);
    const u8 w = get_w(c);
    u8 *mem = dc->mem->mem;
    u32 *IP = dc->IP;
    if (m != MOD_11)
        return decode_eff_addr_expr(c, dc, o);
    o->type = OPERAND_REG;
    o->reg = get_regs(mem[*IP + 1] & 7, w);
    *IP += 2;
    return 0;
}

static i32 decode_mod_reg_rsm(decoder_context_t *dc, instr_t *instr) {
    u8 *mem = dc->mem->mem;
    u32 *IP = dc->IP;
    set_m(instr->ctrl_bits, mem[*IP + 1] >> 6 & 3);
    instr->operands[0].type = OPERAND_REG;
    instr->operands[0].reg =
        get_regs(mem[*IP + 1] >> 3 & 7, get_w(instr->ctrl_bits));
    return decode_mod_rsm(instr->ctrl_bits, dc, &instr->operands[1]);
}

static int decode_imm_mod_reg_mem(decoder_context_t *dc, instr_t *instr) {
    u8 *mem = dc->mem->mem;
    u32 *IP = dc->IP;
    const u8 m = mem[*IP + 1] >> 6 & 3;
    set_m(instr->ctrl_bits, m);
    if (m != MOD_11)
        instr->prefix = PREFIX_EXPLICIT_SIZE;
    return decode_mod_rsm(instr->ctrl_bits, dc, &instr->operands[1]) != -1 &&
           decode_imm(instr->ctrl_bits, dc, &instr->operands[0]);
}

static i32 decode_imm_reg(decoder_context_t *dc, instr_t *instr) {
    u8 *mem = dc->mem->mem;
    u32 *IP = dc->IP;
    instr->operands[1].type = OPERAND_REG;
    instr->operands[1].reg = get_regs(mem[*IP] & 7, get_w(instr->ctrl_bits));
    *IP += 1;
    return decode_imm(instr->ctrl_bits, dc, &instr->operands[0]);
}

static i32 decode_acc_mem(decoder_context_t *dc, instr_t *instr) {
    const u8 w = get_w(instr->ctrl_bits);
    instr->operands[0].type = OPERAND_REG;
    instr->operands[0].reg = get_regs(0, w);
    instr->operands[1].type = OPERAND_WORD;
    instr->operands[1].word.type = WORD_TYPE_ADDR;
    instr->operands[1].word.width = w ? BIT_SIZE_16 : BIT_SIZE_8;
    *dc->IP += 1;
    return decode_word(dc, &instr->operands[1].word);
}

static i32 decode_acc_imm(decoder_context_t *dc, instr_t *instr) {
    const u8 w = get_w(instr->ctrl_bits);
    instr->operands[0].type = OPERAND_REG;
    instr->operands[0].reg = get_regs(0, w);
    instr->operands[1].type = OPERAND_WORD;
    instr->operands[1].word.type = WORD_TYPE_IMM;
    instr->operands[1].word.width = w ? BIT_SIZE_16 : BIT_SIZE_8;
    *dc->IP += 1;
    return decode_word(dc, &instr->operands[1].word);
}

hashmap_instr_t *load_instr_stream(memory_t *mem_t) {
    u8 b1 = 0;
    u8 b2 = 0;
    u8 *mem = mem_t->mem;
    u32 IP = 0;
    opcode_e opcode = OPCODE_NONE;
    decoder_context_t *dc = calloc(1, sizeof(decoder_context_t));
    if (dc == NULL) {
        fprintf(stderr, "decoder_context_t calloc error\n");
        return NULL;
    }
    dc->IP = &IP;
    dc->mem = mem_t;

    vector_instr_p_t *jmp_instrs = init_vector_instr_p(2);
    if (jmp_instrs == NULL) {
        fprintf(stderr, "init_vector_instr error\n");
        goto Abort;
    }
    hashmap_instr_t *instr_stream = init_hashmap_instr();
    if (instr_stream == NULL) {
        fprintf(stderr, "init_hashmap_instr error\n");
        goto Abort;
    }

    instr_t *instr = calloc(1, sizeof(instr_t));
    if (instr == NULL) {
        fprintf(stderr, "instr_t calloc error\n");
        goto Abort;
    }

    for (IP = 0; IP + 1 < mem_t->source_end;) {
        clear_instr(instr);
        instr->base_addr = IP;
        instr->raw_ctrl_bits1 = mem[IP];
        instr->raw_ctrl_bits2 = mem[IP + 1];
        if ((opcode = decode_opcode(mem[IP], mem[IP + 1])) == OPCODE_NONE) {
            fprintf(stderr, "Opcode is unknown for byte at address %d\n", IP);
            fprintf(stderr, "Abort instruction decoding\n");
            goto Abort;
        }
        instr->opcode = opcode;

        /**
         * 1.Extract all control bits (d, s, v, w, z)
         * 2.Extract other relevant fields (mod, reg, rsm) in the specific
         * function call
         * */
        switch (instr->opcode) {
        case OPCODE_NONE:
            goto Abort;
            break;
        case OPCODE_MOV_REG_MEM_REG:
            set_d(instr->ctrl_bits, mem[IP] >> 1 & 1);
            set_w(instr->ctrl_bits, mem[IP] & 1);
            if (decode_mod_reg_rsm(dc, instr) == -1)
                goto Abort;
            instr->size = IP - instr->base_addr;
            break;
        case OPCODE_MOV_IMM_REG_MEM:
            set_w(instr->ctrl_bits, mem[IP] & 1);
            if (decode_imm_mod_reg_mem(dc, instr) == -1)
                goto Abort;
            instr->size = IP - instr->base_addr;
            break;
        case OPCODE_MOV_IMM_REG:
            set_w(instr->ctrl_bits, mem[IP] >> 3 & 1);
            if (decode_imm_reg(dc, instr) == -1)
                goto Abort;
            instr->size = IP - instr->base_addr;
            break;
        case OPCODE_MOV_ACC:
            set_d(instr->ctrl_bits, !(mem[IP] >> 1 & 1));
            set_w(instr->ctrl_bits, mem[IP] & 1);
            if (decode_acc_mem(dc, instr) == -1)
                goto Abort;
            instr->size = IP - instr->base_addr;
            break;
        case OPCODE_MOV_REG_MEM_SEG:
            set_d(instr->ctrl_bits, mem[IP] >> 1 & 1);
            set_w(instr->ctrl_bits, 1);
            set_m(instr->ctrl_bits, mem[IP + 1] >> 6 & 3);
            instr->operands[0].type = OPERAND_REG;
            instr->operands[0].reg = get_seg_regs(mem[IP + 1] >> 3 & 3);
            if (decode_mod_rsm(instr->ctrl_bits, dc, &instr->operands[1]) == -1)
                goto Abort;
            instr->size = IP - instr->base_addr;
            break;
        case OPCODE_ADD_REG_MEM_REG:
            set_d(instr->ctrl_bits, mem[IP] >> 1 & 1);
            set_w(instr->ctrl_bits, mem[IP] & 1);
            if (decode_mod_reg_rsm(dc, instr) == -1)
                goto Abort;
            instr->size = IP - instr->base_addr;
            break;
        case OPCODE_ADD_IMM_REG_MEM:
            set_s(instr->ctrl_bits, mem[IP] >> 1 & 1);
            set_w(instr->ctrl_bits, mem[IP] & 1);
            if (decode_imm_mod_reg_mem(dc, instr) == -1)
                goto Abort;
            instr->size = IP - instr->base_addr;
            break;
        case OPCODE_ADD_IMM_ACC:
            set_d(instr->ctrl_bits, 1);
            set_w(instr->ctrl_bits, mem[IP] & 1);
            if (decode_acc_imm(dc, instr) == -1)
                goto Abort;
            instr->size = IP - instr->base_addr;
            break;
        case OPCODE_SUB_REG_MEM_REG:
            set_d(instr->ctrl_bits, mem[IP] >> 1 & 1);
            set_w(instr->ctrl_bits, mem[IP] & 1);
            if (decode_mod_reg_rsm(dc, instr) == -1)
                goto Abort;
            instr->size = IP - instr->base_addr;
            break;
        case OPCODE_SUB_IMM_REG_MEM:
            set_s(instr->ctrl_bits, mem[IP] >> 1 & 1);
            set_w(instr->ctrl_bits, mem[IP] & 1);
            if (decode_imm_mod_reg_mem(dc, instr) == -1)
                goto Abort;
            instr->size = IP - instr->base_addr;
            break;
        case OPCODE_SUB_IMM_ACC:
            set_d(instr->ctrl_bits, 1);
            set_w(instr->ctrl_bits, mem[IP] & 1);
            if (decode_acc_imm(dc, instr) == -1)
                goto Abort;
            instr->size = IP - instr->base_addr;
            break;
        case OPCODE_CMP_REG_MEM_REG:
            set_d(instr->ctrl_bits, mem[IP] >> 1 & 1);
            set_w(instr->ctrl_bits, mem[IP] & 1);
            if (decode_mod_reg_rsm(dc, instr) == -1)
                goto Abort;
            instr->size = IP - instr->base_addr;
            break;
        case OPCODE_CMP_IMM_REG_MEM:
            set_s(instr->ctrl_bits, mem[IP] >> 1 & 1);
            set_w(instr->ctrl_bits, mem[IP] & 1);
            if (decode_imm_mod_reg_mem(dc, instr) == -1)
                goto Abort;
            instr->size = IP - instr->base_addr;
            break;
        case OPCODE_CMP_IMM_ACC:
            set_d(instr->ctrl_bits, 1);
            set_w(instr->ctrl_bits, mem[IP] & 1);
            if (decode_acc_imm(dc, instr) == -1)
                goto Abort;
            instr->size = IP - instr->base_addr;
            break;
        case OPCODE_COND_JMP:
            instr->operands[0].type = OPERAND_WORD;
            instr->operands[0].word.type = WORD_TYPE_IMM;
            instr->operands[0].word.width = BIT_SIZE_8;
            instr->operands[0].word.value = (i8)mem[IP + 1];
            IP += 2;
            instr->size = IP - instr->base_addr;
            assert(hashmap_instr_put(instr_stream, *instr) != -1);
            vector_instr_p_append(
                jmp_instrs, hashmap_instr_get(instr_stream, instr->base_addr));
            continue;
        case OPCODE_LOOP_JMP:
            instr->operands[0].type = OPERAND_WORD;
            instr->operands[0].word.type = WORD_TYPE_IMM;
            instr->operands[0].word.width = BIT_SIZE_8;
            instr->operands[0].word.value = (i8)mem[IP + 1];
            IP += 2;
            instr->size = IP - instr->base_addr;
            assert(hashmap_instr_put(instr_stream, *instr) != -1);
            vector_instr_p_append(
                jmp_instrs, hashmap_instr_get(instr_stream, instr->base_addr));
            continue;
        }
        assert(hashmap_instr_put(instr_stream, *instr) != -1);
    }

    const instr_t *forward_jmp_instr = NULL;
    instr_t *jmp_dest_instr = NULL;
    for (u32 k = 0; k < vector_instr_p_size(jmp_instrs); k++) {
        assert((forward_jmp_instr = vector_instr_p_get(jmp_instrs, k)) != NULL);
        assert((jmp_dest_instr = hashmap_instr_get(
                    instr_stream,
                    forward_jmp_instr->base_addr + forward_jmp_instr->size +
                        forward_jmp_instr->operands[0].word.value)) != NULL);
        jmp_dest_instr->is_jmp_dest = 1;
    }

    return instr_stream;
Abort:
    if (instr_stream != NULL)
        free_hashmap_instr(instr_stream);
    if (jmp_instrs != NULL)
        free_vector_instr_p(jmp_instrs);
    if (instr != NULL)
        free(instr);
    if (dc != NULL)
        free(dc);
    return NULL;
}
