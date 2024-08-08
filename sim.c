#include <assert.h>
#include <stdio.h>

#include "sim.h"
#include "decoder.h"
#include "table.h"


i32 read_reg(cpu_t *c, const reg_t *r) {
    assert(c && r);
    assert(r->reg != REG_NONE);
    switch (r->length) {
    case BIT_SIZE_8:
    case BIT_SIZE_16:
        break;
    default:
        assert(0);
    }
    if (r->length == BIT_SIZE_16) {
        assert(r->offset == BIT_SIZE_0);
        return c->regs[r->reg - REG_A];
    }
    if (r->length == BIT_SIZE_8) {
        switch (r->offset) {
        case BIT_SIZE_0:
        case BIT_SIZE_8:
            break;
        default:
            assert(0);
        }
        return r->offset == BIT_SIZE_0 ?
            c->regs[r->reg - REG_A] & BIT_MASK_8_LO :
            (c->regs[r->reg - REG_A] & BIT_MASK_8_HI) >> BYTE_BIT_LENGTH;
    }
    assert(0); // unreachable
}

void write_reg_val(cpu_t *c, const reg_t *r, u32 v) {
    assert(c && r);
    printf("; Before: ");
    print_reg_state(c, r);
    printf("\n");
    switch (r->length) {
    case BIT_SIZE_8:
        assert(v <= BIT_MASK_8_LO);
        switch (r->offset) {
            case BIT_SIZE_0:
                c->regs[r->reg - REG_A] &= 0xff00;
                c->regs[r->reg - REG_A] |= v & 0xff;
                break;
            case BIT_SIZE_8:
                c->regs[r->reg - REG_A] &= 0x00ff;
                c->regs[r->reg - REG_A] |= (v & 0xff) << BYTE_BIT_LENGTH;
                break;
            default:
                assert(0);
        }
        break;
    case BIT_SIZE_16:
        assert(v <= BIT_MASK_16);
        c->regs[r->reg - REG_A] = v;
        break;
    default:
        assert(0);
    }
    printf("; After: ");
    print_reg_state(c, r);
    printf("\n");
}

void write_reg_reg(cpu_t *c, const reg_t *dest, const reg_t *src) {
    assert(c && dest && src);
    const i32 v = read_reg(c, src);
    write_reg_val(c, dest, v);
}

i32 simulate(src_instructions_t *src, memory_t *mem) {
    cpu_t cpu = { 0 };

    for (i32 i = REG_A; i <= REG_DS; i++) { cpu.regs[i - 1] = 0; }

    printf("; Execution context:\n");
    instruction_t i;
    operand_t from;
    operand_t to;
    for (u32 c = 0; c < src->count; c++) {
        i = src->i_arr[c];
        print_instruction(&i, 0, 1);
        printf("\n");
        switch (i.opcode) {
        case OP_NONE:
            printf("; Unknown opcode make it to the simulation from decoding \
                    stage\n");
            return 1;
            break;
        case MOV_REG_MEM_REG:
        case MOV_REG_MEM_SEG:
            to = get_d(i.ctrl_bits) ? i.operands[0] : i.operands[1];
            from = get_d(i.ctrl_bits) ? i.operands[1] : i.operands[0];
            switch (get_m(i.ctrl_bits)) {
            case MOD_11:
                assert(to.type == OPERAND_REG && from.type == OPERAND_REG);
                write_reg_reg(&cpu, to.reg, from.reg);
                break;
            case MOD_10:
                break;
            case MOD_01:
                break;
            case MOD_00:
                break;
            }
            break;
        case MOV_IMM_REG_MEM:
            break;
        case MOV_IMM_REG:
            to = i.operands[1];
            from = i.operands[0];
            assert(to.type == OPERAND_REG);
            assert(from.type == OPERAND_WORD && from.word.type == WORD_TYPE_IMM);
            if (get_w(i.ctrl_bits)) {
                assert(to.reg->length == BIT_SIZE_16 && 
                       from.word.width == BIT_SIZE_16);
            } else {
                assert(from.word.width == BIT_SIZE_8 &&
                       to.reg->reg < REG_BP && to.reg->length == BIT_SIZE_8);
            }
            write_reg_val(&cpu, to.reg, from.word.value);
            break;
        default:
            printf("; This operation is not yet implemented yet.\n");
            break;
        }
        printf("\n");
    }

    printf("; Final register state:\n");
    print_regs_state(&cpu);

    return 0;
}

static void print_reg_state(cpu_t *c, const reg_t *r) {
    printf("; register %s state: %d (%#x)", get_reg_name(r->reg), 
            c->regs[r->reg - 1], c->regs[r->reg - 1]);
}

void print_regs_state(cpu_t *c) {
    for (u32 i = REG_A; i <= REG_DS; i++) {
        printf("; register %s state: %d (%#x)\n", get_reg_name(i), c->regs[i - 1], 
                c->regs[i - 1]);
    }
}
