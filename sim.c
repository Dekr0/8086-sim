#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "arithmetic.h"
#include "hashtable.h"
#include "instruction.h"
#include "print.h"
#include "sim.h"
#include "tui.h"

static void imm_reg_mem_ops_assert(const instr_t *instr_t,
                                   const operand_t *from, const operand_t *to);

static void imm_reg_ops_assert(const instr_t *instr_t, const operand_t *from,
                               const operand_t *to);

static void imm_acc_ops_assert(const instr_t *instr_t, const operand_t *reg_a_t,
                               const operand_t *imm);

static void estimate_ea_clock(clock_counter_t *cc, const eff_addr_expr_t *e,
                              const mod_e mod);

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

void init_sim_opts(sim_opts_t *sim_opts, const opts_t *opts) {
    memset(sim_opts, 0, sizeof(sim_opts_t));
    sim_opts->interact = opts->interact;
    sim_opts->op_mode = opts->op_mode;
    sim_opts->is8088 = opts->is8088;
    sim_opts->auto_run = opts->auto_run;
    sim_opts->auto_run_timeout = opts->auto_run_timeout;
    sim_opts->f = opts->decode_o;
}

i32 simulate(const hashmap_instr_t *stream, memory_t *mem,
             const sim_opts_t *opts) {
    FILE *f = opts->f;

    cpu_t *cpu = calloc(1, sizeof(cpu_t));
    if (cpu == NULL)
        return EXIT_FAILURE;
    cpu->op_mode = opts->op_mode;
    cpu->is8088 = opts->is8088;

    sim_ctx_t *ctx = calloc(1, sizeof(sim_ctx_t));
    if (ctx == NULL)  {
        free(cpu);
        return EXIT_FAILURE;
    }

    tui_t *tui = NULL;
    if (opts->interact && ((tui = init_tui()) == NULL)) {
        free(cpu);
        free(ctx);
        return EXIT_FAILURE;
    }

    ctx->cpu = cpu;
    ctx->mem = mem;
    ctx->stream = stream;
    ctx->curr_instr = NULL;
    ctx->f = f;

    u32 *IP = &cpu->BIU.IP;

    i8 exit = EXIT_SUCCESS;
    xfprintf(f, "; Execution context:\n");

    ctx->asm_viewer_instr = hashmap_instr_get(stream, *IP);
    while (*IP < mem->source_end && exit != EXIT_FAILURE) {
        ctx->curr_instr = hashmap_instr_get(stream, *IP);
        assert(ctx->curr_instr != NULL);

        if (tui && opts->interact) {
            render(tui, ctx);
            usleep(opts->auto_run_timeout);
            u32 key = opts->auto_run ? get_input_nblock(tui) : get_input(tui);
            if (key == 'q') {
                break;
            } else if (key == 'h' && ctx->memory_viewer_addr >= 32) {
                ctx->memory_viewer_addr -= 32;
                continue;
            } else if (key == 'l' && ctx->memory_viewer_addr + 32 < MEM_SIZE_8086) {
                ctx->memory_viewer_addr += 32;
                continue;
            } else if (key == 'j' && ctx->asm_viewer_instr->next != NULL) {
                ctx->asm_viewer_instr = ctx->asm_viewer_instr->next;
                continue;
            } else if (key == 'k' && ctx->asm_viewer_instr->prev != NULL) {
                ctx->asm_viewer_instr = ctx->asm_viewer_instr->prev;
                continue;
            } else if (!opts->auto_run && key != 'n') {
                continue;
            }
        }

        ctx->asm_viewer_instr = ctx->curr_instr;
        print_instr(ctx->curr_instr, 0, 1, f);

        xfprintf(f, " ; ");
        watch_cpu_ip(cpu, *IP += ctx->curr_instr->size, f);


        switch (ctx->curr_instr->opcode) {
        case OPCODE_NONE:
            xfprintf(stderr, "; Unknown opcode in simulation\n");
            exit = EXIT_FAILURE;
            break;
        case OPCODE_MOV_REG_MEM_REG:
        case OPCODE_MOV_REG_MEM_SEG:
            sim_mov_reg_mem(ctx);
            break;
        case OPCODE_MOV_IMM_REG_MEM:
            sim_mov_imm_reg_mem(ctx);
            break;
        case OPCODE_MOV_IMM_REG:
            sim_mov_imm_reg(ctx);
            break;
        case OPCODE_MOV_ACC:
            sim_mov_acc(ctx);
            break;
        case OPCODE_ADD_REG_MEM_REG:
            sim_arithmetic_reg_mem(ctx, binary_add, 1);
            break;
        case OPCODE_ADD_IMM_REG_MEM:
            sim_arithmetic_imm_reg_mem(ctx, binary_add, 1);
            break;
        case OPCODE_ADD_IMM_ACC:
            sim_arithmetic_imm_acc(ctx, binary_add, 1);
            break;
        case OPCODE_SUB_REG_MEM_REG:
            sim_arithmetic_reg_mem(ctx, binary_sub, 1);
            break;
        case OPCODE_SUB_IMM_REG_MEM:
            sim_arithmetic_imm_reg_mem(ctx, binary_sub, 1);
            break;
        case OPCODE_SUB_IMM_ACC:
            sim_arithmetic_imm_acc(ctx, binary_sub, 1);
            break;
        case OPCODE_CMP_REG_MEM_REG:
            sim_arithmetic_reg_mem(ctx, binary_sub, 0);
            break;
        case OPCODE_CMP_IMM_REG_MEM:
            sim_arithmetic_imm_reg_mem(ctx, binary_sub, 0);
            break;
        case OPCODE_CMP_IMM_ACC:
            sim_arithmetic_imm_acc(ctx, binary_sub, 0);
            break;
        case OPCODE_COND_JMP:
            sim_cond_jmp(ctx);
            break;
        case OPCODE_LOOP_JMP:
            break;
        default:
            xfprintf(stderr, "; This operation is not yet implemented yet.\n");
            break;
        }
        xfprintf(f, "\n");

        ctx->clock_count += ctx->cc.base + ctx->cc.ea + ctx->cc.transfer +
                       ctx->cc.odd_transfer_penalty + ctx->cc.ref_penalty;

        memset(&ctx->cc, 0, sizeof(clock_counter_t));
    }

    print_cpu(cpu, f);
    fprintf(f, "\n; Estimated EU's clock cycles: %ld", ctx->clock_count);

    if (cpu != NULL)
        free(cpu);
    if (ctx != NULL)
        free(ctx);
    if (tui != NULL)
        free_tui(tui);

    return exit;
}

static void estimate_ea_clock(clock_counter_t *cc, const eff_addr_expr_t *e,
                              const mod_e mod) {
    if (mod == MOD_11)
        assert(0);

    const reg_t *t0 = e->terms[0];
    const reg_t *t1 = e->terms[1];
    switch (mod) {
    case MOD_00:
        if (e->da) {
            cc->ea = 6;
            return;
        }
        if (t0->reg == REG_NONE || t1->reg == REG_NONE) {
            cc->ea = 5;
            return;
        }
        u8 min = (t0->reg == REG_BP && t1->reg == REG_DI) ||
                 (t0->reg == REG_B && t1->reg == REG_SI);
        if (!min)
            assert((t0->reg == REG_BP && t1->reg == REG_SI) ||
                   (t0->reg == REG_B && t1->reg == REG_DI));
        cc->ea = 7 * min + 8 * !min;
        return;
    case MOD_01:
    case MOD_10:
        if (t0->reg == REG_NONE || t1->reg == REG_NONE) {
            cc->ea = 9;
            return;
        }
        min = (t0->reg == REG_BP && t1->reg == REG_DI) ||
              (t0->reg == REG_B && t1->reg == REG_SI);
        if (!min)
            assert((t0->reg == REG_BP && t1->reg == REG_SI) ||
                   (t0->reg == REG_B && t1->reg == REG_DI));
        cc->ea = 11 * min + 12 * !min;
        return;
    }
}

static void imm_reg_mem_ops_assert(const instr_t *i, const operand_t *from,
                                   const operand_t *to) {
    assert(to->type == OPERAND_REG || to->type == OPERAND_EFF_ADDR_EXPR);
    assert(from->type == OPERAND_WORD && from->word.type == WORD_TYPE_IMM);
    if (get_w(i->ctrl_bits)) {
        if (to->type == OPERAND_REG)
            assert(to->reg->length == BIT_SIZE_16);
    } else {
        assert(from->word.width == BIT_SIZE_8);
        if (to->type == OPERAND_REG)
            assert(from->word.width == BIT_SIZE_8 && to->reg->reg < REG_BP &&
                   to->reg->length == BIT_SIZE_8);
    }
}

static void imm_reg_ops_assert(const instr_t *i, const operand_t *from,
                               const operand_t *to) {
    assert(to->type == OPERAND_REG);
    assert(from->type == OPERAND_WORD && from->word.type == WORD_TYPE_IMM);
    if (get_w(i->ctrl_bits))
        assert(to->reg->length == BIT_SIZE_16);
    else
        assert(from->word.width == BIT_SIZE_8 && to->reg->reg < REG_BP &&
               to->reg->length == BIT_SIZE_8);
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
    clock_counter_t *cc = &ctx->cc;
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
                        ctx->f);
        cc->base = 2;
        return;
    }
    const bit_width_e bit_width_e =
        get_w(i->ctrl_bits) ? BIT_SIZE_16 : BIT_SIZE_8;

    ctx->cc.transfer = 1;

    if (from->type == OPERAND_EFF_ADDR_EXPR) {
        assert(to->type == OPERAND_REG);
        const u32 physical_addr =
            calculate_eff_addr_expr(cpu, &from->expr, mod);
        const u32 val = read_mem(mem, physical_addr, bit_width_e);
        watch_reg_state(to->reg, cpu, write_reg_val(cpu, to->reg, val), ctx->f);

        /** Clock estimation */
        cc->base = 8;
        estimate_ea_clock(cc, &from->expr, mod);
        cc->ref_penalty =
            4 * (from->expr.disp.width == BIT_SIZE_16 && cpu->is8088);
        cc->odd_transfer_penalty = 4 * (!cpu->is8088 && (physical_addr & 1));
        return;
    }

    assert(from->type == OPERAND_REG);
    const u32 physical_addr = calculate_eff_addr_expr(cpu, &to->expr, mod);
    const u32 reg_val = read_reg(cpu, from->reg);
    write_mem(mem, physical_addr, reg_val, bit_width_e);

    /** Clock estimation */
    cc->base = 9;
    estimate_ea_clock(cc, &to->expr, mod);
    cc->ref_penalty = 4 * (from->expr.disp.width == BIT_SIZE_16 && cpu->is8088);
    cc->odd_transfer_penalty = 4 * (!cpu->is8088 && (physical_addr & 1));
}

static void sim_mov_imm_reg_mem(sim_ctx_t *ctx) {
    clock_counter_t *cc = &ctx->cc;
    cpu_t *cpu = ctx->cpu;
    const instr_t *i = ctx->curr_instr;
    memory_t *mem = ctx->mem;

    const operand_t *imm = &i->operands[0];
    const operand_t *to = &i->operands[1];
    imm_reg_mem_ops_assert(i, imm, to);

    if (get_m(i->ctrl_bits) == MOD_11) {
        assert(to->type == OPERAND_REG);
        watch_reg_state(to->reg, cpu,
                        write_reg_val(cpu, to->reg, imm->word.value), ctx->f);
        cc->base = 4;
        return;
    }

    assert(to->type == OPERAND_EFF_ADDR_EXPR &&
           to->expr.disp.type == WORD_TYPE_DISP);
    const mod_e mod = get_m(i->ctrl_bits);
    const bit_width_e bit_width_e =
        get_w(i->ctrl_bits) ? BIT_SIZE_16 : BIT_SIZE_8;

    u32 physical_addr = calculate_eff_addr_expr(cpu, &to->expr, mod);

    write_mem(mem, physical_addr, imm->word.value, bit_width_e);

    /** Clock estimation */
    cc->base = 10;
    estimate_ea_clock(cc, &to->expr, mod);
    cc->ref_penalty = 4 * (to->expr.disp.width == BIT_SIZE_16 && cpu->is8088);
    cc->odd_transfer_penalty = 4 * (!cpu->is8088 && (physical_addr & 1));
    cc->transfer = 1;
}

static void sim_mov_imm_reg(sim_ctx_t *ctx) {
    cpu_t *cpu = ctx->cpu;
    const instr_t *i = ctx->curr_instr;

    const operand_t *from = &i->operands[0];
    const operand_t *to = &i->operands[1];
    imm_reg_ops_assert(i, from, to);

    watch_reg_state(to->reg, cpu, write_reg_val(cpu, to->reg, from->word.value),
                    ctx->f);

    /** Clock estimation */
    ctx->cc.base = 4;
}

static void sim_mov_acc(sim_ctx_t *ctx) {
    clock_counter_t *cc = &ctx->cc;
    memory_t *mem = ctx->mem;
    cpu_t *cpu = ctx->cpu;
    const instr_t *i = ctx->curr_instr;

    assert(i->operands[0].type == OPERAND_REG &&
           i->operands[0].reg->reg == REG_A);
    assert(i->operands[1].type == OPERAND_WORD &&
           i->operands[1].word.type == WORD_TYPE_ADDR);

    const u8 d = get_d(i->ctrl_bits);
    const mod_e mod = get_m(i->ctrl_bits);
    const bit_width_e bit_width_e =
        get_w(i->ctrl_bits) ? BIT_SIZE_16 : BIT_SIZE_8;

    /** Clock estimation */
    cc->base = 10;
    cc->transfer = 1;

    const reg_t *reg_a = i->operands[0].reg;
    const eff_addr_expr_t *addr = &i->operands[1].expr;
    if (d) { // memory to acc
        const u32 physical_addr = calculate_eff_addr_expr(cpu, addr, mod);
        const u32 val = read_mem(mem, physical_addr, bit_width_e);
        watch_reg_state(reg_a, cpu, write_reg_val(cpu, reg_a, val), ctx->f);
        return;
    }

    // acc to memory
    const u32 physical_addr = calculate_eff_addr_expr(cpu, addr, mod);
    const u32 reg_val = read_reg(cpu, reg_a);
    write_mem(mem, physical_addr, reg_val, bit_width_e);
}

static void sim_arithmetic_reg_mem(sim_ctx_t *ctx, arithmetic arithmetic_func,
                                   i8 write_back) {
    clock_counter_t *cc = &ctx->cc;
    cpu_t *cpu = ctx->cpu;
    const instr_t *i = ctx->curr_instr;
    memory_t *mem = ctx->mem;

    const operand_t *to =
        get_d(i->ctrl_bits) ? &i->operands[0] : &i->operands[1];
    const operand_t *from =
        get_d(i->ctrl_bits) ? &i->operands[1] : &i->operands[0];
    const bit_width_e bit_width_e =
        get_w(i->ctrl_bits) ? BIT_SIZE_16 : BIT_SIZE_8;

    const mod_e mod = get_m(i->ctrl_bits);
    if (mod == MOD_11) {
        assert(to->type == OPERAND_REG && from->type == OPERAND_REG);

        const u32 from_val = read_reg(cpu, from->reg);
        const u32 to_val = read_reg(cpu, to->reg);
        watch_cpu_flags(&cpu->EU.flags,
                        const u32 result = arithmetic_func(
                            &cpu->EU.flags, to_val, from_val, bit_width_e),
                        ctx->f);
        if (write_back) {
            watch_reg_state(to->reg, cpu, write_reg_val(cpu, to->reg, result),
                            ctx->f);
        }

        /** Clock Estimation */
        switch (i->opcode) {
        case OPCODE_ADD_REG_MEM_REG:
            cc->base = 3;
            break;
        case OPCODE_CMP_REG_MEM_REG:
            cc->base = 3;
            break;
        case OPCODE_SUB_REG_MEM_REG:
            cc->base = 3;
            break;
        default:
            break;
        }

        return;
    }

    u32 physical_addr = 0;

    if (to->type == OPERAND_EFF_ADDR_EXPR) {
        assert(from->type == OPERAND_REG);
        physical_addr = calculate_eff_addr_expr(cpu, &to->expr, mod);
        const u32 to_val = read_mem(mem, physical_addr, bit_width_e);
        const u32 from_val = read_reg(cpu, from->reg);
        watch_cpu_flags(&cpu->EU.flags,
                        const u32 result = arithmetic_func(
                            &cpu->EU.flags, to_val, from_val, bit_width_e),
                        ctx->f);
        write_mem(mem, physical_addr, result, bit_width_e);

        /** Clock Estimation */
        switch (i->opcode) {
        case OPCODE_ADD_REG_MEM_REG:
            cc->base = 16;
            cc->transfer = 2;
        case OPCODE_CMP_REG_MEM_REG:
            cc->base = 9;
            cc->transfer = 1;
        case OPCODE_SUB_REG_MEM_REG:
            cc->base = 16;
            cc->transfer = 2;
            break;
        default:
            break;
        }
        cc->ref_penalty =
            4 * (from->expr.disp.width == BIT_SIZE_16 && cpu->is8088);
        cc->odd_transfer_penalty = 4 * (!cpu->is8088 && (physical_addr & 1));
        estimate_ea_clock(cc, &from->expr, mod);

    } else {
        assert(from->type == OPERAND_EFF_ADDR_EXPR);
        physical_addr = calculate_eff_addr_expr(cpu, &from->expr, mod);
        const u32 to_val = read_reg(cpu, to->reg);
        const u32 from_val = read_mem(mem, physical_addr, bit_width_e);
        watch_cpu_flags(&cpu->EU.flags,
                        const u32 result = arithmetic_func(
                            &cpu->EU.flags, to_val, from_val, bit_width_e),
                        ctx->f);
        watch_reg_state(to->reg, cpu, write_reg_val(cpu, to->reg, result),
                        ctx->f);

        /** Clock Estimation */
        switch (i->opcode) {
        case OPCODE_ADD_REG_MEM_REG:
            cc->base = 9;
            cc->transfer = 1;
            break;
        case OPCODE_SUB_REG_MEM_REG:
            cc->base = 9;
            cc->transfer = 1;
            break;
        case OPCODE_CMP_REG_MEM_REG:
            cc->base = 9;
            cc->transfer = 1;
            break;
        default:
            break;
        }
        cc->ref_penalty =
            4 * (from->expr.disp.width == BIT_SIZE_16 && cpu->is8088);
        cc->odd_transfer_penalty = 4 * (!cpu->is8088 && (physical_addr & 1));
        estimate_ea_clock(cc, &from->expr, mod);
    }
}

static void sim_arithmetic_imm_reg_mem(sim_ctx_t *ctx,
                                       arithmetic arithmetic_func,
                                       i8 write_back) {
    clock_counter_t *cc = &ctx->cc;
    cpu_t *cpu = ctx->cpu;
    const instr_t *i = ctx->curr_instr;

    const operand_t *from = &i->operands[0];
    const operand_t *to = &i->operands[1];

    imm_reg_mem_ops_assert(i, from, to);

    const mod_e mod = get_m(i->ctrl_bits);
    const bit_width_e bit_width_e =
        get_w(i->ctrl_bits) ? BIT_SIZE_16 : BIT_SIZE_8;

    u32 from_val = from->word.value;
    if (get_s(i->ctrl_bits) && get_w(i->ctrl_bits) && from_val >> 7 & 1) {
        from_val |= 0xff00;
    }

    if (mod == MOD_11) {
        assert(to->type == OPERAND_REG);
        const u32 to_val = read_reg(cpu, to->reg);
        watch_cpu_flags(&cpu->EU.flags,
                        const u32 result = arithmetic_func(
                            &cpu->EU.flags, to_val, from_val, bit_width_e),
                        ctx->f);
        if (write_back) {
            watch_reg_state(to->reg, cpu, write_reg_val(cpu, to->reg, result),
                            ctx->f);
        }
        return;

        /** Clock estimation */
        switch (i->opcode) {
        case OPCODE_ADD_IMM_REG_MEM:
            cc->base = 4;
            break;
        case OPCODE_CMP_IMM_REG_MEM:
            cc->base = 4;
            break;
        case OPCODE_SUB_IMM_REG_MEM:
            cc->base = 4;
            break;
        default:
            break;
        }
    }

    assert(to->type == OPERAND_EFF_ADDR_EXPR);
    const u32 physical_addr = calculate_eff_addr_expr(cpu, &to->expr, mod);
    const u32 to_val = read_mem(ctx->mem, physical_addr, bit_width_e);
    watch_cpu_flags(&cpu->EU.flags,
                    const u32 result = arithmetic_func(&cpu->EU.flags, to_val,
                                                       from_val, bit_width_e),
                    ctx->f);
    write_mem(ctx->mem, physical_addr, result, bit_width_e);

    /** Clock estimation */
    switch (i->opcode) {
    case OPCODE_ADD_IMM_REG_MEM:
        cc->base = 17;
        break;
    case OPCODE_CMP_IMM_REG_MEM:
        cc->base = 10;
        break;
    case OPCODE_SUB_IMM_REG_MEM:
        cc->base = 17;
        break;
    default:
        break;
    }

    cc->ref_penalty = 4 * (from->expr.disp.width == BIT_SIZE_16 && cpu->is8088);
    cc->odd_transfer_penalty = 4 * (!cpu->is8088 && (physical_addr & 1));
    estimate_ea_clock(cc, &from->expr, mod);
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
    watch_cpu_flags(&cpu->EU.flags,
                    const u32 result =
                        arithmetic_func(&cpu->EU.flags, reg_a_val,
                                        imm->word.value, bit_width_e),
                    ctx->f);
    if (write_back) {
        watch_reg_state(reg_a_t->reg, cpu,
                        write_reg_val(cpu, reg_a_t->reg, result), ctx->f);
    }

    /** Clock Estimation */
    switch (i->opcode) {
    case OPCODE_ADD_IMM_ACC:
        ctx->cc.base = 4;
        break;
    case OPCODE_CMP_IMM_ACC:
        ctx->cc.base = 4;
        break;
    case OPCODE_SUB_IMM_ACC:
        ctx->cc.base = 4;
        break;
    default:
        break;
    }
}

static void sim_cond_jmp(sim_ctx_t *ctx) {
    cpu_t *cpu = ctx->cpu;
    const instr_t *i = ctx->curr_instr;
    i32 new_ip = 0;
    const cond_jmp_e cond_jmp_e = decode_cond_jump_variant(i->raw_ctrl_bits1);

    switch (cond_jmp_e) {
    case COND_JMP_NE:
        if (!(get_zf(cpu->EU.flags))) {
            new_ip = cpu->BIU.IP + (i8)i->raw_ctrl_bits2;
        } else {
            return;
        }
        break;
    default:
        break;
    }
    assert(new_ip >= 0);
    const instr_t *jmp_dest_instr = hashmap_instr_get(ctx->stream, new_ip);
    assert(jmp_dest_instr != NULL);
    watch_cpu_ip(cpu, cpu->BIU.IP = new_ip, ctx->f);
}
