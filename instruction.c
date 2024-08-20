#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "instruction.h"

instr_stream_t *init_instr_stream(instr_t *i, u32 count) {
    instr_stream_t *stream_t = calloc(1, sizeof(instr_stream_t));
    if (stream_t == NULL) {
        printf("init_src_instruction: calloc error\n");
        return NULL;
    }
    stream_t->stream = realloc(i, count * sizeof(instr_t));
    if (stream_t->stream == NULL) {
        printf("init_src_instruction: realloc error. Extra memory is being\
                occupied\n");
        return NULL;
    }
    stream_t->count = count;
    return stream_t;
}

void free_instr_stream(instr_stream_t *stream_t) {
    assert(stream_t);
    assert(stream_t->stream);

    free(stream_t->stream);
    free(stream_t);
}
