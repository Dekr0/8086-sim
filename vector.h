#ifndef VECTOR_TEMPLATE_H
#define VECTOR_TEMPLATE_H

#include "instruction.h"

typedef struct vector_instr_t vector_instr_t;

vector_instr_t *init_vector_instr(const uint8_t K);

void free_vector_instr(vector_instr_t *v);

int8_t vector_instr_append(vector_instr_t *v, const instr_t i);

int8_t vector_instr_pop(vector_instr_t *v);

instr_t *vector_instr_get(vector_instr_t *v, const uint64_t i);

uint64_t vector_instr_cap(vector_instr_t *v);

uint64_t vector_instr_size(vector_instr_t *v);

typedef struct vector_instr_p_t vector_instr_p_t;

vector_instr_p_t *init_vector_instr_p(const uint8_t K);

void free_vector_instr_p(vector_instr_p_t *v);

int8_t vector_instr_p_append(vector_instr_p_t *v, instr_t *i);

instr_t *vector_instr_p_get(vector_instr_p_t *v, const uint64_t i);

int8_t vector_instr_p_pop(vector_instr_p_t *v);

uint64_t vector_instr_p_cap(vector_instr_p_t *v);

uint64_t vector_instr_p_size(vector_instr_p_t *v);

#endif
