#include <assert.h>

#include "cpu.h"
#include "instruction.h"
#include "memory.h"
#include "print.h"
#include "sim.h"

i32 simulate(instr_stream_t *stream_t, memory_t *mem, u8 interactive,
             FILE *wf) {
    cpu_t cpu = {0};
    i32 stream_cursor = 0;
    for (i32 i = REG_A; i <= REG_DS; i++) {
        cpu.regs[i - 1] = 0;
    }

    sim_ctx_t ctx = {&cpu, mem, stream_t, NULL, &stream_cursor, wf};

    fprintf(wf, "; Execution context:\n");
    for (; stream_cursor < stream_t->count;) {
        ctx.curr_instr = &stream_t->stream[stream_cursor];
        print_instr(ctx.curr_instr, 0, 1, wf);
        fprintf(wf, " ; ");
        watch_cpu_ip((&cpu), cpu.ip += ctx.curr_instr->size, wf);

        switch (ctx.curr_instr->opcode) {
        case OP_NONE:
            fprintf(stderr, "; Unknown opcode in simulation\n");
            return 1;
        case MOV_REG_MEM_REG:
        case MOV_REG_MEM_SEG:
            sim_mov_reg_mem(&ctx);
            stream_cursor++;
            break;
        case MOV_IMM_REG_MEM:
            sim_mov_imm_reg_mem(&ctx);
            stream_cursor++;
            break;
        case MOV_IMM_REG:
            sim_mov_imm_reg(&ctx);
            stream_cursor++;
            break;
        case ADD_REG_MEM_REG:
            sim_arithmetic_reg_mem(&ctx, binary_add, 1);
            stream_cursor++;
            break;
        case ADD_IMM_REG_MEM:
            sim_arithmetic_imm_reg_mem(&ctx, binary_add, 1);
            stream_cursor++;
            break;
        case ADD_IMM_ACC:
            sim_arithmetic_imm_acc(&ctx, binary_add, 1);
            stream_cursor++;
            break;
        case SUB_REG_MEM_REG:
            sim_arithmetic_reg_mem(&ctx, binary_sub, 1);
            stream_cursor++;
            break;
        case SUB_IMM_REG_MEM:
            sim_arithmetic_imm_reg_mem(&ctx, binary_sub, 1);
            stream_cursor++;
            break;
        case SUB_IMM_ACC:
            sim_arithmetic_imm_acc(&ctx, binary_sub, 1);
            stream_cursor++;
            break;
        case CMP_REG_MEM_REG:
            sim_arithmetic_reg_mem(&ctx, binary_sub, 0);
            stream_cursor++;
            break;
        case CMP_IMM_REG_MEM:
            sim_arithmetic_imm_reg_mem(&ctx, binary_sub, 0);
            stream_cursor++;
            break;
        case CMP_IMM_ACC:
            sim_arithmetic_imm_acc(&ctx, binary_sub, 0);
            stream_cursor++;
            break;
        case COND_JMP:
            sim_cond_jmp(&ctx);
            break;
        case LOOP_JMP:
            break;
        default:
            printf("; This operation is not yet implemented yet.\n");
            break;
        }
        fprintf(wf, "\n");
    }

    fprintf(wf, "\n; Final register state:\n");
    print_cpu(&cpu, wf);

    return 0;
}

static void imm_reg_mem_ops_assert(instr_t *i, operand_t *from, operand_t *to) {
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

static void imm_reg_ops_assert(instr_t *i, operand_t *from, operand_t *to) {
    assert(to->type == OPERAND_REG);
    assert(from->type == OPERAND_WORD && from->word.type == WORD_TYPE_IMM);
    if (get_w(i->ctrl_bits)) {
        assert(to->reg->length == BIT_SIZE_16);
    } else {
        assert(from->word.width == BIT_SIZE_8 && to->reg->reg < REG_BP &&
               to->reg->length == BIT_SIZE_8);
    }
}

static void imm_acc_ops_assert(instr_t *i, operand_t *reg_a_t, operand_t *imm) {
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
    instr_t *i = ctx->curr_instr;
    memory_t *mem = ctx->mem;

    operand_t *to = get_d(i->ctrl_bits) ? &i->operands[0] : &i->operands[1];
    operand_t *from = get_d(i->ctrl_bits) ? &i->operands[1] : &i->operands[0];
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
    instr_t *i = ctx->curr_instr;
    memory_t *mem = ctx->mem;

    operand_t *imm = &i->operands[0];
    operand_t *to = &i->operands[1];
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
    instr_t *i = ctx->curr_instr;

    operand_t *from = &i->operands[0];
    operand_t *to = &i->operands[1];
    imm_reg_ops_assert(i, from, to);
    watch_reg_state(to->reg, cpu, write_reg_val(cpu, to->reg, from->word.value),
                    ctx->wf);
}

static void sim_mov_acc(sim_ctx_t *ctx) {}

static void sim_arithmetic_reg_mem(sim_ctx_t *ctx, arithmetic arithmetic_func,
                                   i8 write_back) {
    cpu_t *cpu = ctx->cpu;
    instr_t *i = ctx->curr_instr;
    memory_t *mem = ctx->mem;

    operand_t *to = get_d(i->ctrl_bits) ? &i->operands[0] : &i->operands[1];
    operand_t *from = get_d(i->ctrl_bits) ? &i->operands[1] : &i->operands[0];
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
    instr_t *i = ctx->curr_instr;

    operand_t *from = &i->operands[0];
    operand_t *to = &i->operands[1];

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
    instr_t *i = ctx->curr_instr;

    operand_t *reg_a_t = &i->operands[0];
    operand_t *imm = &i->operands[1];
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
    instr_t *i = ctx->curr_instr;
    instr_stream_t *stream_t = ctx->stream;
    i32 *stream_cursor = ctx->stream_cursor;

    i32 new_ip = 0;
    const cond_jmp_e cond_jmp_e = decode_cond_jump_variant(i->raw_ctrl_bits1);

    switch (cond_jmp_e) {
    case COND_JMP_NE:
        if (!(get_zf(cpu->flags))) {
            new_ip = cpu->ip + (i8)i->raw_ctrl_bits2;
        } else {
            *stream_cursor += 1;
            return;
        }
        break;
    }
    assert(new_ip >= 0);
    const i32 f = new_ip > cpu->ip ? 1 : -1;
    for (; *stream_cursor >= 0 && *stream_cursor < stream_t->count;
         *stream_cursor += f) {
        if (stream_t->stream[*stream_cursor].base_addr == new_ip) {
            watch_cpu_ip(cpu, cpu->ip = new_ip, ctx->wf);
            return;
        }
    }
}
