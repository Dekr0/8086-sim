#include <assert.h>

#include "cpu.h"

u32 read_reg(cpu_t *cpu, const reg_t *reg_t) {
    assert(cpu && reg_t);
    assert(reg_t->reg != REG_NONE);
    switch (reg_t->length) {
    case BIT_SIZE_8:
    case BIT_SIZE_16:
        break;
    default:
        assert(0);
    }
    if (reg_t->length == BIT_SIZE_16) {
        assert(reg_t->offset == BIT_SIZE_0);
        return cpu->regs[reg_t->reg - REG_A];
    }
    if (reg_t->length == BIT_SIZE_8) {
        switch (reg_t->offset) {
        case BIT_SIZE_0:
        case BIT_SIZE_8:
            break;
        default:
            assert(0);
        }
        return reg_t->offset == BIT_SIZE_0
                   ? cpu->regs[reg_t->reg - REG_A] & BIT_MASK_8_LO
                   : (cpu->regs[reg_t->reg - REG_A] & BIT_MASK_8_HI) >>
                         BYTE_BIT_LENGTH;
    }
    assert(0);
}

void write_reg_val(cpu_t *cpu, const reg_t *reg_t, u32 val) {
    assert(cpu && reg_t);
    switch (reg_t->length) {
    case BIT_SIZE_8:
        assert(val <= BIT_MASK_8_LO);
        switch (reg_t->offset) {
        case BIT_SIZE_0:
            cpu->regs[reg_t->reg - REG_A] &= 0xff00;
            cpu->regs[reg_t->reg - REG_A] |= val & 0xff;
            break;
        case BIT_SIZE_8:
            cpu->regs[reg_t->reg - REG_A] &= 0x00ff;
            cpu->regs[reg_t->reg - REG_A] |= (val & 0xff) << BYTE_BIT_LENGTH;
            break;
        default:
            assert(0);
        }
        break;
    case BIT_SIZE_16:
        assert(val <= BIT_MASK_16);
        cpu->regs[reg_t->reg - REG_A] = val;
        break;
    default:
        assert(0);
    }
}

void write_reg_reg(cpu_t *cpu, const reg_t *dest_reg_t,
                   const reg_t *src_reg_t) {
    assert(cpu && dest_reg_t && src_reg_t);
    assert(dest_reg_t->length == src_reg_t->length);
    const u32 src_val = read_reg(cpu, src_reg_t);
    write_reg_val(cpu, dest_reg_t, src_val);
}

u32 calculate_eff_addr_expr(cpu_t *cpu, eff_addr_expr_t *expr,
                            const mod_e mod) {
    i32 disp = 0;
    switch (mod) {
    case MOD_00:
        if (expr->da) {
            assert(expr->disp.width == BIT_SIZE_16 &&
                   expr->disp.value <= 0xffff);
            disp = expr->disp.value;
            break;
        }
        assert(expr->disp.width == BIT_SIZE_0);
        break;
    case MOD_01:
        assert(expr->disp.width == BIT_SIZE_8 && expr->disp.value <= 0xff);
        disp = expr->disp.sign ? (i8)expr->disp.value : expr->disp.value;
        break;
    case MOD_10:
        assert(expr->disp.width == BIT_SIZE_16 && expr->disp.value <= 0xfff);
        disp = expr->disp.sign ? (i16)expr->disp.value : expr->disp.value;
        break;
    case MOD_11:
        assert(0);
    }

    u16 physical_addr = 0;
    const u32 idx_val1 =
        expr->terms[0]->reg != REG_NONE ? read_reg(cpu, expr->terms[0]) : 0;
    const u32 idx_val2 =
        expr->terms[1]->reg != REG_NONE ? read_reg(cpu, expr->terms[1]) : 0;
    physical_addr = (i32)idx_val1 + (i32)idx_val2 + disp;

    return physical_addr;
}
