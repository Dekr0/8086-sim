#include <assert.h>

#include "arithmetic.h"
#include "cpu.h"

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
        bit_a = a & 1;
        a >>= 1;
        bit_b = b & 1;
        b >>= 1;
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
        if ((sum >> i) & 1) {
            one_counts++;
        }
    }

    set_cf(*flags_reg, carry_bit);
    set_pf(*flags_reg, !(one_counts % 2));
    set_af(*flags_reg, aux_flag);
    set_zf(*flags_reg, !sum);
    set_sf(*flags_reg, sum_sign_bit);
    set_of(*flags_reg, (!a_sign_bit & !b_sign_bit & sum_sign_bit) |
                           (a_sign_bit & b_sign_bit & !sum_sign_bit));
    return sum & 0xffff;
}

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
        if (borrow_out && i == 4) {
            aux_flag = 1;
        }
        bit_a = a & 1;
        a >>= 1;
        bit_b = b & 1;
        b >>= 1;
        diff |= (bit_a ^ bit_b ^ borrow_out) << i;
        borrow_out =
            ((!bit_a) & borrow_out) | ((!bit_a) & bit_b) | (bit_b & borrow_out);
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
        if ((diff >> i) & 1) {
            one_counts++;
        }
    }

    set_cf(*flags_reg, borrow_out);
    set_pf(*flags_reg, !(one_counts % 2));
    set_af(*flags_reg, aux_flag);
    set_zf(*flags_reg, !diff);
    set_sf(*flags_reg, diff_sign_bit);
    set_of(*flags_reg, (b_sign_bit & !a_sign_bit & diff_sign_bit) |
                           (a_sign_bit & !b_sign_bit & !diff_sign_bit));
    return diff & 0xffff;
}
