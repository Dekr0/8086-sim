#ifndef TUI_8086_SIM_H
#define TUI_8086_SIM_H

#include "cpu.h"
#include "sim.h"
#include "stdlib.h"

#define FIELD_BG 238
#define TITLE_BG 235
#define FG 231
#define LEFT_UP_CORNER "┌"
#define RIGHT_UP_CORNER "┐"
#define LEFT_BOTTOM_CORNER "└"
#define RIGHT_BOTTOM_CORNER "┘"
#define LINE "─"
#define BAR "│"

void clear_screen();

void draw_tui(sim_ctx_t *ctx);

void hide_cursor();

void show_cursor();

void move_down_begin(u32 line);

int init_tui(char **tty_buf, const size_t size);

#endif
