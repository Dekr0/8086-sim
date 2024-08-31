#include <string.h>

#include "instruction.h"

void deep_copy_instr(instr_t *dest, const instr_t *src) {
    // Non struct and array field
    memcpy(dest, src, sizeof(instr_t));
    deep_copy_operand(&dest->operands[0], &src->operands[0]);
    deep_copy_operand(&dest->operands[1], &src->operands[1]);
}

void deep_copy_operand(operand_t *dest, const operand_t *src) {
    dest->type = src->type;
    switch (dest->type) {
    case OPERAND_NONE:
        return;
    case OPERAND_REG:
        dest->reg = src->reg;
        break;
    case OPERAND_WORD:
        memcpy(&dest->word, &src->word, sizeof(word_t));
        break;
    case OPERAND_EFF_ADDR_EXPR:
        dest->expr.da = src->expr.da;
        memcpy(&dest->expr.disp, &src->expr.disp, sizeof(word_t));
        dest->expr.terms[0] = src->expr.terms[0];
        dest->expr.terms[1] = src->expr.terms[1];
        break;
    }
}

i8 deep_cmp_instr(const instr_t *dest, const instr_t *src) {
    return memcmp(dest, src, sizeof(instr_t)) ||
           deep_cmp_operand(&dest->operands[0], &src->operands[0]) ||
           deep_cmp_operand(&dest->operands[1], &src->operands[1]);
}

i8 deep_cmp_operand(const operand_t *dest, const operand_t *src) {
    if (dest->type != src->type)
        return 1;

    switch (dest->type) {
    case OPERAND_NONE:
        return 0;
    case OPERAND_REG:
        return dest->reg != src->reg;
    case OPERAND_EFF_ADDR_EXPR:
        return dest->expr.da != src->expr.da ||
               memcmp(&dest->expr.disp, &src->expr.disp, sizeof(word_t)) ||
               dest->expr.terms[0] != src->expr.terms[0] ||
               dest->expr.terms[1] != src->expr.terms[1];
    case OPERAND_WORD:
        return memcmp(&dest->word, &src->word, sizeof(word_t));
    }
}

void clear_instr(instr_t *i) {
    memset(i, 0, sizeof(instr_t));
    clear_operand(&i->operands[0]);
    clear_operand(&i->operands[1]);
}

void clear_operand(operand_t *o) {
    o->type = OPERAND_NONE;
    memset(&o->expr, 0, sizeof(eff_addr_expr_t));
    memset(&o->expr.disp, 0, sizeof(word_t));
    o->expr.terms[0] = NULL;
    o->expr.terms[1] = NULL;
    memset(&o->word, 0, sizeof(word_t));
    o->reg = NULL;
}
