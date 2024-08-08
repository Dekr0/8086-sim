#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "decoder.h"
#include "table.h"
#include "type.h"

#define unpack_b16_to_int(lo, hi) (i32) hi << 8 | (i32) lo
#define decode_jump_variant(raw_ctrl_bits) raw_ctrl_bits & 0b1111
#define decode_loop_variant(raw_ctrl_bits) raw_ctrl_bits & 0b11


static opcode_e decode_opcode(u8 b1, u8 b2) {
    if (b1 >> 2 == 0b100010 ) { return MOV_REG_MEM_REG; }
    if (b1 >> 1 == 0b1100011) { return MOV_IMM_REG_MEM; }
    if (b1 >> 4 == 0b1011   ) { return MOV_IMM_REG; }
    if (b1 >> 2 == 0b101000 ) { return MOV_ACC; }
    if (b1 >> 2 == 0b100011 ) { return MOV_REG_MEM_SEG; }

    if (b1 >> 2 == 0   ) { return ADD_REG_MEM_REG; }
    if (b1 >> 2 == 0b100000 && (b2 >> 3 & 0b111) == 0) { return ADD_IMM_REG_MEM; }
    if (b1 >> 1 == 0b10) { return ADD_IMM_ACC; }

    if (b1 >> 2 == 0b001010 ) { return SUB_REG_MEM_REG; }
    if (b1 >> 2 == 0b100000 && (b2 >> 3 & 0b111) == 0b101) { return SUB_IMM_REG_MEM; }
    if (b1 >> 1 == 0b0010110) { return SUB_IMM_ACC; }

    if (b1 >> 2 == 0b001110 ) { return CMP_REG_MEM_REG; }
    if (b1 >> 2 == 0b100000 && (b2 >> 3 & 0b111) == 0b111) { return CMP_IMM_REG_MEM; }
    if (b1 >> 1 == 0b0011110) { return CMP_IMM_ACC; }

    if ((b1 & 0b11110000) == 0b01110000) { return COND_JMP; }
    if ((b1 & 0b11110000) == 0b11100000) { return LOOP_JMP; }

    return OP_NONE;
}

static i32 decode_word(decoder_context_t *dc, word_t *w) {
    u8  *mem = dc->mem->mem;
    u32 *a   = dc->a;
    switch (w->width) {
    case BIT_SIZE_16:
        if (*a + 1 > dc->mem->src_size) {
            printf("decode_word: decoder is out of source code bound\n");
            return -1;
        }
        w->value = unpack_b16_to_int(mem[*a], mem[*a + 1]);
        *a += 2;
        break;
    case BIT_SIZE_8:
        if (*a > dc->mem->src_size) {
            printf("decode_word: decoder is out of source code bound\n");
            return -1;
        }
        w->value = (u8) mem[*a];
        *a += 1;
        break;
    case BIT_SIZE_0:
        break;
    }
    return 0;
}

static i32 decode_imm(const u8 c, decoder_context_t *dc, operand_t *o) {
    const u8 s = get_s(c);
    const u8 w = get_w(c);
    u8  *mem = dc->mem->mem;
    u32 *a   = dc->a;
    o->type = OPERAND_WORD;
    o->word.type = WORD_TYPE_IMM;
    if (!s && w) {
        o->word.width = BIT_SIZE_16;
    } else {
        o->word.width = BIT_SIZE_8;
    }
    return decode_word(dc, &o->word);
}

static i32 decode_eff_addr_expr(const u8 c, decoder_context_t *dc, operand_t *o) {
    u8  *mem = dc->mem->mem;
    u32 *a   = dc->a;
    const u8 m = get_m(c);
    const u8 rsm = mem[*a + 1] & 7;
    o->type = OPERAND_EFF_ADDR_EXPR;
    o->expr.terms[0] = get_eff_addr_exprs(rsm, 0);
    o->expr.terms[1] = get_eff_addr_exprs(rsm, 1);
    *a += 2;
    switch (m) {
    case MOD_11:
        printf("decode_eff_add_expr: mod = 11 in effective address expression\
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
        if (rsm == 6) { 
            o->expr.da = 1;
            o->expr.disp.type = WORD_TYPE_DISP;
            o->expr.disp.width = BIT_SIZE_16;
        }
        break;
    }
    return decode_word(dc, &o->expr.disp);
}

static i32 decode_mod_rsm(const u8 c, decoder_context_t *dc, operand_t *o) {
    const u8 m = get_m(c);
    const u8 w = get_w(c);
    u8  *mem = dc->mem->mem;
    u32 *a   = dc->a;

    if (m == MOD_11) {
        o->type = OPERAND_REG;
        o->reg = get_regs(mem[*a + 1] & 7, w);
        *a += 2;
        return 0;
    }

    return decode_eff_addr_expr(c, dc, o);
}

static i32 decode_mod_reg_rsm(decoder_context_t *dc, instruction_t *i) {
    u8  *mem = dc->mem->mem;
    u32 *a   = dc->a;
    set_m(i->ctrl_bits, mem[*a + 1] >> 6 & 3);
    i->operands[0].type = OPERAND_REG;
    i->operands[0].reg = get_regs(mem[*a + 1] >> 3 & 7, get_w(i->ctrl_bits));
    if (decode_mod_rsm(i->ctrl_bits, dc, &i->operands[1]) == -1) 
    {
        return -1;
    }
    return 0;
}

static int decode_imm_mod_reg_mem(decoder_context_t *dc, instruction_t *i) {
    u8  *mem = dc->mem->mem;
    u32 *a   = dc->a;
    const u8 m = mem[*a + 1] >> 6 & 3;
    set_m(i->ctrl_bits, m);
    if (m != MOD_11) {
        i->prefix = PREFIX_EXPLICIT_SIZE;
    }
    if (decode_mod_rsm(i->ctrl_bits, dc, &i->operands[1]) 
            == -1)
    {
        return -1;
    }
    return decode_imm(i->ctrl_bits, dc, &i->operands[0]);
}

static i32 decode_imm_reg(decoder_context_t *dc, instruction_t *i) {
    u8  *mem = dc->mem->mem;
    u32 *a   = dc->a;
    i->operands[1].type = OPERAND_REG;
    i->operands[1].reg  = get_regs(mem[*a] & 7, get_w(i->ctrl_bits));
    *a += 1;
    return decode_imm(i->ctrl_bits, dc, &i->operands[0]);
}

static i32 decode_acc_mem(decoder_context_t *dc, instruction_t *i) {
    const u8 w = get_w(i->ctrl_bits);
    i->operands[0].type = OPERAND_REG;
    i->operands[0].reg = get_regs(0, w);
    i->operands[1].type = OPERAND_WORD;
    i->operands[1].word.type = WORD_TYPE_ADDR;
    i->operands[1].word.width = w ? BIT_SIZE_16 : BIT_SIZE_8;
    *dc->a += 1;
    return decode_word(dc, &i->operands[1].word);
}

static i32 decode_acc_imm(decoder_context_t *dc, instruction_t *i) {
    const u8 w = get_w(i->ctrl_bits);
    i->operands[0].type = OPERAND_REG;
    i->operands[0].reg = get_regs(0, w);
    i->operands[1].type = OPERAND_WORD;
    i->operands[1].word.type = WORD_TYPE_IMM;
    i->operands[1].word.width = w ? BIT_SIZE_16 : BIT_SIZE_8;
    *dc->a += 1;
    return decode_word(dc, &i->operands[1].word);
}

static i32 label_jmp(u32 jmp, instruction_t *i_arr, const u32 size) {
    i32 lo = 0;
    i32 hi = size;
    do {
        i32 mid = (hi + lo) / 2;
        if (i_arr[mid].base_addr == jmp) {
            i_arr[mid].is_jmp_dest = 1;
            return 0;
        }
        if (jmp > i_arr[mid].base_addr) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    } while (lo < hi);
    return 1;
}

src_instructions_t *load_instruction(memory_t *mem_t) {
    u8 b1 = 0;
    u8 b2 = 0;
    u8 abort = 0;
    u8 *mem = mem_t->mem;
    u32 a = 0;
    opcode_e opcode = OP_NONE;
    decoder_context_t dc = { &a, mem_t };

    instruction_t *i_arr = calloc(mem_t->src_size, sizeof( instruction_t ));
    if (i_arr == NULL) { return NULL; }
    instruction_t **j_arr = calloc(mem_t->src_size, sizeof( instruction_t *));
    if (j_arr == NULL) { return NULL; }
    instruction_t *i = NULL;
    u32 ni = 0;
    u32 nj = 0;

    printf("bits 16\n");

    for (a = 0; a + 1 < mem_t->src_size; ) {
        i = i_arr + ni++;

        i->base_addr = a;
        i->raw_ctrl_bits1 = mem[a];
        i->raw_ctrl_bits2 = mem[a + 1];
        if ((opcode = decode_opcode(mem[a], mem[a + 1])) == 
                OP_NONE) {
            printf("Opcode is unknown for byte at address %d\n", a);
            break;
        }
        i->opcode = opcode;

        /** 
         * 1.Extract all control bits (d, s, v, w, z) 
         * 2.Extract other relevant fields (mod, reg, rsm) in the specific 
         * function call
         * */
        switch (i->opcode) {
        case OP_NONE:
            abort = 1;
            break;
        case MOV_REG_MEM_REG: 
            set_d(i->ctrl_bits, mem[a] >> 1 & 1);
            set_w(i->ctrl_bits, mem[a] & 1);
            if (decode_mod_reg_rsm(&dc, i) == -1) { 
                abort = 1;
                break;
            }
            i->size = a - i->base_addr;
            break;
        case MOV_IMM_REG_MEM:
            set_w(i->ctrl_bits, mem[a] & 1);
            if (decode_imm_mod_reg_mem(&dc, i) == -1) { 
                abort = 1;
                break;
            }
            i->size = a - i->base_addr;
            break;
        case MOV_IMM_REG:
            set_w(i->ctrl_bits, mem[a] >> 3 & 1);
            if (decode_imm_reg(&dc, i) == -1) {
                abort = 1;
                break;
            }
            i->size = a - i->base_addr;
            break;
        case MOV_ACC:
            set_d(i->ctrl_bits, !(mem[a] >> 1 & 1));
            set_w(i->ctrl_bits, mem[a] & 1);
            if (decode_acc_mem(&dc, i) == -1) {
                abort = 1;
                break;
            }
            i->size = a - i->base_addr;
            break;
        case MOV_REG_MEM_SEG:
            set_d(i->ctrl_bits, mem[a] >> 1 & 1);
            set_w(i->ctrl_bits, 1);
            set_m(i->ctrl_bits, mem[a + 1] >> 6 & 3);
            i->operands[0].type = OPERAND_REG;
            i->operands[0].reg  = get_seg_regs(mem[a + 1] >> 3 & 3);
            if (decode_mod_rsm(i->ctrl_bits, &dc, &i->operands[1]) == -1) { 
                abort = 1;
                break;
            }
            break;
        case ADD_REG_MEM_REG:
            set_d(i->ctrl_bits, mem[a] >> 1 & 1);
            set_w(i->ctrl_bits, mem[a] & 1);
            if (decode_mod_reg_rsm(&dc, i) == -1) {
                abort = 1;
                break;
            }
            i->size = a - i->base_addr;
            break;
        case ADD_IMM_REG_MEM:
            set_s(i->ctrl_bits, mem[a] >> 1 & 1);
            set_w(i->ctrl_bits, mem[a] & 1);
            if (decode_imm_mod_reg_mem(&dc, i) == -1) {
                abort = 1;
                break;
            }
            i->size = a - i->base_addr;
            break;
        case ADD_IMM_ACC:
            set_d(i->ctrl_bits, !(mem[a] >> 1 & 1));
            set_w(i->ctrl_bits, mem[a] & 1);
            if (decode_acc_imm(&dc, i) == -1) {
                abort = 1;
                break;
            }
            i->size = a - i->base_addr;
            break;
        case SUB_REG_MEM_REG:
            set_d(i->ctrl_bits, mem[a] >> 1 & 1);
            set_w(i->ctrl_bits, mem[a] & 1);
            if (decode_mod_reg_rsm(&dc, i) == -1) {
                abort = 1;
                break;
            }
            i->size = a - i->base_addr;
            break;
        case SUB_IMM_REG_MEM:
            set_s(i->ctrl_bits, mem[a] >> 1 & 1);
            set_w(i->ctrl_bits, mem[a] & 1);
            if (decode_imm_mod_reg_mem(&dc, i) == -1) {
                abort = 1;
                break;
            }
            i->size = a - i->base_addr;
            break;
        case SUB_IMM_ACC:
            set_d(i->ctrl_bits, !(mem[a] >> 1 & 1));
            set_w(i->ctrl_bits, mem[a] & 1);
            if (decode_acc_imm(&dc, i) == -1) {
                abort = 1;
                break;
            }
            i->size = a - i->base_addr;
            break;
        case CMP_REG_MEM_REG:
            set_d(i->ctrl_bits, mem[a] >> 1 & 1);
            set_w(i->ctrl_bits, mem[a] & 1);
            if (decode_mod_reg_rsm(&dc, i) == -1) {
                abort = 1;
                break;
            }
            i->size = a - i->base_addr;
            break;
        case CMP_IMM_REG_MEM:
            set_s(i->ctrl_bits, mem[a] >> 1 & 1);
            set_w(i->ctrl_bits, mem[a] & 1);
            if (decode_imm_mod_reg_mem(&dc, i) == -1) {
                abort = 1;
                break;
            }
            i->size = a - i->base_addr;
            break;
        case CMP_IMM_ACC:
            set_d(i->ctrl_bits, !(mem[a] >> 1 & 1));
            set_w(i->ctrl_bits, mem[a] & 1);
            if (decode_acc_imm(&dc, i) == -1) {
                abort = 1;
                break;
            }
            i->size = a - i->base_addr;
            break;
        case COND_JMP:
            i->operands[0].type = OPERAND_WORD;
            i->operands[0].word.type = WORD_TYPE_IMM;
            i->operands[0].word.width = BIT_SIZE_8;
            i->operands[0].word.value = (i8) mem[a + 1];
            a += 2;
            i->size = a - i->base_addr;
            j_arr[nj++] = i;
            break;
        case LOOP_JMP:
            i->operands[0].type = OPERAND_WORD;
            i->operands[0].word.type = WORD_TYPE_IMM;
            i->operands[0].word.width = BIT_SIZE_8;
            i->operands[0].word.value = (i8) mem[a + 1];
            a += 2;
            i->size = a - i->base_addr;
            j_arr[nj++] = i;
            break;
        }
    }

    if (abort) {
        free(i_arr);
        return NULL;
    }

    instruction_t *j = NULL;
    for (u32 k = 0; k < nj; k++) {
        j = j_arr[k];
        assert(j);
        const i32 jmp = j->operands[0].word.value + j->base_addr + j->size;
        if (label_jmp(jmp, i_arr, ni)) {
            printf("instruction with address ??? + %d does not exist\n", jmp);
            abort = 1;
        }
    }
    free(j_arr);

    src_instructions_t *src = init_src_instruction(i_arr, ni);
    if (src == NULL) {
        free(i_arr);
        return NULL;
    }
    i_arr = NULL;

    return src;
}

static i32 print_word(word_t *w) {
    i32 nwrite = 0;
    switch (w->type) {
    case WORD_TYPE_NONE:
        break;
    case WORD_TYPE_ADDR:
        nwrite += printf("[%d]", w->value);
        break;
    case WORD_TYPE_DISP:
        if (!w->value) return 0;
        nwrite += w->value > 0 ? printf("+%d", w->value) : printf("-%d", 
                -w->value);
        break;
    case WORD_TYPE_IMM:
        nwrite += printf("%d", w->value);
        break;
    }
    return nwrite;
}

static i32 print_eff_addr_expr(eff_addr_expr_t *e) {
    i32 nwrite = 0;
    nwrite += printf("[");
    if (e->da) {
        printf("%d", e->disp.value);
        nwrite += printf("]");
        return nwrite;
    }
    print_reg(e->terms[0]);
    if (e->terms[1]->reg != REG_NONE) { 
        nwrite += printf("+");
        nwrite += print_reg(e->terms[1]);
    }
    nwrite += print_word(&e->disp);
    nwrite += printf("]");
    return nwrite;
}


static i32 print_reg(const reg_t *r) {
    i32 nwrite = printf("%s", get_reg_name(r->reg));
    if (r->reg > REG_B) { return nwrite; }
    if (r->length == BIT_SIZE_8) {
        nwrite += printf("%s", r->offset ? "h" : "l");
    } else {
        nwrite += printf("x");
    }
    return nwrite;
}

static i32 print_operand(operand_t *o) {
    i32 nwrite = 0;
    switch (o->type) {
    case OPERAND_NONE:
        break;
    case OPERAND_EFF_ADDR_EXPR:
        nwrite += print_eff_addr_expr(&o->expr);
        break;
    case OPERAND_WORD:
        nwrite += print_word(&o->word);
        break;
    case OPERAND_REG:
        nwrite += print_reg(o->reg);
        break;
    }
    return nwrite;
}

static void print_jmp(instruction_t *in) {
    const char *name;
    const i32 jmp = in->operands[0].word.value + in->base_addr + in->size;
    switch (in->opcode) {
    case COND_JMP:
        name = get_cond_jmp(decode_jump_variant(in->raw_ctrl_bits1));
        break;
    case LOOP_JMP:
        name = get_loop_jmp(decode_loop_variant(in->raw_ctrl_bits1));
        break;
    default:
        assert(0);
    }
    printf("%s ", name);
    printf("jmp_addr%d\n", jmp);
}

void print_instruction(instruction_t *in, u8 show_base_addr, u8 as_comment) {
    if (show_base_addr) {
        printf("; address = ??? + 0x%x, instruction width = %u\n", in->base_addr, 
                in->size);
    }
    if (in->is_jmp_dest) {
        if (as_comment) {
            printf("; ");
        }
        printf("jmp_addr%d:\n", in->base_addr);
    }
    const char *name;
    switch (in->opcode) {
    case OP_NONE:
        return;
    case MOV_REG_MEM_REG: 
    case MOV_IMM_REG_MEM:
    case MOV_IMM_REG:
    case MOV_ACC:
    case MOV_REG_MEM_SEG:
        name = "mov";
        break;
    case ADD_REG_MEM_REG:
    case ADD_IMM_REG_MEM:
    case ADD_IMM_ACC:
        name = "add";
        break;
    case SUB_REG_MEM_REG:
    case SUB_IMM_REG_MEM:
    case SUB_IMM_ACC:
        name = "sub";
        break;
    case CMP_REG_MEM_REG:
    case CMP_IMM_REG_MEM:
    case CMP_IMM_ACC:
        name = "cmp";
        break;
    case COND_JMP:
    case LOOP_JMP:
        print_jmp(in);
        return;
    }
    if (as_comment) {
        printf("; ");
    }
    printf("%s ", name);
    switch (in->prefix) {
    case PREFIX_NONE:
        break;
    case PREFIX_EXPLICIT_SIZE:
        printf("%s ", in->ctrl_bits >> 4 & 1 ? "word" : "byte");
        break;
    }
    if (in->operands[1].type == OP_NONE) {
        print_operand(&in->operands[0]);
        return;
    }
    if (get_d(in->ctrl_bits)) {
        print_operand(&in->operands[0]);
        printf(",");
        print_operand(&in->operands[1]);
    } else {
        print_operand(&in->operands[1]);
        printf(",");
        print_operand(&in->operands[0]);
    }
}
