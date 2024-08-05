#include <assert.h>
#include <stdio.h>

#include "sim.h"
#include "instruction.h"
#include "table.h"


i32 simulate(src_instructions_t *src, memory_t *mem) {
    cpu_t cpu = { 0 };

    instruction_t i;
    operand_t from;
    operand_t to;
    for (u32 c = 0; c < src->count; c++) {
        i = src->i_arr[c];
        switch (i.opcode) {
        case OP_NONE:
            printf("Unknown opcode make it to the simulation from decoding \
                    stage\n");
            return 1;
            break;
        case MOV_REG_MEM_REG:
            to = get_d(i.ctrl_bits) ? i.operands[0] : i.operands[1];
            from = get_d(i.ctrl_bits) ? i.operands[1] : i.operands[0];
            switch (get_m(i.ctrl_bits)) {
            case MOD_11:
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
            assert(to.type == OPERAND_REG && to.reg.reg != REG_NONE);
            assert(from.type == OPERAND_WORD && from.word.type == WORD_TYPE_IMM);
            if (get_w(i.ctrl_bits)) {
                assert(to.reg.length == 2);
                assert(from.word.width == BIT_SIZE_16);
                cpu.regs[to.reg.reg] = from.word.value;
            } else {
                assert(from.word.width == BIT_SIZE_8);
                assert(to.reg.reg < REG_BP && to.reg.length == 1);
                if (to.reg.offset) {
                    // high
                    cpu.regs[to.reg.reg] &= 0x00ff;
                    cpu.regs[to.reg.reg] |= (from.word.value & 0xff) << 8;
                } else {
                    cpu.regs[to.reg.reg] &= 0xff00;
                    cpu.regs[to.reg.reg] |= from.word.value & 0xff;
                }
            }
            break;
        default:
            printf("This operation is not yet implemented yet.\n");
            break;
        }
    }

    print_regs_state(&cpu);

    return 0;
}

void print_regs_state(cpu_t *c) {
    for (u32 i = 1; i <= REG_DS; i++) {
        printf("register %s state: %d\n", reg_name[i], c->regs[i]);
    }
}
