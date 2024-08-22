#ifndef ARITHMETIC_SIM_8086_H
#define ARITHMETIC_SIM_8086_H

#include "type.h"

typedef u32 (*arithmetic)(u16 *, u32, u32, const bit_width_e);

u32 binary_add(u16 *flags_reg, u32 a, u32 b, const bit_width_e width_e);

u32 binary_sub(u16 *flags_reg, u32 a, u32 b, const bit_width_e bit_width_e);

#endif
