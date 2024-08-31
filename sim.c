#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "print.h"
#include "sim.h"

typedef struct sim_ctx_t {
    cpu_t *cpu;
    memory_t *mem;
    hashmap_instr_t *stream;
    const instr_t *curr_instr;
    i32 *instr_counts;
    FILE *wf;
} sim_ctx_t;

static void imm_reg_mem_ops_assert(const instr_t *instr_t,
                                   const operand_t *from, const operand_t *to);

static void imm_reg_ops_assert(const instr_t *instr_t, const operand_t *from,
                               const operand_t *to);

static void imm_acc_ops_assert(const instr_t *instr_t, const operand_t *reg_a_t,
                               const operand_t *imm);

static void sim_mov_reg_mem(sim_ctx_t *ctx);

static void sim_mov_imm_reg_mem(sim_ctx_t *ctx);

static void sim_mov_imm_reg(sim_ctx_t *ctx);

static void sim_mov_acc(sim_ctx_t *ctx);

static void sim_arithmetic_reg_mem(sim_ctx_t *ctx, arithmetic arithmetic_func,
                                   i8 write_back);

static void sim_arithmetic_imm_reg_mem(sim_ctx_t *ctx,
                                       arithmetic arithmetic_func,
                                       i8 write_back);

static void sim_arithmetic_imm_acc(sim_ctx_t *ctx, arithmetic arithmetic_func,
                                   i8 write_back);

static void sim_cond_jmp(sim_ctx_t *ctx);

i32 simulate(hashmap_instr_t *stream, memory_t *mem, u8 interact, FILE *f) {
    cpu_t cpu = {0};
    i32 instr_counts = 0;
    for (i32 i = REG_A; i <= REG_DS; i++) {
        cpu.regs[i - 1] = 0;
    }

    char cmd[64] = {0};

    sim_ctx_t ctx = {&cpu, mem, stream, NULL, &instr_counts, f};

    xfprintf(f, "; Execution context:\n");
    while (cpu.ip < mem->source_end) {
        ctx.curr_instr = hashmap_instr_get(stream, cpu.ip);
        assert(ctx.curr_instr != NULL);
        print_instr(ctx.curr_instr, 0, 1, f);

        xfprintf(f, " ; ");
        watch_cpu_ip((&cpu), cpu.ip += ctx.curr_instr->size, f);

        switch (ctx.curr_instr->opcode) {
        case OPCODE_NONE:
            xfprintf(stderr, "; Unknown opcode in simulation\n");
            return 1;
        case OPCODE_MOV_REG_MEM_REG:
        case OPCODE_MOV_REG_MEM_SEG:
            sim_mov_reg_mem(&ctx);
            instr_counts++;
            break;
        case OPCODE_MOV_IMM_REG_MEM:
            sim_mov_imm_reg_mem(&ctx);
            instr_counts++;
            break;
        case OPCODE_MOV_IMM_REG:
            sim_mov_imm_reg(&ctx);
            instr_counts++;
            break;
        case OPCODE_ADD_REG_MEM_REG:
            sim_arithmetic_reg_mem(&ctx, binary_add, 1);
            instr_counts++;
            break;
        case OPCODE_ADD_IMM_REG_MEM:
            sim_arithmetic_imm_reg_mem(&ctx, binary_add, 1);
            instr_counts++;
            break;
        case OPCODE_ADD_IMM_ACC:
            sim_arithmetic_imm_acc(&ctx, binary_add, 1);
            instr_counts++;
            break;
        case OPCODE_SUB_REG_MEM_REG:
            sim_arithmetic_reg_mem(&ctx, binary_sub, 1);
            instr_counts++;
            break;
        case OPCODE_SUB_IMM_REG_MEM:
            sim_arithmetic_imm_reg_mem(&ctx, binary_sub, 1);
            instr_counts++;
            break;
        case OPCODE_SUB_IMM_ACC:
            sim_arithmetic_imm_acc(&ctx, binary_sub, 1);
            instr_counts++;
            break;
        case OPCODE_CMP_REG_MEM_REG:
            sim_arithmetic_reg_mem(&ctx, binary_sub, 0);
            instr_counts++;
            break;
        case OPCODE_CMP_IMM_REG_MEM:
            sim_arithmetic_imm_reg_mem(&ctx, binary_sub, 0);
            instr_counts++;
            break;
        case OPCODE_CMP_IMM_ACC:
            sim_arithmetic_imm_acc(&ctx, binary_sub, 0);
            instr_counts++;
            break;
        case OPCODE_COND_JMP:
            sim_cond_jmp(&ctx);
            instr_counts++;
            break;
        case OPCODE_LOOP_JMP:
            instr_counts++;
            break;
        default:
            xfprintf(stderr, "; This operation is not yet implemented yet.\n");
            break;
        }
        xfprintf(f, "\n");
    }

    print_cpu(&cpu, f);

    return EXIT_SUCCESS;
}

static void imm_reg_mem_ops_assert(const instr_t *i, const operand_t *from,
                                   const operand_t *to) {
    assert(to->type == OPERAND_REG || to->type == OPERAND_EFF_ADDR_EXPR);
    assert(from->type == OPERAND_WORD && from->word.type == WORD_TYPE_IMM);
    if (get_w(i->ctrl_bits)) {
        if (to->type == OPERAND_REG) {
            assert(to->reg->length == BIT_SIZE_16);
        }
    } else {
        assert(from->word.width == BIT_SIZE_8);
        if (to->type == OPERAND_REG) {
            assert(from->word.width == BIT_SIZE_8 && to->reg->reg < REG_BP &&
                   to->reg->length == BIT_SIZE_8);
        }
    }
}

static void imm_reg_ops_assert(const instr_t *i, const operand_t *from,
                               const operand_t *to) {
    assert(to->type == OPERAND_REG);
    assert(from->type == OPERAND_WORD && from->word.type == WORD_TYPE_IMM);
    if (get_w(i->ctrl_bits)) {
        assert(to->reg->length == BIT_SIZE_16);
    } else {
        assert(from->word.width == BIT_SIZE_8 && to->reg->reg < REG_BP &&
               to->reg->length == BIT_SIZE_8);
    }
}

static void imm_acc_ops_assert(const instr_t *i, const operand_t *reg_a_t,
                               const operand_t *imm) {
    assert(reg_a_t->type == OPERAND_REG && reg_a_t->reg->reg == REG_A);
    assert(imm->type == OPERAND_WORD && imm->word.type == WORD_TYPE_IMM);
    if (get_w(i->ctrl_bits)) {
        assert(reg_a_t->reg->length == BIT_SIZE_16);
        assert(imm->word.width == BIT_SIZE_16);
    } else {
        assert(reg_a_t->reg->length == BIT_SIZE_8);
        assert(reg_a_t->reg->offset >= BIT_SIZE_0 ||
               reg_a_t->reg->offset <= BIT_SIZE_8);
        assert(imm->word.width == BIT_SIZE_8);
    }
}

static void sim_mov_reg_mem(sim_ctx_t *ctx) {
    cpu_t *cpu = ctx->cpu;
    const instr_t *i = ctx->curr_instr;
    memory_t *mem = ctx->mem;

    const operand_t *to =
        get_d(i->ctrl_bits) ? &i->operands[0] : &i->operands[1];
    const operand_t *from =
        get_d(i->ctrl_bits) ? &i->operands[1] : &i->operands[0];
    const mod_e mod = get_m(i->ctrl_bits);
    if (mod == MOD_11) {
        assert(to->type == OPERAND_REG && from->type == OPERAND_REG);
        watch_reg_state(to->reg, cpu, write_reg_reg(cpu, to->reg, from->reg),
                        ctx->wf);
        return;
    }
    const bit_width_e bit_width_e =
        get_w(i->ctrl_bits) ? BIT_SIZE_16 : BIT_SIZE_8;
    if (from->type == OPERAND_EFF_ADDR_EXPR) {
        assert(to->type == OPERAND_REG);
        const u32 physical_addr =
            calculate_eff_addr_expr(cpu, &from->expr, mod);
        const u32 val = read_mem(mem, physical_addr, bit_width_e);
        watch_reg_state(to->reg, cpu, write_reg_val(cpu, to->reg, val),
                        ctx->wf);
    } else {
        assert(from->type == OPERAND_REG);
        const u32 physical_addr = calculate_eff_addr_expr(cpu, &to->expr, mod);
        const u32 reg_val = read_reg(cpu, from->reg);
        write_mem(mem, physical_addr, reg_val, bit_width_e);
    }
}

static void sim_mov_imm_reg_mem(sim_ctx_t *ctx) {
    cpu_t *cpu = ctx->cpu;
    const instr_t *i = ctx->curr_instr;
    memory_t *mem = ctx->mem;

    const operand_t *imm = &i->operands[0];
    const operand_t *to = &i->operands[1];
    imm_reg_mem_ops_assert(i, imm, to);

    if (get_m(i->ctrl_bits) == MOD_11) {
        assert(to->type == OPERAND_REG);
        watch_reg_state(to->reg, cpu,
                        write_reg_val(cpu, to->reg, imm->word.value), ctx->wf);
        return;
    }

    assert(to->type == OPERAND_EFF_ADDR_EXPR &&
           to->expr.disp.type == WORD_TYPE_DISP);
    const mod_e mod = get_m(i->ctrl_bits);
    const bit_width_e bit_width_e =
        get_w(i->ctrl_bits) ? BIT_SIZE_16 : BIT_SIZE_8;

    u32 physical_addr = calculate_eff_addr_expr(cpu, &to->expr, mod);

    write_mem(mem, physical_addr, imm->word.value, bit_width_e);
}

static void sim_mov_imm_reg(sim_ctx_t *ctx) {
    cpu_t *cpu = ctx->cpu;
    const instr_t *i = ctx->curr_instr;

    const operand_t *from = &i->operands[0];
    const operand_t *to = &i->operands[1];
    imm_reg_ops_assert(i, from, to);
    watch_reg_state(to->reg, cpu, write_reg_val(cpu, to->reg, from->word.value),
                    ctx->wf);
}

static void sim_mov_acc(sim_ctx_t *ctx) {}

static void sim_arithmetic_reg_mem(sim_ctx_t *ctx, arithmetic arithmetic_func,
                                   i8 write_back) {
    cpu_t *cpu = ctx->cpu;
    const instr_t *i = ctx->curr_instr;
    memory_t *mem = ctx->mem;

    const operand_t *to =
        get_d(i->ctrl_bits) ? &i->operands[0] : &i->operands[1];
    const operand_t *from =
        get_d(i->ctrl_bits) ? &i->operands[1] : &i->operands[0];
    const bit_width_e bit_width_e =
        get_w(i->ctrl_bits) ? BIT_SIZE_16 : BIT_SIZE_8;
    const mod_e mod_e = get_m(i->ctrl_bits);
    if (mod_e == MOD_11) {
        assert(to->type == OPERAND_REG && from->type == OPERAND_REG);
        const u32 from_val = read_reg(cpu, from->reg);
        const u32 to_val = read_reg(cpu, to->reg);
        watch_cpu_flags(&cpu->flags,
                        const u32 result = arithmetic_func(
                            &cpu->flags, to_val, from_val, bit_width_e),
                        ctx->wf);
        if (write_back) {
            watch_reg_state(to->reg, cpu, write_reg_val(cpu, to->reg, result),
                            ctx->wf);
        }
        return;
    }

    u32 physical_addr = 0;
    if (to->type == OPERAND_EFF_ADDR_EXPR) {
        assert(from->type == OPERAND_REG);
        physical_addr = calculate_eff_addr_expr(cpu, &to->expr, mod_e);
        const u32 to_val = read_mem(mem, physical_addr, bit_width_e);
        const u32 from_val = read_reg(cpu, from->reg);
        watch_cpu_flags(&cpu->flags,
                        const u32 result = arithmetic_func(
                            &cpu->flags, to_val, from_val, bit_width_e),
                        ctx->wf);
        write_mem(mem, physical_addr, result, bit_width_e);
    } else {
        assert(from->type == OPERAND_EFF_ADDR_EXPR);
        physical_addr = calculate_eff_addr_expr(cpu, &from->expr, mod_e);
        const u32 to_val = read_reg(cpu, to->reg);
        const u32 from_val = read_mem(mem, physical_addr, bit_width_e);
        watch_cpu_flags(&cpu->flags,
                        const u32 result = arithmetic_func(
                            &cpu->flags, to_val, from_val, bit_width_e),
                        ctx->wf);
        watch_reg_state(to->reg, cpu, write_reg_val(cpu, to->reg, result),
                        ctx->wf);
    }
}

static void sim_arithmetic_imm_reg_mem(sim_ctx_t *ctx,
                                       arithmetic arithmetic_func,
                                       i8 write_back) {
    cpu_t *cpu = ctx->cpu;
    const instr_t *i = ctx->curr_instr;

    const operand_t *from = &i->operands[0];
    const operand_t *to = &i->operands[1];

    imm_reg_mem_ops_assert(i, from, to);

    const mod_e mod_e = get_m(i->ctrl_bits);
    const bit_width_e bit_width_e =
        get_w(i->ctrl_bits) ? BIT_SIZE_16 : BIT_SIZE_8;

    u32 from_val = from->word.value;
    if (get_s(i->ctrl_bits) && get_w(i->ctrl_bits) && from_val >> 7 & 1) {
        from_val |= 0xff00;
    }

    if (mod_e == MOD_11) {
        assert(to->type == OPERAND_REG);
        const u32 to_val = read_reg(cpu, to->reg);
        watch_cpu_flags(&cpu->flags,
                        const u32 result = arithmetic_func(
                            &cpu->flags, to_val, from_val, bit_width_e),
                        ctx->wf);
        if (write_back) {
            watch_reg_state(to->reg, cpu, write_reg_val(cpu, to->reg, result),
                            ctx->wf);
        }
        return;
    }

    assert(to->type == OPERAND_EFF_ADDR_EXPR);
    const u32 physical_addr = calculate_eff_addr_expr(cpu, &to->expr, mod_e);
    const u32 to_val = read_mem(ctx->mem, physical_addr, bit_width_e);
    watch_cpu_flags(&cpu->flags,
                    const u32 result = arithmetic_func(&cpu->flags, to_val,
                                                       from_val, bit_width_e),
                    ctx->wf);
    write_mem(ctx->mem, physical_addr, result, bit_width_e);
}

static void sim_arithmetic_imm_acc(sim_ctx_t *ctx, arithmetic arithmetic_func,
                                   i8 write_back) {
    cpu_t *cpu = ctx->cpu;
    const instr_t *i = ctx->curr_instr;

    const operand_t *reg_a_t = &i->operands[0];
    const operand_t *imm = &i->operands[1];
    const bit_width_e bit_width_e =
        get_w(i->ctrl_bits) ? BIT_SIZE_16 : BIT_SIZE_8;

    imm_acc_ops_assert(i, reg_a_t, imm);

    const u32 reg_a_val = read_reg(cpu, reg_a_t->reg);
    watch_cpu_flags(&cpu->flags,
                    const u32 result = arithmetic_func(
                        &cpu->flags, reg_a_val, imm->word.value, bit_width_e),
                    ctx->wf);
    if (write_back) {
        watch_reg_state(reg_a_t->reg, cpu,
                        write_reg_val(cpu, reg_a_t->reg, result), ctx->wf);
    }
}

static void sim_cond_jmp(sim_ctx_t *ctx) {
    cpu_t *cpu = ctx->cpu;
    const instr_t *i = ctx->curr_instr;
    hashmap_instr_t *stream_t = ctx->stream;
    i32 new_ip = 0;
    const cond_jmp_e cond_jmp_e = decode_cond_jump_variant(i->raw_ctrl_bits1);

    switch (cond_jmp_e) {
    case COND_JMP_NE:
        if (!(get_zf(cpu->flags))) {
            new_ip = cpu->ip + (i8)i->raw_ctrl_bits2;
        } else {
            return;
        }
        break;
    }
    assert(new_ip >= 0);
    const instr_t *jmp_dest_instr = hashmap_instr_get(ctx->stream, new_ip);
    assert(jmp_dest_instr != NULL);
    watch_cpu_ip(cpu, cpu->ip = new_ip, ctx->wf);
}
