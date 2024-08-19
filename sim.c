#include <assert.h>
#include <stdio.h>

#include "sim.h"
#include "cpu.h"
#include "decoder.h"
#include "instruction.h"
#include "table.h"
#include "type.h"

typedef u32 (*arithmetic)(u16 *, u32, u32, const bit_width_e);

void print_cpu_flags_state(u16 *flag_reg) {
    if(get_cf(*flag_reg)) { printf("C"); }
    if(get_pf(*flag_reg)) { printf("P"); }
    if(get_af(*flag_reg)) { printf("A"); }
    if(get_zf(*flag_reg)) { printf("Z"); }
    if(get_sf(*flag_reg)) { printf("S"); }
    if(get_of(*flag_reg)) { printf("O"); }
    if(get_if(*flag_reg)) { printf("I"); }
    if(get_df(*flag_reg)) { printf("D"); }
    if(get_tf(*flag_reg)) { printf("T"); }
}

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
        return reg_t->offset == BIT_SIZE_0 ?
            cpu->regs[reg_t->reg - REG_A] & BIT_MASK_8_LO :
            (cpu->regs[reg_t->reg - REG_A] & BIT_MASK_8_HI) >> BYTE_BIT_LENGTH;
    }
    assert(0); // unreachable
}

void write_reg_val(cpu_t *cpu, const reg_t *reg_t, u32 val) {
    assert(cpu && reg_t);
    printf("; ");
    print_reg_state(cpu, reg_t);
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
    printf("->");
    print_reg_state(cpu, reg_t);
}

void write_reg_reg(cpu_t *cpu, const reg_t *dest_reg_t, const reg_t *src_reg_t) {
    assert(cpu && dest_reg_t && src_reg_t);
    assert(dest_reg_t->length == src_reg_t->length);
    const u32 src_val = read_reg(cpu, src_reg_t);
    write_reg_val(cpu, dest_reg_t, src_val);
}

// a + b
u32 binary_add(u16 *flags_reg, u32 a, u32 b, const bit_width_e width_e) {
    u8 bits_width = 0;
    u8 a_sign_bit = 0, b_sign_bit = 0;
    switch (width_e) {
    case BIT_SIZE_0:
        return 0;
    case BIT_SIZE_8:
        assert(a <= 0xff && b <= 0xff);
        bits_width = 8;
        a_sign_bit = a >> 7 & 1;
        b_sign_bit = b >> 7 & 1;
        break;
    case BIT_SIZE_16:
        assert(a <= 0xffff && b <= 0xffff);
        bits_width = 16;
        a_sign_bit = a >> 15 & 1;
        b_sign_bit = b >> 15 & 1;
        break;
    }

    u8 carry_bit = 0;
    u8 aux_flag = 0;
    u8 bit_a = 0, bit_b = 0;
    u32 sum = 0;
    for (u8 i = 0; i < bits_width; i++) {
        if (carry_bit && i == 4) {
            aux_flag = 1;
        }
        bit_a = a & 1; a >>= 1;
        bit_b = b & 1; b >>= 1;
        sum |= (bit_a ^ bit_b ^ carry_bit) << i;
        carry_bit = (bit_a & bit_b) | (carry_bit & (bit_a ^ bit_b));
    }

    u8 sum_sign_bit = 0;
    switch (width_e) {
    case BIT_SIZE_0:
        break;
    case BIT_SIZE_8:
        sum_sign_bit = sum >> 7 & 1;
        break;
    case BIT_SIZE_16:
        sum_sign_bit = sum >> 15 & 1;
        break;
    }

    u8 one_counts = 0;
    for (u8 i = 0; i < 8; i++) {
        if ((sum >> i) & 1) { one_counts++; }
    }

    printf("; flags register: ");
    print_cpu_flags_state(flags_reg);
    set_cf(*flags_reg, carry_bit);
    set_pf(*flags_reg, !(one_counts % 2));
    set_af(*flags_reg, aux_flag);
    set_zf(*flags_reg, !sum);
    set_sf(*flags_reg, sum_sign_bit);
    set_of(*flags_reg, (!a_sign_bit & !b_sign_bit & sum_sign_bit) | 
            (a_sign_bit & b_sign_bit & !sum_sign_bit));

    printf("->");
    print_cpu_flags_state(flags_reg);
    printf("\n");
    return sum & 0xffff;
}

// b - a
u32 binary_sub(u16 *flags_reg, u32 a, u32 b, const bit_width_e bit_width_e) {
    u8 bits_width = 0;
    u8 a_sign_bit = 0, b_sign_bit = 0;
    switch (bit_width_e) {
    case BIT_SIZE_0:
        return 0;
    case BIT_SIZE_8:
        assert(a <= 0xff && b <= 0xff);
        bits_width = 8;
        a_sign_bit = a >> 7 & 1;
        b_sign_bit = b >> 7 & 1;
        break;
    case BIT_SIZE_16:
        assert(a <= 0xffff && b <= 0xffff);
        bits_width = 16;
        a_sign_bit = a >> 15 & 1;
        b_sign_bit = b >> 15 & 1;
        break;
    }

    u8 borrow_out = 0;
    u8 aux_flag = 0;
    u8 bit_a = 0;
    u8 bit_b = 0;
    u32 diff = 0;
    for (u8 i = 0; i < bits_width; i++) {
        if (borrow_out && i == 4) { aux_flag = 1; }
        bit_a = a & 1; a >>= 1;
        bit_b = b & 1; b >>= 1;
        diff |= (bit_a ^ bit_b ^ borrow_out) << i;
        borrow_out = ((!bit_a) & borrow_out) | ((!bit_a) & bit_b) | 
            (bit_b & borrow_out);
    }

    u8 diff_sign_bit = 0;
    switch (bit_width_e) {
    case BIT_SIZE_0:
        break;
    case BIT_SIZE_8:
        diff_sign_bit = diff >> 7 & 1;
        break;
    case BIT_SIZE_16:
        diff_sign_bit = diff >> 15 & 1;
        break;
    }

    u8 one_counts = 0;
    for (u8 i = 0; i < 8; i++) {
        if ((diff >> i) & 1) { one_counts++; }
    }

    printf("; flags: ");
    print_cpu_flags_state(flags_reg);
    set_cf(*flags_reg, borrow_out);
    set_pf(*flags_reg, !(one_counts % 2));
    set_af(*flags_reg, aux_flag);
    set_zf(*flags_reg, !diff);
    set_sf(*flags_reg, diff_sign_bit);
    set_of(*flags_reg, (b_sign_bit & !a_sign_bit & diff_sign_bit) | 
            (a_sign_bit & !b_sign_bit & !diff_sign_bit));
    printf(" -> ");
    print_cpu_flags_state(flags_reg);
    printf("\n");

    return diff & 0xffff;
}

static void imm_reg_mem_ops_assert(instruction_t *i, operand_t *from, operand_t *
        to) {
    assert(to->type == OPERAND_REG || to->type == OPERAND_EFF_ADDR_EXPR);
    assert(from->type == OPERAND_WORD && from->word.type == WORD_TYPE_IMM);
    if (get_w(i->ctrl_bits)) {
        if (to->type == OPERAND_REG) {
            assert(to->reg->length == BIT_SIZE_16);
        }
    } else {
        assert(from->word.width == BIT_SIZE_8);
        if (to->type == OPERAND_REG) {
            assert(from->word.width == BIT_SIZE_8 && 
                    to->reg->reg < REG_BP && to->reg->length == BIT_SIZE_8);
        }
    }
}

static void imm_reg_ops_assert(instruction_t *i, operand_t *from, operand_t 
        *to) {
    assert(to->type == OPERAND_REG);
    assert(from->type == OPERAND_WORD && from->word.type == WORD_TYPE_IMM);
    if (get_w(i->ctrl_bits)) {
        assert(to->reg->length == BIT_SIZE_16);
    } else {
        assert(from->word.width == BIT_SIZE_8 &&
               to->reg->reg < REG_BP && to->reg->length == BIT_SIZE_8);
    }
}

static void imm_acc_ops_assert(instruction_t *i, operand_t *reg_a_t, 
        operand_t *imm) {
    assert(reg_a_t->type == OPERAND_REG && reg_a_t->reg->reg == REG_A);
    assert(imm->type == OPERAND_WORD && imm->word.type == WORD_TYPE_IMM);
    if (get_w(i->ctrl_bits)) {
        assert(reg_a_t->reg->length == BIT_SIZE_16);
        assert(imm->word.width == BIT_SIZE_16);
    } else {
        assert(reg_a_t->reg->length == BIT_SIZE_8);
        assert(reg_a_t->reg->offset >= BIT_SIZE_0 || reg_a_t->reg->offset <= BIT_SIZE_8);
        assert(imm->word.width == BIT_SIZE_8);
    }
}

static void sim_mov_reg_mem(cpu_t *cpu, instruction_t *i) {
    operand_t *to = get_d(i->ctrl_bits) ? &i->operands[0] : &i->operands[1];
    operand_t *from = get_d(i->ctrl_bits) ? &i->operands[1] : &i->operands[0];
    switch (get_m(i->ctrl_bits)) {
    case MOD_11:
        assert(to->type == OPERAND_REG && from->type == OPERAND_REG);
        write_reg_reg(cpu, to->reg, from->reg);
        break;
    case MOD_10:
        printf("Register move with memory access (16 bits offset) is yet \
                implemented\n");
        break;
    case MOD_01:
        printf("Register move with memory access (8 bits offset) is yet \
                implemented\n");
        break;
    case MOD_00:
        printf("Register move with memory access is yet implemented\n");
        break;
    }
}

static void sim_mov_imm_reg_mem(cpu_t *cpu, instruction_t *i) {
    operand_t *from = &i->operands[0];
    operand_t *to = &i->operands[1];
    imm_reg_mem_ops_assert(i, from, to);
    switch (get_m(i->ctrl_bits)) {
    case MOD_11:
        assert(to->type == OPERAND_REG);
        write_reg_val(cpu, to->reg, from->word.value);
        break;
    case MOD_10:
        printf("Immediate register move with memory access (16 bits \
            offset) is yet implemented\n");
        // Add a break here to avoid fall-through if this is intentional
        break;
    case MOD_01:
        printf("Immediate register move with memory access (8 bits \
            offset) is yet implemented\n");
        break;
    case MOD_00:
        printf("Immediate register move with memory access is yet \
                implemented\n");
        break;
    }
}

static void sim_mov_imm_reg(cpu_t *cpu, instruction_t *i) {
    operand_t *from = &i->operands[0];
    operand_t *to = &i->operands[1];
    imm_reg_ops_assert(i, from, to);
    write_reg_val(cpu, to->reg, from->word.value);
}

static void sim_mov_acc(cpu_t *cpu, instruction_t *i) { }

static void sim_arithmetic_reg_mem(cpu_t *cpu, instruction_t *i, arithmetic 
        arithmetic_func, i8 write_back) {
    operand_t *to = get_d(i->ctrl_bits) ? &i->operands[0] : &i->operands[1];
    operand_t *from = get_d(i->ctrl_bits) ? &i->operands[1] : &i->operands[0];
    bit_width_e bit_width_e = get_w(i->ctrl_bits) ? BIT_SIZE_16 : BIT_SIZE_8;

    switch (get_m(i->ctrl_bits)) {
    case MOD_11:
        assert(to->type == OPERAND_REG && from->type == OPERAND_REG);
        const u32 from_val = read_reg(cpu, from->reg);
        const u32 to_val = read_reg(cpu, to->reg);
        const u32 result = arithmetic_func(&cpu->flags, to_val, from_val, 
                bit_width_e);
        if (write_back) {
            write_reg_val(cpu, to->reg, result);
        }
        break;
    case MOD_10:
        printf("Register arithmetic ops with memory access (16 bits offset) is yet \
                implemented\n");
        break;
    case MOD_01:
        printf("Register arithmetic ops with memory access (8 bits offset) is yet \
                implemented\n");
        break;
    case MOD_00:
        printf("Register arithmetic ops with memory access is yet implemented\n");
        break;
    }
}

static void sim_arithmetic_imm_reg_mem(cpu_t *cpu, instruction_t *i, 
        arithmetic arithmetic_func, i8 write_back) {
    operand_t *from = &i->operands[0];
    operand_t *to = &i->operands[1];

    imm_reg_mem_ops_assert(i, from, to);

    bit_width_e bit_width_e = get_w(i->ctrl_bits) ? BIT_SIZE_16 : 
        BIT_SIZE_8;

    u32 from_val = from->word.value;
    if (get_s(i->ctrl_bits) && get_w(i->ctrl_bits) && from_val >> 7 & 1) {
        from_val |= 0xff00;
    }

    switch (get_m(i->ctrl_bits)) {
    case MOD_11:
        assert(to->type == OPERAND_REG);
        const u32 to_val = read_reg(cpu, to->reg);
        const u32 result = arithmetic_func(&cpu->flags, to_val, from_val, 
                bit_width_e);
        if (write_back) {
            write_reg_val(cpu, to->reg, result);
        }
        break;
    case MOD_10:
        printf("Immediate register move with memory access (16 bits \
            offset) is yet implemented\n");
        break;
    case MOD_01:
        printf("Immediate register move with memory access (8 bits \
            offset) is yet implemented\n");
        break;
    case MOD_00:
        printf("Immediate register move with memory access is yet \
                implemented\n");
        break;
    }
}

static void sim_arithmetic_imm_acc(cpu_t *cpu, instruction_t *i, 
        arithmetic arithmetic_func, i8 write_back) {
    operand_t *reg_a_t = &i->operands[0];
    operand_t *imm = &i->operands[1];
    const bit_width_e bit_width_e = get_w(i->ctrl_bits) ? BIT_SIZE_16 : BIT_SIZE_8;

    imm_acc_ops_assert(i, reg_a_t, imm);

    const u32 reg_a_val = read_reg(cpu, reg_a_t->reg);
    const u32 result = arithmetic_func(&cpu->flags, reg_a_val, imm->word.value,
            bit_width_e);
    if (write_back) {
        write_reg_val(cpu, reg_a_t->reg, result);
    }
}

i32 simulate(src_instructions_t *src, memory_t *mem) {
    cpu_t cpu = { 0 };

    for (i32 i = REG_A; i <= REG_DS; i++) { cpu.regs[i - 1] = 0; }

    printf("; Execution context:\n");
    instruction_t instruction_t;
    for (u32 c = 0; c < src->count; c++) {
        instruction_t = src->i_arr[c];
        print_instruction(&instruction_t, 0, 1);
        printf("\n");
        cpu.ip += instruction_t.size;
        switch (instruction_t.opcode) {
        case OP_NONE:
            printf("; Unknown opcode make it to the simulation from decoding \
                    stage\n");
            return 1;
            break;
        case MOV_REG_MEM_REG:
        case MOV_REG_MEM_SEG:
            sim_mov_reg_mem(&cpu, &instruction_t);
            break;
        case MOV_IMM_REG_MEM:
            sim_mov_imm_reg_mem(&cpu, &instruction_t);
            break;
        case MOV_IMM_REG:
            sim_mov_imm_reg(&cpu, &instruction_t);
            break;
        case ADD_REG_MEM_REG:
            sim_arithmetic_reg_mem(&cpu, &instruction_t, binary_add, 1);
            break;
        case ADD_IMM_REG_MEM:
            sim_arithmetic_imm_reg_mem(&cpu, &instruction_t, binary_add, 1);
            break;
        case ADD_IMM_ACC:
            sim_arithmetic_imm_acc(&cpu, &instruction_t, binary_add, 1);
            break;
        case SUB_REG_MEM_REG:
            sim_arithmetic_reg_mem(&cpu, &instruction_t, binary_sub, 1);
            break;
        case SUB_IMM_REG_MEM:
            sim_arithmetic_imm_reg_mem(&cpu, &instruction_t, binary_sub, 1);
            break;
        case SUB_IMM_ACC:
            sim_arithmetic_imm_acc(&cpu, &instruction_t, binary_sub, 1);
            break;
        case CMP_REG_MEM_REG:
            sim_arithmetic_reg_mem(&cpu, &instruction_t, binary_sub, 0);
            break;
        case CMP_IMM_REG_MEM:
            sim_arithmetic_imm_reg_mem(&cpu, &instruction_t, binary_sub, 0);
            break;
        case CMP_IMM_ACC:
            sim_arithmetic_imm_acc(&cpu, &instruction_t, binary_sub, 0);
            break;
        default:
            printf("; This operation is not yet implemented yet.\n");
            break;
        }
        printf("\n\n");
    }

    printf("; Final register state:\n");
    print_cpu_state(&cpu);

    return 0;
}

static void print_reg_state(cpu_t *cpu, const reg_t *reg_t) {
    printf("%s:%#04x", get_reg_name(reg_t->reg), cpu->regs[reg_t->reg - 1]);
}

void print_cpu_state(cpu_t *cpu) {
    for (u32 i = REG_A; i <= REG_DS; i++) {
        printf("; reg %s state: %#04x\n", get_reg_name(i), cpu->regs[i - 1]);
    }
    printf("; flags: ");
    print_cpu_flags_state(&cpu->flags);
}
