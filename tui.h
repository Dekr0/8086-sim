#ifndef TUI_8086_SIM_H
#define TUI_8086_SIM_H

#include <stdio.h>

#include "cpu.h"
#include "sim.h"
#include "type.h"

typedef struct tui_t tui_t;

typedef struct theme_rgb_32_t theme_rgb_32_t;

typedef struct dim_t dim_t;

i8 render(tui_t *t, const sim_ctx_t *ctx);

u32 get_input(tui_t *t);

u32 get_input_nblock(tui_t *t);

tui_t *init_tui();

void free_tui(tui_t *tui);

#endif
