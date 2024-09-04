#include "instruction.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211

#include "hashtable.h"

typedef struct hashmap_instr_t {
    u64 size;
    u64 cap;
    instr_t *map;
} hashmap_instr_t;

static u64 instr_hasing(const u64 base_addr);

static void _hashmap_instr_put(instr_t *map, const instr_t i, u64 *size,
                               const u64 cap);

static i8 hashmap_instr_resize(hashmap_instr_t *h_t);

static u64 instr_hasing(const u64 base_addr) {
    u64 hash = FNV_OFFSET;
    hash ^= base_addr & 0xff;
    hash *= FNV_OFFSET; // 1st byte
    hash ^= base_addr >> 8 & 0xff;
    hash *= FNV_OFFSET; // 2nd byte
    hash ^= base_addr >> 16 & 0xff;
    hash *= FNV_OFFSET; // 3rd byte
    hash ^= base_addr >> 24 & 0xff;
    hash *= FNV_OFFSET; // 4th byte
    hash ^= base_addr >> 32 & 0xff;
    hash *= FNV_OFFSET; // 5th byte
    hash ^= base_addr >> 40 & 0xff;
    hash *= FNV_OFFSET; // 6th byte
    hash ^= base_addr >> 48 & 0xff;
    hash *= FNV_OFFSET; // 7th byte
    hash ^= base_addr >> 56 & 0xff;
    hash *= FNV_OFFSET; // 8th byte
    return hash;
}

static void _hashmap_instr_put(instr_t *map, const instr_t i, u64 *size,
                               const u64 cap) {
    u64 hash = instr_hasing(i.base_addr);
    u64 idx = hash & (cap - 1);
    // Loop until there is an empty slot
    while (map[idx].opcode != OPCODE_NONE) {
        if (i.base_addr == map[idx].base_addr)
            return deep_copy_instr(map + idx, &i);
        idx++;
        if (idx >= cap)
            idx = 0; // At the end, wrap around
    }
    deep_copy_instr(map + idx, &i);
    if (size == NULL)
        return;
    *size += 1;
}

static i8 hashmap_instr_resize(hashmap_instr_t *h_t) {
    if (h_t == NULL)
        return 0;
    u64 new_cap = h_t->cap * 2;
    // Overflow (Capacity is would be too big)
    if (new_cap < h_t->cap)
        return -1;
    instr_t *new_h = calloc(new_cap, sizeof(instr_t));
    if (new_h == NULL)
        return -1;
    for (u64 idx = 0; idx < h_t->cap; idx++) {
        instr_t i = h_t->map[idx];
        if (i.opcode != OPCODE_NONE)
            _hashmap_instr_put(new_h, i, NULL, new_cap);
    }
    free(h_t->map);
    h_t->map = new_h;
    h_t->cap = new_cap;
    return 0;
}

hashmap_instr_t *init_hashmap_instr() {
    hashmap_instr_t *h = calloc(1, sizeof(hashmap_instr_t));
    if (h == NULL)
        return NULL;
    if ((h->map = calloc(16, sizeof(instr_t))) == NULL) {
        free(h);
        return NULL;
    }
    h->cap = 16;
    return h;
}

void free_hashmap_instr(hashmap_instr_t *h) {
    if (h == NULL)
        return;
    if (h->map != NULL)
        free(h->map);
    free(h);
}

instr_t *hashmap_instr_get(const hashmap_instr_t *h, const u64 base_addr) {
    u64 hash = instr_hasing(base_addr);
    u64 idx = hash & (h->cap - 1);
    while (h->map[idx].opcode != OPCODE_NONE) {
        if (h->map[idx].base_addr == base_addr)
            return &h->map[idx];
        idx++; // Key is not in this slot, move to next slot (linear probing).
        if (idx >= h->cap)
            idx = 0; // At the end, wrap around
    }
    return NULL;
}

i8 hashmap_instr_put(hashmap_instr_t *h, const instr_t i) {
    if (h == NULL)
        return 0;
    if (i.opcode == OPCODE_NONE)
        return 0;
    if (h->size >= h->cap / 2 && hashmap_instr_resize(h) == -1)
        return -1;
    _hashmap_instr_put(h->map, i, &h->size, h->cap);
    return 0;
}

u64 hashmap_instr_cap(const hashmap_instr_t *h) { return h->cap; }

u64 hashmap_instr_size(const hashmap_instr_t *h) { return h->size; }
