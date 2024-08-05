#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "instruction.h"

src_instructions_t *init_src_instruction(instruction_t *i, u32 count) {
    src_instructions_t *src = calloc(1, sizeof ( src_instructions_t ));
    if (src == NULL) {
        printf("init_src_instruction: calloc error\n");
        return NULL;
    }
    src->i_arr = realloc(i, count * sizeof( instruction_t ));
    if (src->i_arr == NULL) {
        printf("init_src_instruction: realloc error. Extra memory is being\
                occupied\n");
        return NULL;
    }
    src->count = count;
    return src;
}

void free_src_instruction(src_instructions_t *src) {
    assert(src);
    assert(src->i_arr);

    free(src->i_arr);
    free(src);
}
