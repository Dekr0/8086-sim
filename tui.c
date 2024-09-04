#define _GNU_SOURCE
#include <assert.h>
#include <locale.h>
#include <notcurses/notcurses.h>
#include <stdlib.h>
#include <unistd.h>

#include "string_builder.h"
#include "print.h"
#include "table.h"
#include "sim.h"
#include "tui.h"

typedef struct tui_t {
    struct notcurses *nc;
    struct ncplane *p;
    struct ncinput ni;
    str_builder_t *b;
    struct nccell box_line[6];
} tui_t;

typedef struct theme_rgb_32_t {
    u32 fg;
    u32 bg;
} theme_rgb_32_t;

typedef struct dim_t {
    u32 width;
    u32 height;
} dim_t;

const theme_rgb_32_t field_theme = { 0xffffff, 0x262424 };
const theme_rgb_32_t title_theme = { 0x00141A, 0x1B6497 };

static i8 set_theme_rgb(struct ncplane *p, const struct theme_rgb_32_t *theme);
static i32 draw_reg_info_title(struct ncplane *p, const char *fmt, ...);
static i32 draw_reg_field(struct ncplane *p, const char *fmt, ...);
static i32 draw_memory_byte(struct ncplane *p, const u8 *byte, const u8 src);
static i32 draw_default_gap(struct ncplane *p, const u16 len);
static i32 draw_current_instr(struct ncplane *p, const char *str);
static i32 draw_instr(struct ncplane *p, const char *str);
static i32 draw_addr(struct ncplane *p, const u32 addr);
static void warmup_input(tui_t *t);
static void render_essential(tui_t *t, const sim_ctx_t *ctx, dim_t *d);
static void render_asm(tui_t *t, const sim_ctx_t *ctx, dim_t *d);
static void render_memory(tui_t *t, const sim_ctx_t *ctx, dim_t *d);
static void reset_theme(struct ncplane *p);

static i8 set_theme_rgb(struct ncplane *p, const struct theme_rgb_32_t *theme) {
    return ncplane_set_fg_rgb(p, theme->fg) || ncplane_set_bg_rgb(p, theme->bg);
}

static void reset_theme(struct ncplane *p) {
    ncplane_set_fg_default(p); 
    ncplane_set_bg_default(p);
}

static i32 draw_reg_info_title(struct ncplane *p, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    set_theme_rgb(p, &title_theme);
    i32 np = ncplane_vprintf(p, fmt, va);
    va_end(va);
    return np;
}

static i32 draw_reg_field(struct ncplane *p, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    set_theme_rgb(p, &field_theme);
    i32 np = ncplane_vprintf(p, fmt, va);
    va_end(va);
    return np;
}

static i32 draw_memory_byte(struct ncplane *p, const u8 *byte, const u8 src) {
    if (src)
        ncplane_set_fg_rgb(p, 0xB02669);
    else
        ncplane_set_fg_rgb(p, 0x2AA198);

    i32 np = ncplane_printf(p, "%.2x", *byte);
    return np;
}

static i32 draw_current_instr(struct ncplane *p, const char *str) {
    set_theme_rgb(p, &field_theme);
    i32 np = ncplane_putstr(p, str);
    return np;
}

static i32 draw_addr(struct ncplane *p, const u32 addr) {
    ncplane_set_fg_rgb(p, 0x268BD2);
    i32 np = ncplane_printf(p, "0x%.4x ", addr);
    return np;
}

static i32 draw_instr(struct ncplane *p, const char *str) {
    ncplane_set_fg_rgb(p, 0xCB4B16);
    ncplane_set_bg_default(p);
    i32 np = ncplane_putstr(p, str);
    return np;
}

static i32 draw_default_gap(struct ncplane *p, const u16 len) {
    reset_theme(p);
    return ncplane_printf(p, "%*s", len, "");
}

i8 render(tui_t *t, const sim_ctx_t *ctx) {
    ncplane_cursor_move_yx(t->p, 0, 0);

    struct dim_t essential_dim = { 0 };
    struct dim_t asm_dim = { 0 };
    struct dim_t memory_dim = { 0 };

    render_essential(t, ctx, &essential_dim);

    asm_dim.height = essential_dim.height;

    ncplane_cursor_move_yx(t->p, 0, 0);
    ncplane_cursor_move_rel(t->p, 0, essential_dim.width);

    render_asm(t, ctx, &asm_dim);

    ncplane_cursor_move_yx(t->p, essential_dim.height + 1, 0);

    memory_dim.width = essential_dim.width + asm_dim.width;
    memory_dim.height = 16;
    render_memory(t, ctx, &memory_dim);

    notcurses_render(t->nc);

    return 0;
}

void warmup_input(tui_t *t) {
    i8 i = 10;
    while (i-- > 0) notcurses_get_nblock(t->nc, NULL);
}

u32 get_input(tui_t *t) {
    return notcurses_get_blocking(t->nc, &t->ni);
}

u32 get_input_nblock(tui_t *t) {
    return notcurses_get_nblock(t->nc, &t->ni);
}

/***************
 * REG  H   L  *    PI           SEGMENT
 *  A   00  00 *    SP  0000       ES     0000
 *  B   00  00 *        0000
 *  C   00  00 *
 *  D   00  00 *
 *
 * TF  DF  IF  OF  SF  ZF  AF  PF  CF *  *  IP  *
 * 0   0   0   0   0   0   0   0   0  *  * 0000 *
 *
 *************  ***************  ****************
 * CPU Cycle *  * Instr. Size *  * Machine Code *
 *************  ***************  ****************
 * 000000000 *  *      6      *  *              *
 *************  ***************  ****************
 */
static void render_essential(tui_t *t, const sim_ctx_t *ctx, dim_t *d) {
    struct ncplane *p = t->p;

    const cpu_t *c = ctx->cpu;
    const instr_t *instr = ctx->curr_instr;

    i16 np = 0,  width = 0;

    ncplane_cursor_move_rel(p, 1, 0);

    /** Register Title */
    np += ncplane_putc(p, &t->box_line[5]);

    np += draw_reg_info_title(p, " REG  H   L  ");
    np += draw_default_gap(p, 3);

    np += draw_reg_info_title(p, " PI       ");
    np += draw_default_gap(p, 3);

    np += draw_reg_info_title(p, " SEGMENT       ");

    reset_theme(p);
    np += ncplane_putc(p, &t->box_line[5]);


    /** Top Border */
    width = np;
    ncplane_cursor_move_rel(p, -1, -width);
    ncplane_putc(p, &t->box_line[0]);
    for (i8 i = 0; i < width - 2; i++) {
        ncplane_putc(p, &t->box_line[4]);
        if (i == 0) {
            ncplane_set_fg_rgb(p, 0x859900);
            i += ncplane_putstr(p, "CPU");
            continue;
        }
    }
    ncplane_putc(p, &t->box_line[1]);

    /** Register Content */
    ncplane_cursor_move_rel(p, 2, -width);
    np = 0;
    u16 reg_val = 0;
    for (i8 i = 0; i < 4; i++) {
        np = 0;

        np += ncplane_putc(p, &t->box_line[5]);

        reg_val = c->EU.regs[i];
        np += draw_reg_field(p, "  %s   %.2x  %.2x ", get_reg_name(i + REG_A),
                           (u8)(reg_val >> 8), (u8)(reg_val & 0xff));

        np += draw_default_gap(p, 3);
            
        reg_val = c->EU.regs[i + REG_SP - 1];
        np += draw_reg_field(p, " %s  %.4x ", get_reg_name(i + REG_SP), reg_val);
        
        np += draw_default_gap(p, 3);

        reg_val = c->BIU.regs[i];
        np += draw_reg_field(p, "   %s     %.4x ", get_reg_name(i + REG_ES), reg_val);

        np += ncplane_putc(p, &t->box_line[5]);

        ncplane_cursor_move_rel(p, 1, -np);
    }

    /** New line */
    np = 0;
    ncplane_putc(p, &t->box_line[5]);
    ncplane_cursor_move_rel(p, 0, width - 2);
    ncplane_putc(p, &t->box_line[5]);
    ncplane_cursor_move_rel(p, 1, -width);
    np = 0;


    /** Flags and IP Title */
    np += ncplane_putc(p, &t->box_line[5]);
    np = draw_reg_info_title(p, " TF  DF  IF  OF  SF  ZF  AF  PF  CF ");

    np += draw_default_gap(p, 2);

    np += draw_reg_info_title(p, "  IP  ");

    np += ncplane_putc(p, &t->box_line[5]);


    /** Flags and IP content */
    ncplane_cursor_move_rel(p, 1, -np-1);
    np = 0;
    np += ncplane_putc(p, &t->box_line[5]);
    set_theme_rgb(p, &field_theme);
    for (i8 i = 8; i >= 0; i--) {
        np += ncplane_printf(p, " %d  ", (c->EU.flags >> i & 1));
    }

    np += draw_default_gap(p, 2);

    np += draw_reg_field(p, " %.4x ", c->BIU.IP);

    np += ncplane_putc(p, &t->box_line[5]);

    ncplane_cursor_move_rel(p, 1, -np);
    np = 0;


    /** New line */
    ncplane_putc(p, &t->box_line[5]);
    ncplane_cursor_move_rel(p, 0, width - 2);
    ncplane_putc(p, &t->box_line[5]);
    ncplane_cursor_move_rel(p, 1, -width);


    /** Other information title */
    np += ncplane_putc(p, &t->box_line[5]);

    np += draw_reg_info_title(p, " CPU Cycle ");
    np += draw_default_gap(p, 2);

    np += draw_reg_info_title(p, " Instr. Size ");
    np += draw_default_gap(p, 2);

    np += draw_reg_info_title(p, "  Machine Code  ");

    np += ncplane_putc(p, &t->box_line[5]);
    ncplane_cursor_move_rel(p, 1, -np);
    np = 0;


    /** Other information field */
    np += ncplane_putc(p, &t->box_line[5]);

    np += draw_reg_field(p, " %.9x ", ctx->clock_count);
    np += draw_default_gap(p, 2);

    np += draw_reg_field(p, "      %d      ", instr->size);
    np += draw_default_gap(p, 2);

    u64 machine_code = 0;
    for (i8 i = 0; i < instr->size - 1; i++) {
        machine_code |= (u64) (ctx->mem->mem[instr->base_addr + i]);
        machine_code <<= 8;
    }

    np += draw_reg_field(p, "  %.12x  ", machine_code);

    np += ncplane_putc(p, &t->box_line[5]);

    ncplane_cursor_move_rel(p, 1, -np);
    np = 0;

    /** Bottom Border */
    np += ncplane_putc(p, &t->box_line[2]);
    for (i8 i = 0; i < width - 2; i++)
        ncplane_putc(p, &t->box_line[4]);
    ncplane_putc(p, &t->box_line[3]);

    d->width = width;
    d->height = ncplane_cursor_y(p);
}

static void render_asm(tui_t *t, const sim_ctx_t *ctx, dim_t *d) {
    struct ncplane *p = t->p;

    i16 np = 0, width = 58;

    d->width = width;

    /** Top Border */
    ncplane_putc(p, &t->box_line[0]);
    for (i8 i = 0; i < width - 2; i++) {
        ncplane_putc(p, &t->box_line[4]);
        if (i == 0) {
            ncplane_set_fg_rgb(p, 0x859900);
            ncplane_set_bg_default(p);
            i += ncplane_putstr(p, "Assembly");
            ncplane_set_fg_default(p);
            continue;
        }
    }
    ncplane_putc(p, &t->box_line[1]);
    ncplane_cursor_move_rel(p, 1, -width);


    /** ASM Code */
    i8 idx = 0;
    const i8 old_history_count = d->height / 2;
    const instr_t *viewer_instr = ctx->asm_viewer_instr;
    while (idx++ < old_history_count && viewer_instr->prev != NULL)
        viewer_instr = viewer_instr->prev;
    idx = 0;
    while (idx++ < d->height - 1 && viewer_instr != NULL) {
        np += ncplane_putc(p, &t->box_line[5]);
        if (sprint_instr(viewer_instr, 0, 0, t->b)) {
            np += draw_current_instr(p, "Error when formating instruction");
        } else {
            np += draw_addr(p, viewer_instr->base_addr);
            if (viewer_instr == ctx->curr_instr) {
                np += draw_current_instr(p, build(t->b, NULL));
            } else {
                np += draw_instr(p, build(t->b, NULL));
            }
            reset(t->b);
        }
        np += draw_default_gap(p, width - np - 1);
        np += ncplane_putc(p, &t->box_line[5]);
        viewer_instr = viewer_instr->next;
        ncplane_cursor_move_rel(p, 1, -width);
        np = 0;
    }
    while (idx++ < d->height) {
        ncplane_putc(p, &t->box_line[5]);
        draw_default_gap(p, width - 2);
        ncplane_putc(p, &t->box_line[5]);
        ncplane_cursor_move_rel(p, 1, -width);
    }


    /** Bottom Border */
    np += ncplane_putc(p, &t->box_line[2]);
    for (i8 i = 0; i < width - 2; i++)
        ncplane_putc(p, &t->box_line[4]);
    ncplane_putc(p, &t->box_line[3]);
}

static void render_memory(tui_t *t, const sim_ctx_t *ctx, dim_t *d) {
    struct ncplane *p = t->p;
    const u32 source_end = ctx->mem->source_end;
    const u8 *mem = ctx->mem->mem;
    u32 np = 0;

    ncplane_putc(p, &t->box_line[0]);
    for (i8 i = 0; i < d->width - 2; i++) {
        ncplane_putc(p, &t->box_line[4]);
        if (i == 0) {
            ncplane_set_fg_rgb(p, 0x859900);
            ncplane_set_bg_default(p);
            i += ncplane_putstr(p, "Memory");
            ncplane_set_fg_default(p);
            continue;
        }
    }
    ncplane_putc(p, &t->box_line[1]);
    ncplane_cursor_move_rel(p, 1, -d->width);
    
    u32 line_display = 0; 
    u32 offset = ctx->memory_viewer_addr;

    while (line_display++ < d->height) {
        np += ncplane_putc(p, &t->box_line[5]);
        np += draw_addr(p, offset);
        for (; np < d->width && offset < 1048576; offset++) {
            np += draw_memory_byte(p, mem + offset, offset <= source_end);
            if (np == d->width - 1)
                np += ncplane_putc(p, &t->box_line[5]);
            else
                np += draw_default_gap(p, 1);
        }
        if (np < d->width) {
            np += draw_default_gap(p, d->width - np - 1);
            np += ncplane_putc(p, &t->box_line[5]);
        }
        ncplane_cursor_move_rel(p, 1, -np);
        np = 0;
    }

    np += ncplane_putc(p, &t->box_line[2]);
    for (i8 i = 0; i < d->width - 2; i++)
        ncplane_putc(p, &t->box_line[4]);
    ncplane_putc(p, &t->box_line[3]);
}


tui_t *init_tui() {
    setlocale(LC_ALL, "");

    tui_t *t = (tui_t *)calloc(1, sizeof(tui_t));
    if (t == NULL) {
        return NULL;
    }

    struct notcurses *nc = notcurses_init(NULL, NULL);
    if (nc == NULL) {
        fprintf(stderr, "notcurses_init error\n");
        free(t);
        return NULL;
    }
    t->nc = nc;

    struct ncplane *p = notcurses_stdplane(nc);
    if (p == NULL) {
        fprintf(stderr, "notcurses_stdplane error\n");
        notcurses_stop(nc);
        free(t);
        return NULL;
    }
    t->p = p;

    if ((t->b = init_str_builder_t()) == NULL) {
        fprintf(stderr, "notcurses_stdplane error\n");
        notcurses_stop(nc);
        free(t);
        return NULL;
    }

    const char *lines[6] = {"┌", "┐", "└", "┘", "─", "│"};
    for (u8 i = 0; i < 6; i++) {
        nccell_init(t->box_line + i);
        if (nccell_load(p, t->box_line + i, lines[i]) == -1) {
            goto cleanup;
        }
        nccell_set_fg_rgb(t->box_line + i, 0x859900);
    }

    warmup_input(t);

    return t;
cleanup:
    if (nc != NULL)
        notcurses_stop(nc);
    if (t != NULL)
        free(t);
    return NULL;
}

void free_tui(tui_t *tui) {
    if (tui == NULL)
        return;
    tui->nc != NULL &&notcurses_stop(tui->nc);
    free(tui);
}
