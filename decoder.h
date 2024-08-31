#ifndef DECODER_8086_SIM_H
#define DECODER_8086_SIM_H

#include <stdio.h>

#include "hashtable.h"
#include "memory.h"

hashmap_instr_t *load_instr_stream(memory_t *mem_t);

#endif
