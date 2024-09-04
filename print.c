#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "print.h"
#include "table.h"

u32 print_word(const word_t *w, FILE *f) {
    if (f == NULL)
        return 0;
    u32 nprint = 0;
    switch (w->type) {
    case WORD_TYPE_NONE:
        break;
    case WORD_TYPE_ADDR:
        nprint += fprintf(f, "[%d]", w->value);
        break;
    case WORD_TYPE_DISP:
        if (w->sign) {
            switch (w->width) {
            case BIT_SIZE_8:
                if ((i8)w->value > 0)
                    nprint += fprintf(f, "+%d", (i8)w->value);
                else
                    nprint += fprintf(f, "%d", (i8)w->value);
                break;
            case BIT_SIZE_16:
                if ((i16)w->value > 0)
                    nprint += fprintf(f, "+%d", (i16)w->value);
                else
                    nprint += fprintf(f, "%d", (i16)w->value);
                break;
            default:
                assert(0);
            }
        } else
            nprint += fprintf(f, "+%d", w->value);
        break;
    case WORD_TYPE_IMM:
        if (w->sign) {
            switch (w->width) {
            case BIT_SIZE_8:
                nprint += fprintf(f, "%d", (i8)w->value);
                break;
            case BIT_SIZE_16:
                nprint += fprintf(f, "%d", (i16)w->value);
                break;
            default:
                assert(0);
            }
        } else
            nprint += fprintf(f, "%d", w->value);
        break;
    }
    return nprint;
}

u32 print_eff_addr_expr(const eff_addr_expr_t *e, FILE *f) {
    if (f == NULL)
        return 0;
    u32 nprint = fprintf(f, "[");
    if (e->da) {
        nprint += fprintf(f, "%d", e->disp.value);
        nprint += fprintf(f, "]");
        return nprint;
    }
    print_reg(e->terms[0], f);
    if (e->terms[1]->reg != REG_NONE) {
        nprint += fprintf(f, "+");
        nprint += print_reg(e->terms[1], f);
    }
    nprint += print_word(&e->disp, f);
    nprint += fprintf(f, "]");
    return nprint;
}

u32 print_reg(const reg_t *r, FILE *f) {
    if (f == NULL)
        return 0;
    u32 nprint = fprintf(f, "%s", get_reg_name(r->reg));
    if (r->reg > REG_B)
        return nprint;
    if (r->length == BIT_SIZE_8)
        nprint += fprintf(f, "%s", r->offset ? "h" : "l");
    else
        nprint += fprintf(f, "X");
    return nprint;
}

u32 print_operand(const operand_t *o, FILE *f) {
    if (f == NULL)
        return 0;
    u32 nprint = 0;
    switch (o->type) {
    case OPERAND_NONE:
        break;
    case OPERAND_EFF_ADDR_EXPR:
        nprint += print_eff_addr_expr(&o->expr, f);
        break;
    case OPERAND_WORD:
        nprint += print_word(&o->word, f);
        break;
    case OPERAND_REG:
        nprint += print_reg(o->reg, f);
        break;
    }
    return nprint;
}

u32 print_jmp_instr(const instr_t *instr_t, FILE *f) {
    if (f == NULL)
        return 0;
    const char *name;
    const i32 jmp =
        instr_t->operands[0].word.value + instr_t->base_addr + instr_t->size;
    switch (instr_t->opcode) {
    case OPCODE_COND_JMP:
        name = get_cond_jmp(decode_cond_jump_variant(instr_t->raw_ctrl_bits1));
        break;
    case OPCODE_LOOP_JMP:
        name = get_loop_jmp(decode_loop_variant(instr_t->raw_ctrl_bits1));
        break;
    default:
        assert(0);
    }
    u32 nprintf = fprintf(f, "%s ", name);
    nprintf += fprintf(f, "jmp_addr%d", jmp);

    return nprintf;
}

u32 print_instr(const instr_t *instr_t, u8 show_base_addr, u8 as_comment,
                FILE *f) {
    if (f == NULL)
        return 0;
    u32 nprint = 0;
    if (show_base_addr)
        nprint +=
            fprintf(f, "; address = ??? + 0x%x, instruction width = %u ;\n",
                    instr_t->base_addr, instr_t->size);
    if (as_comment)
        nprint += fprintf(f, "; ");
    if (instr_t->is_jmp_dest)
        nprint += fprintf(f, "jmp_addr%d: ", instr_t->base_addr);
    const char *name;
    switch (instr_t->opcode) {
    case OPCODE_NONE:
        return 0;
    case OPCODE_MOV_REG_MEM_REG:
    case OPCODE_MOV_IMM_REG_MEM:
    case OPCODE_MOV_IMM_REG:
    case OPCODE_MOV_ACC:
    case OPCODE_MOV_REG_MEM_SEG:
        name = "mov";
        break;
    case OPCODE_ADD_REG_MEM_REG:
    case OPCODE_ADD_IMM_REG_MEM:
    case OPCODE_ADD_IMM_ACC:
        name = "add";
        break;
    case OPCODE_SUB_REG_MEM_REG:
    case OPCODE_SUB_IMM_REG_MEM:
    case OPCODE_SUB_IMM_ACC:
        name = "sub";
        break;
    case OPCODE_CMP_REG_MEM_REG:
    case OPCODE_CMP_IMM_REG_MEM:
    case OPCODE_CMP_IMM_ACC:
        name = "cmp";
        break;
    case OPCODE_COND_JMP:
    case OPCODE_LOOP_JMP:
        nprint += print_jmp_instr(instr_t, f);
        return nprint;
    }
    nprint += fprintf(f, "%s ", name);
    switch (instr_t->prefix) {
    case PREFIX_NONE:
        break;
    case PREFIX_EXPLICIT_SIZE:
        nprint +=
            fprintf(f, "%s ", get_w(instr_t->ctrl_bits) ? "word" : "byte");
        break;
    }
    if (instr_t->operands[1].type == OPERAND_NONE) {
        nprint += print_operand(&instr_t->operands[0], f);
        return nprint;
    }
    if (get_d(instr_t->ctrl_bits)) {
        nprint += print_operand(&instr_t->operands[0], f);
        nprint += fprintf(f, ",");
        nprint += print_operand(&instr_t->operands[1], f);
    } else {
        nprint += print_operand(&instr_t->operands[1], f);
        nprint += fprintf(f, ",");
        nprint += print_operand(&instr_t->operands[0], f);
    }
    return nprint;
}

void print_cpu(const cpu_t *cpu, FILE *f) {
    if (f == NULL)
        return;
    fprintf(f, "\n; CPU state:\n");
    for (u32 i = REG_A; i <= REG_DI; i++)
        fprintf(f, "; %s:%#04x\n", get_reg_name(i), cpu->EU.regs[i - REG_A]);
    for (u32 i = REG_ES; i <= REG_DS; i++)
        fprintf(f, "; %s:%#04x\n", get_reg_name(i), cpu->BIU.regs[i - REG_ES]);
    fprintf(f, "; IP:%#04x\n", cpu->BIU.IP);
    fprintf(f, "; flags: ");
    print_cpu_flags(&cpu->EU.flags, f);
}

void print_cpu_flags(const u16 *flag_reg, FILE *f) {
    if (f == NULL)
        return;
    if (get_cf(*flag_reg))
        fprintf(f, "C");
    if (get_pf(*flag_reg))
        fprintf(f, "P");
    if (get_af(*flag_reg))
        fprintf(f, "A");
    if (get_zf(*flag_reg))
        fprintf(f, "Z");
    if (get_sf(*flag_reg))
        fprintf(f, "S");
    if (get_of(*flag_reg))
        fprintf(f, "O");
    if (get_if(*flag_reg))
        fprintf(f, "I");
    if (get_df(*flag_reg))
        fprintf(f, "D");
    if (get_tf(*flag_reg))
        fprintf(f, "T");
}

void print_cpu_reg(const cpu_t *cpu, const reg_t *reg_t, FILE *f) {
    if (f == NULL)
        return;
    if (reg_t->reg <= REG_DI)
        fprintf(f, "%s:%#04x", get_reg_name(reg_t->reg),
                cpu->EU.regs[reg_t->reg - REG_A]);
    else
        fprintf(f, "%s:%#04x", get_reg_name(reg_t->reg),
                cpu->BIU.regs[reg_t->reg - REG_ES]);
}

i8 sprint_word(const word_t *w, str_builder_t *b) {
    if (b == NULL)
        return 1;
    switch (w->type) {
    case WORD_TYPE_NONE:
        return 0;
    case WORD_TYPE_ADDR:
        return put_fmt(b, "[%d]", w->value);
    case WORD_TYPE_DISP:
        if (w->sign) {
            switch (w->width) {
            case BIT_SIZE_8:
                if ((i8)w->value > 0)
                    return put_fmt(b, "+%d", (i8)w->value);

                return put_fmt(b, "%d", (i8)w->value);
            case BIT_SIZE_16:
                if ((i16)w->value > 0)
                    return put_fmt(b, "+%d", (i16)w->value);

                return put_fmt(b, "%d", (i16)w->value);
            default:
                assert(0);
            }
        }
        return put_fmt(b, "+%d", w->value);
    case WORD_TYPE_IMM:
        if (w->sign) {
            switch (w->width) {
            case BIT_SIZE_8:
                return put_fmt(b, "%d", (i8)w->value);
            case BIT_SIZE_16:
                return put_fmt(b, "%d", (i16)w->value);
            default:
                assert(0);
            }
        }
        return put_fmt(b, "%d", w->value);
    }
}

i8 sprint_reg(const reg_t *r, str_builder_t *b) {
    if (b == NULL)

        return 1;
    if (put_fmt(b, "%s", get_reg_name(r->reg))) return 1;

    if (r->reg > REG_B)
        return 0;

    if (r->length == BIT_SIZE_8)
        return put_fmt(b, "%s", r->offset ? "h" : "l");
    
    return put_char(b, 'X');
}

i8 sprint_eff_addr_expr(const eff_addr_expr_t *e, str_builder_t *b) {
    if (b == NULL)
        return 1;

    if (put_char(b, '[')) return 1;

    if (e->da)
        return put_fmt(b, "%d", e->disp.value) || put_char(b, ']');

    if (sprint_reg(e->terms[0], b)) return 1;

    if (e->terms[1]->reg != REG_NONE)
        return put_char(b, '+') || sprint_reg(e->terms[1], b);

    return sprint_word(&e->disp, b) || put_char(b, ']');
}

i8 sprint_operand(const operand_t *o, str_builder_t *b) {
    if (b == NULL)
        return 1;
    switch (o->type) {
    case OPERAND_NONE:
        return 0;
    case OPERAND_EFF_ADDR_EXPR:
        return sprint_eff_addr_expr(&o->expr, b);
    case OPERAND_WORD:
        return sprint_word(&o->word, b);
    case OPERAND_REG:
        return sprint_reg(o->reg, b);
    }
}

i8 sprint_jmp_instr(const instr_t *instr_t, str_builder_t *b) {
    if (b == NULL)
        return 1;
    const char *name;
    const i32 jmp =
        instr_t->operands[0].word.value + instr_t->base_addr + instr_t->size;
    switch (instr_t->opcode) {
    case OPCODE_COND_JMP:
        name = get_cond_jmp(decode_cond_jump_variant(instr_t->raw_ctrl_bits1));
        break;
    case OPCODE_LOOP_JMP:
        name = get_loop_jmp(decode_loop_variant(instr_t->raw_ctrl_bits1));
        break;
    default:
        assert(0);
    }
    if (put_fmt(b, "%s ", name)) return 1;

    if (put_fmt(b, "jmp_addr%d", jmp)) return 1;

    return 0;
}

i8 sprint_instr(const instr_t *instr_t, u8 show_base_addr, u8 as_comment,
                str_builder_t *b) {
    if (b == NULL)
        return 1;

    if (show_base_addr && put_fmt(b, 
                "; address = ??? + 0x%x, instruction width = %u ;\n",
                    instr_t->base_addr, instr_t->size))
        return 1;

    if (as_comment && put_str(b, "; ", 2)) return 1;

    if (instr_t->is_jmp_dest && put_fmt(b, "jmp_addr%d: ", instr_t->base_addr))
        return 1;
    const char *name;
    switch (instr_t->opcode) {
    case OPCODE_NONE:
        return 0;
    case OPCODE_MOV_REG_MEM_REG:
    case OPCODE_MOV_IMM_REG_MEM:
    case OPCODE_MOV_IMM_REG:
    case OPCODE_MOV_ACC:
    case OPCODE_MOV_REG_MEM_SEG:
        name = "mov";
        break;
    case OPCODE_ADD_REG_MEM_REG:
    case OPCODE_ADD_IMM_REG_MEM:
    case OPCODE_ADD_IMM_ACC:
        name = "add";
        break;
    case OPCODE_SUB_REG_MEM_REG:
    case OPCODE_SUB_IMM_REG_MEM:
    case OPCODE_SUB_IMM_ACC:
        name = "sub";
        break;
    case OPCODE_CMP_REG_MEM_REG:
    case OPCODE_CMP_IMM_REG_MEM:
    case OPCODE_CMP_IMM_ACC:
        name = "cmp";
        break;
    case OPCODE_COND_JMP:
    case OPCODE_LOOP_JMP:
        return sprint_jmp_instr(instr_t, b);
    }
    if (put_fmt(b, "%s ", name))
        return 1;

    switch (instr_t->prefix) {
    case PREFIX_NONE:
        break;
    case PREFIX_EXPLICIT_SIZE:
        if (put_fmt(b, "%s ", get_w(instr_t->ctrl_bits) ? "word" : "byte")) 
            return 1;
        break;
    }

    if (instr_t->operands[1].type == OPERAND_NONE)
        return sprint_operand(&instr_t->operands[0], b);

    if (get_d(instr_t->ctrl_bits))
        return sprint_operand(&instr_t->operands[0], b) || put_char(b, ',') || 
            sprint_operand(&instr_t->operands[1], b);

    return sprint_operand(&instr_t->operands[1], b) || put_char(b, ',') || 
        sprint_operand(&instr_t->operands[0], b); 
}
