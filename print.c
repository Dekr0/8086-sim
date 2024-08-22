#include <assert.h>

#include "print.h"
#include "table.h"

i32 print_word(word_t *w, FILE *wf) {
    i32 nwrite = 0;
    switch (w->type) {
    case WORD_TYPE_NONE:
        break;
    case WORD_TYPE_ADDR:
        nwrite += fprintf(wf, "[%d]", w->value);
        break;
    case WORD_TYPE_DISP:
        if (w->sign) {
            switch (w->width) {
            case BIT_SIZE_8:
                if ((i8)w->value > 0) {
                    nwrite += fprintf(wf, "+ %d", (i8)w->value);
                } else {
                    nwrite += fprintf(wf, "%d", (i8)w->value);
                }
                break;
            case BIT_SIZE_16:
                if ((i16)w->value > 0) {
                    nwrite += fprintf(wf, "+ %d", (i16)w->value);
                } else {
                    nwrite += fprintf(wf, "%d", (i16)w->value);
                }
                break;
            default:
                assert(0);
            }
        } else {
            nwrite += fprintf(wf, "+ %d", w->value);
        }
        break;
    case WORD_TYPE_IMM:
        if (w->sign) {
            switch (w->width) {
            case BIT_SIZE_8:
                nwrite += fprintf(wf, "%d", (i8)w->value);
                break;
            case BIT_SIZE_16:
                nwrite += fprintf(wf, "%d", (i16)w->value);
                break;
            default:
                assert(0);
            }
        } else {
            nwrite += fprintf(wf, "%d", w->value);
        }
        break;
    }
    return nwrite;
}

i32 print_eff_addr_expr(eff_addr_expr_t *e, FILE *wf) {
    i32 nwrite = 0;
    nwrite += fprintf(wf, "[");
    if (e->da) {
        fprintf(wf, "%d", e->disp.value);
        nwrite += fprintf(wf, "]");
        return nwrite;
    }
    print_reg(e->terms[0], wf);
    if (e->terms[1]->reg != REG_NONE) {
        nwrite += fprintf(wf, "+");
        nwrite += print_reg(e->terms[1], wf);
    }
    nwrite += print_word(&e->disp, wf);
    nwrite += fprintf(wf, "]");
    return nwrite;
}

i32 print_reg(const reg_t *r, FILE *wf) {
    i32 nwrite = fprintf(wf, "%s", get_reg_name(r->reg));
    if (r->reg > REG_B) {
        return nwrite;
    }
    if (r->length == BIT_SIZE_8) {
        nwrite += fprintf(wf, "%s", r->offset ? "h" : "l");
    } else {
        nwrite += fprintf(wf, "x");
    }
    return nwrite;
}

i32 print_operand(operand_t *o, FILE *wf) {
    i32 nwrite = 0;
    switch (o->type) {
    case OPERAND_NONE:
        break;
    case OPERAND_EFF_ADDR_EXPR:
        nwrite += print_eff_addr_expr(&o->expr, wf);
        break;
    case OPERAND_WORD:
        nwrite += print_word(&o->word, wf);
        break;
    case OPERAND_REG:
        nwrite += print_reg(o->reg, wf);
        break;
    }
    return nwrite;
}

void print_jmp_instr(instr_t *instr_t, FILE *wf) {
    const char *name;
    const i32 jmp =
        instr_t->operands[0].word.value + instr_t->base_addr + instr_t->size;
    switch (instr_t->opcode) {
    case COND_JMP:
        name = get_cond_jmp(decode_cond_jump_variant(instr_t->raw_ctrl_bits1));
        break;
    case LOOP_JMP:
        name = get_loop_jmp(decode_loop_variant(instr_t->raw_ctrl_bits1));
        break;
    default:
        assert(0);
    }
    fprintf(wf, "%s ", name);
    fprintf(wf, "jmp_addr%d", jmp);
}

void print_instr(instr_t *instr_t, u8 show_base_addr, u8 as_comment, FILE *wf) {
    if (show_base_addr) {
        fprintf(wf, "; address = ??? + 0x%x, instruction width = %u ;\n",
                instr_t->base_addr, instr_t->size);
    }
    if (instr_t->is_jmp_dest) {
        if (as_comment) {
            fprintf(wf, "; ");
        }
        fprintf(wf, "jmp_addr%d:\n", instr_t->base_addr);
    }
    if (as_comment) {
        fprintf(wf, "; ");
    }
    const char *name;
    switch (instr_t->opcode) {
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
        print_jmp_instr(instr_t, wf);
        return;
    }
    fprintf(wf, "%s ", name);
    switch (instr_t->prefix) {
    case PREFIX_NONE:
        break;
    case PREFIX_EXPLICIT_SIZE:
        fprintf(wf, "%s ", instr_t->ctrl_bits >> 4 & 1 ? "word" : "byte");
        break;
    }
    if (instr_t->operands[1].type == OP_NONE) {
        print_operand(&instr_t->operands[0], wf);
        return;
    }
    if (get_d(instr_t->ctrl_bits)) {
        print_operand(&instr_t->operands[0], wf);
        fprintf(wf, ",");
        print_operand(&instr_t->operands[1], wf);
    } else {
        print_operand(&instr_t->operands[1], wf);
        fprintf(wf, ",");
        print_operand(&instr_t->operands[0], wf);
    }
}

void print_cpu(cpu_t *cpu, FILE *wf) {
    for (u32 i = REG_A; i <= REG_DS; i++) {
        printf("; %s:%#04x\n", get_reg_name(i), cpu->regs[i - 1]);
    }
    fprintf(wf, "; IP:%#04x\n", cpu->ip);
    fprintf(wf, "; flags: ");
    print_cpu_flags(&cpu->flags, wf);
}

void print_cpu_flags(u16 *flag_reg, FILE *wf) {
    if (get_cf(*flag_reg)) {
        printf("C");
    }
    if (get_pf(*flag_reg)) {
        printf("P");
    }
    if (get_af(*flag_reg)) {
        printf("A");
    }
    if (get_zf(*flag_reg)) {
        printf("Z");
    }
    if (get_sf(*flag_reg)) {
        printf("S");
    }
    if (get_of(*flag_reg)) {
        printf("O");
    }
    if (get_if(*flag_reg)) {
        printf("I");
    }
    if (get_df(*flag_reg)) {
        printf("D");
    }
    if (get_tf(*flag_reg)) {
        printf("T");
    }
}

void print_cpu_reg(cpu_t *cpu, const reg_t *reg_t, FILE *wf) {
    fprintf(wf, "%s:%#04x", get_reg_name(reg_t->reg),
            cpu->regs[reg_t->reg - 1]);
}
