#ifndef HASHTABLE_TEMPLATE_H
#define HASHTABLE_TEMPLATE_H

#include "instruction.h"

/** Context: There are number of instructions. Each instruction is guarantee to
 * have an unique base address. Some instructions are jump instructions. They
 * will jump to other instruction, either conditionally or unconditionally.
 *
 * During simulation and outputting decoded result, the program will use IP
 * register to achieve in-order traversal through hash table because each
 * instruction size and instruction base address is known, and IP register
 * always starts with the base address of the first instruction, which can be
 * used as a key to look up instruction in the hash table, and obtain its
 * instruction size. This eliminate the needs of having an additional vector to
 * traverse instruction stream in order.*/

/** Collision handling: Linear Probing. If there's already an item with the
 * same key occupies this slot, simply move to the next slot. If the next slot
 * is full too, move along again, until there's an empty one. Make sure to wrap
 * around if hitting the end because index generate from hashing function will
 * be most likely in the middle of the array.
 *
 * Why linear probing over bucket for each key? Cache friendly if bucket is
 * implemented with linked list, and memory save if bucket is implemented with
 * dynamic array.*/

/** Expansion factor: one half */

typedef struct hashmap_instr_t hashmap_instr_t;

hashmap_instr_t *init_hashmap_instr();

void free_hashmap_instr(hashmap_instr_t *h);

instr_t *hashmap_instr_get(hashmap_instr_t *h, const u64 base_addr);

i8 hashmap_instr_put(hashmap_instr_t *h, const instr_t i);

u64 hashmap_instr_cap(hashmap_instr_t *h);

u64 hashmap_instr_size(hashmap_instr_t *h);

#endif
