#include <assert.h>
#include <stdio.h>

#include "sim.h"
#include "cpu.h"
#include "decoder.h"
#include "instruction.h"
#include "table.h"
#include "type.h"


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

i32 binary_add(u16 *f, u32 a, u32 b, const u8 s, const bit_width_e w) {
    u8 width = 0;
    u8 a_sign = 0;
    u8 b_sign = 0;
    switch (w) {
    case BIT_SIZE_0:
        return 0;
    case BIT_SIZE_8:
        assert(a <= 0xff && b <= 0xff);
        width = 8;
        a_sign = a >> 7 & 1;
        b_sign = b >> 7 & 1;
        break;
    case BIT_SIZE_16:
        assert(a <= 0xffff && b <= 0xffff);
        width = 16;
        a_sign = a >> 15 & 1;
        b_sign = b >> 15 & 1;
        break;
    }
    u8 carry = 0;
    u8 a_bit = 0;
    u8 b_bit = 0;
    u32 r = 0;
    for (u8 i = 0; i < width; i++) {
        a_bit = a & 1; a >>= 1;
        b_bit = b & 1; b >>= 1;
        r |= (a_bit ^ b_bit ^ carry) << i;
        carry = (a_bit & b_bit) | (carry & (a_bit ^ b_bit));
    }
    set_cf(*f, carry);
    set_pf(*f, !(r & 1));
    // AF
    set_zf(*f, !r);
    u8 is_signed = 0;
    switch (w) {
    case BIT_SIZE_0:
        break;
    case BIT_SIZE_8:
        is_signed = r >> 7 & 1;
        break;
    case BIT_SIZE_16:
        is_signed = r >> 15 & 1;
        break;
    }
    set_sf(*f, is_signed);
    set_of(*f, (!a_sign & !b_sign & is_signed) | (a_sign & b_sign & !is_signed));
    return r & 0xffff;
}

i32 binary_sub();

i32 simulate(src_instructions_t *src, memory_t *mem) {
    cpu_t cpu = { 0 };

    for (i32 i = REG_A; i <= REG_DS; i++) { cpu.regs[i - 1] = 0; }

    printf("; Execution context:\n");
    instruction_t i;
    operand_t from;
    operand_t to;
    u32 from_v;
    u32 to_v;
    u32 res;
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
            default:
                printf("MOV with other mods is yet implemented.\n");
            }
            break;
        case MOV_IMM_REG_MEM:
            break;
        case MOV_IMM_REG:
            from = i.operands[0];
            to = i.operands[1];
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
        case ADD_REG_MEM_REG:
            to = get_d(i.ctrl_bits) ? i.operands[0] : i.operands[1];
            from = get_d(i.ctrl_bits) ? i.operands[1] : i.operands[0];
            switch (get_m(i.ctrl_bits)) {
            case MOD_11:
                from_v = read_reg(&cpu, from.reg);
                to_v = read_reg(&cpu, to.reg);
                res = binary_add(&cpu.flags, to_v, from_v, get_s(i.ctrl_bits), 
                        get_w(i.ctrl_bits) ? BIT_SIZE_16 : BIT_SIZE_8);
                write_reg_val(&cpu, to.reg, res);
                break;
            default:
                printf("; ADD with other mods is yet implemented\n");
            }
            break;
        case ADD_IMM_REG_MEM:
            break;
        default:
            printf("; This operation is not yet implemented yet.\n");
            break;
        }
        printf("\n");
    }

    printf("; Final register state:\n");
    print_cpu_state(&cpu);

    return 0;
}

static void print_reg_state(cpu_t *c, const reg_t *r) {
    printf("; register %s state: %d (%#04x)", get_reg_name(r->reg), 
            c->regs[r->reg - 1], c->regs[r->reg - 1]);
}

void print_cpu_state(cpu_t *c) {
    for (u32 i = REG_A; i <= REG_DS; i++) {
        printf("; register %s state: %d (%#04x)\n", get_reg_name(i), c->regs[i - 1], 
                c->regs[i - 1]);
    }
    printf(";cf: %d\n", get_cf(c->flags));
    printf(";pf: %d\n", get_pf(c->flags));
    printf(";af: %d\n", get_af(c->flags));
    printf(";zf: %d\n", get_zf(c->flags));
    printf(";sf: %d\n", get_sf(c->flags));
    printf(";of: %d\n", get_of(c->flags));
    printf(";if: %d\n", get_if(c->flags));
    printf(";df: %d\n", get_df(c->flags));
    printf(";tf: %d\n", get_tf(c->flags));
}
