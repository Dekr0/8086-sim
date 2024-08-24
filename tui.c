#include <assert.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "print.h"
#include "table.h"
#include "tui.h"
#include "type.h"

typedef struct cursor_t {
    i32 col;
    i32 row;
} cursort_t;


void clear_screen() {
    printf("\x1b[2J");
    fflush(stdout);
}

void reset_color_mode() { printf("\x1b[0m"); }

void set_fg_256(u8 fg) { printf("\x1b[38;5;%um", fg); }

void set_bg_256(u8 bg) { printf("\x1b[48;5;%um", bg); }

void set_scheme(u8 fg, u8 bg) {
    set_fg_256(fg);
    set_bg_256(bg);
}

void hide_cursor() { printf("\x1b[?25l"); }

void show_cursor() { printf("\x1b[?25h"); }

void move_zero() { printf("\x1b[H"); }

void move_cursor(const u32 line, const u32 col) {
    printf("\x1b[%u;%uH", line, col);
}

void move_up(u32 line) { printf("\x1b[%uA", line); }

void move_up_begin(u32 line) { printf("\x1b[%uF", line); }

void move_right(u32 col) { printf("\x1b[%uC", col); }

void move_down(u32 line) { printf("\x1b[%uB", line); }

void move_down_begin(u32 line) { printf("\x1b[%uE", line); }

void move_left(u32 col) { printf("\x1b[%uD", col); }

void erase_end_of_line() { printf("\x1b[0K"); }

void save_cursor() { printf("\x1b[s"); }

void restore_cursor() { printf("\x1b[u"); }

void draw_reg(cpu_t *cpu) {
    /** UI
     *  REG  H   L     PI            SEGM
     *   A   00  00    SP    0000    ES      0000
     *   C   00  00    BP    0000    CS      0000
     *   D   00  00    SI    0000    SS      0000
     *   B   00  00    DI    0000    DS      0000
     */

    /** Table Title */
    set_scheme(FG, TITLE_BG);
    printf(" REG  H   L  ");
    reset_color_mode();
    printf("  ");

    set_scheme(FG, TITLE_BG);
    printf(" %-11s", "PI");
    reset_color_mode();
    printf("  ");

    set_scheme(FG, TITLE_BG);
    printf(" %-13s", "SEGM");

    move_down_begin(1);
    move_right(1);

    /** Table Content */
    for (u8 i = 0; i < 4; i++) {
        set_scheme(FG, FIELD_BG);
        printf("  %s   %.2x  %.2x ", get_reg_name(REG_A + i),
               (u8)(cpu->regs[i] >> 8), (u8)(cpu->regs[i]));
        reset_color_mode();
        printf("  ");

        set_scheme(FG, FIELD_BG);
        printf(" %-6s%.4x ", get_reg_name(REG_A + 4 + i), cpu->regs[i + 4]);
        reset_color_mode();
        printf("  ");

        set_scheme(FG, FIELD_BG);
        printf(" %-8s%.4x ", get_reg_name(REG_A + 8 + i), cpu->regs[i + 8]);
        move_down_begin(1);
        move_right(1);
    }
    reset_color_mode();
}

void draw_flags(const u16 *flags) {
    set_scheme(FG, TITLE_BG);
    printf("  ");
    for (i8 i = 8; i >= 0; i--) {
        printf("%-4s", get_flags_names(i));
    }
    move_down_begin(1);
    move_right(1);
    set_scheme(FG, FIELD_BG);
    printf("  ");
    for (i8 i = 8; i >= 0; i--) {
        printf("%-4d", (*flags) >> i & 1);
    }
    reset_color_mode();
}

void draw_ip(const u16 ip) {
    set_scheme(FG, TITLE_BG);
    move_left(printf("  %-3s ", "IP"));
    set_scheme(FG, FIELD_BG);
    move_down(1);
    printf(" %.4x ", ip);
    reset_color_mode();
}

void draw_cpu(cpu_t *cpu) {
    /**
     * ---------------------------------------------
     * |REG  H   L     PI            SEGM          |
     * | A   00  00    SP    0000    ES      0000  |
     * | C   00  00    BP    0000    CS      0000  |
     * | D   00  00    SI    0000    SS      0000  |
     * | B   00  00    DI    0000    DS      0000  |
     * |                                           |
     * |TF  DF  IF  OF  SF  ZF  AF  PF  CF      IP |
     * |0   0   0   0   0   0   0   0   0      0000|
     * ---------------------------------------------
     */

    set_fg_256(FG);
    printf(LEFT_UP_CORNER);
    for (u32 i = 1; i < 48 - 1; i++) {
        printf(LINE);
    }
    printf(RIGHT_UP_CORNER);
    move_down_begin(1);
    for (u32 i = 0; i < 8; i++) {
        printf(BAR);
        move_right(46);
        printf(BAR);
        move_down_begin(1);
    }

    move_cursor(2, 2);
    draw_reg(cpu);

    move_down_begin(1);
    move_right(1);
    draw_flags(&cpu->flags);

    move_up(1);
    move_right(2);
    draw_ip(cpu->ip);

    set_fg_256(FG);
    move_down_begin(1);
    printf(LEFT_BOTTOM_CORNER);
    for (u32 i = 1; i < 48 - 1; i++) {
        printf(LINE);
    }
    printf(RIGHT_BOTTOM_CORNER);
    reset_color_mode();
}

void draw_asm(instr_stream_t *stream_t, const i32 cursor) {
    struct winsize wz;
    ioctl(0, TIOCGWINSZ, &wz);

    set_fg_256(FG);
    printf(LEFT_UP_CORNER);
    for (u32 i = 1; i < wz.ws_col - 1; i++) {
        printf(LINE);
    }
    printf(RIGHT_UP_CORNER);
    move_down_begin(1);

    i32 i = cursor - 4 >= 0 ? cursor - 4 : 0;
    i32 nprint = 0;
    const i32 upper = cursor + 4 < stream_t->count ? cursor + 4 : stream_t->count;
    for ( ; i < upper; i++) {
        printf(BAR);
        printf("%#04x -> ", stream_t->stream[i].base_addr);
        if (i == cursor) {
           set_bg_256(FIELD_BG); 
           print_instr(&stream_t->stream[i], 0, 0, stdout);
           reset_color_mode();
           set_fg_256(FG);
        } else {
           print_instr(&stream_t->stream[i], 0, 0, stdout);
        }
        move_down_begin(1);
        move_up_begin(1);
        move_right(wz.ws_col - 1);
        printf(BAR);
        move_down_begin(1);
    }

    printf(LEFT_BOTTOM_CORNER);
    for (u32 i = 1; i < wz.ws_col - 1; i++) {
        printf(LINE);
    }
    printf(RIGHT_BOTTOM_CORNER);
    reset_color_mode();
}

void draw_tui(sim_ctx_t *ctx) {
    move_zero();
    draw_cpu(ctx->cpu);
    move_down_begin(1);
    draw_asm(ctx->stream, *ctx->stream_cursor);
    move_down_begin(1);
}

int init_tui(char **tty_buf, const size_t size) {
    tty_buf = calloc(size, sizeof(char));
    if (tty_buf == NULL) {
        fprintf(stderr, "buffer calloc error\n");
        return EXIT_FAILURE;
    }
    setvbuf(stdout, *tty_buf, _IOFBF, size);
    return EXIT_SUCCESS;
}
