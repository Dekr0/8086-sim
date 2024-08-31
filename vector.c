#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "instruction.h"
#include "vector.h"

/** Credit to:
 * https://ece.uwaterloo.ca/~dwharder/aads/Algorithms/Array_resizing/src/resizing.array.h
 */
u64 const CAPACITY_ARRAY[5][60] = {
    //                          _  k
    // The closest integer to \/2 2  for non-powers-of-two (every second).
    //   - 2.4 copies per insertion
    //   - 18% empty on average
    {4,          6, // Initial capacity is 8; these are used for downsizing
     8,          11,        16,        23,        32,        45,
     64,         91,        128,       181,       256,       362,
     512,        724,       1024,      1448,      2048,      2896,
     4096,       5793,      8192,      11585,     16384,     23170,
     32768,      46341,     65536,     92682,     131072,    185364,
     262144,     370728,    524288,    741455,    1048576,   1482910,
     2097152,    2965821,   4194304,   5931642,   8388608,   11863283,
     16777216,   23726566,  33554432,  47453133,  67108864,  94906266,
     134217728,  189812531, 268435456, 379625062, 536870912, 759250125,
     1073741824, 1518500250},

    // Using a factor of 1.5
    //   - 2 copies per insertion
    //   - 22% empty on average
    {3,          5, // Initial capacity is 8; these are used for downsizing
     8,          12,        18,        27,        41,         62,
     93,         140,       210,       315,       473,        710,
     1065,       1598,      2397,      3596,      5394,       8091,
     12137,      18206,     27309,     40964,     61446,      92169,
     138254,     207381,    311072,    466608,    699912,     1049868,
     1574802,    2362203,   3543305,   5314958,   7972437,    11958656,
     17937984,   26906976,  40360464,  60540696,  90811044,   136216566,
     204324849,  306487274, 459730911, 689596367, 1034394551, 1551591827,
     2327387741, 3491081612},

    //                         3_   2k + 1        3_  2k + 1
    // The closest integer to \/4  2       and 2 \/2 2       for
    // non-odd-powers-of-two (every third).
    //   - 1.7 copies per insertion
    //   - 25% empty on average
    {5,         6, // Initial capacity is 8; these are used for downsizing
     8,         13,        20,        32,        51,        81,
     128,       203,       323,       512,       813,       1290,
     2048,      3251,      5161,      8192,      13004,     20643,
     32768,     52016,     82570,     131072,    208064,    330281,
     524288,    832255,    1321123,   2097152,   3329021,   5284492,
     8388608,   13316085,  21137968,  33554432,  53264341,  84551870,
     134217728, 213057363, 338207482, 536870912, 852229450, 1352829926},

    // Using a factor of 2
    //   - 1 copy per insertion
    //   - 39% empty on average
    {2,        4, // Initial capacity is 8; these are used for downsizing
     8,        16,       32,       64,        128,       256,       512,
     1024,     2048,     4096,     8192,      16384,     32768,     65536,
     131072,   262144,   524288,   1048576,   2097152,   4194304,   8388608,
     16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 1073741824},

    // Using a factor of infinity
    //    - 0 copies per insertion
    {0, 0, // These two ensures that resizing never occurs
     1073741824}};

typedef struct vector_instr_t {
    u8 K;
    u8 cap_idx;
    u64 cap;
    u64 size;
    instr_t *vector;
} vector_instr_t;

static int8_t vector_instr_resize(vector_instr_t *v, const int8_t delta);

static int8_t vector_instr_resize(vector_instr_t *v, const int8_t delta) {
    if (v == NULL)
        return 0;
    const u8 new_cap_idx = v->cap_idx + delta;
    instr_t *old_vector = v->vector;
    v->vector =
        realloc(v->vector, sizeof(instr_t) * CAPACITY_ARRAY[v->K][new_cap_idx]);
    if (v->vector == NULL) {
        v->vector = old_vector;
        return -1;
    }
    v->cap_idx = new_cap_idx;
    v->cap = CAPACITY_ARRAY[v->K][v->cap_idx];
    return 0;
}

vector_instr_t *init_vector_instr(const u8 K) {
    assert(K >= 0 && K < 5);
    vector_instr_t *v = calloc(1, sizeof(vector_instr_t));
    if (v == NULL)
        return NULL;
    v->K = K;
    v->cap_idx = 2;
    v->cap = CAPACITY_ARRAY[K][v->cap_idx];
    if ((v->vector = calloc(v->cap, sizeof(instr_t))) == NULL) {
        free(v);
        return NULL;
    }
    v->size = 0;
    return v;
}

void free_vector_instr(vector_instr_t *v) {
    if (v == NULL)
        return;
    if (v->vector != NULL)
        free(v->vector);
    free(v);
}

int8_t vector_instr_append(vector_instr_t *v, const instr_t i) {
    if (v == NULL)
        return 0;
    if (v->size >= v->cap && vector_instr_resize(v, 1) == -1)
        return -1;
    deep_copy_instr(v->vector + v->size, &i);
    v->size++;
    return 0;
}

instr_t *vector_instr_get(vector_instr_t *v, const u64 i) {
    if (v == NULL)
        return NULL;
    if (i >= v->size)
        return NULL;
    return &v->vector[i];
}

int8_t vector_instr_pop(vector_instr_t *v) {
    if (v == NULL)
        return 0;
    if (v->size == 0)
        return 0;
    --v->size;
    return v->cap_idx > 2 && v->size == CAPACITY_ARRAY[v->K][v->cap_idx - 2] &&
           vector_instr_resize(v, -1);
}

u64 vector_instr_cap(vector_instr_t *v) { return v->cap; }

u64 vector_instr_size(vector_instr_t *v) { return v->size; }

typedef struct vector_instr_p_t {
    uint8_t K;
    uint8_t cap_idx;
    uint64_t cap;
    uint64_t size;
    instr_t **vector;
} vector_instr_p_t;

static int8_t vector_instr_p_resize(vector_instr_p_t *v, const int8_t delta);

static int8_t vector_instr_p_resize(vector_instr_p_t *v, const int8_t delta) {
    if (v == NULL)
        return 0;
    const uint8_t new_cap_idx = v->cap_idx + delta;
    instr_t **old_vector = v->vector;
    v->vector = realloc(v->vector,
                        sizeof(instr_t *) * CAPACITY_ARRAY[v->K][new_cap_idx]);
    if (v->vector == NULL) {
        v->vector = old_vector;
        return -1;
    }
    v->cap_idx = new_cap_idx;
    v->cap = CAPACITY_ARRAY[v->K][v->cap_idx];
    return 0;
}

vector_instr_p_t *init_vector_instr_p(const uint8_t K) {
    assert(K >= 0 && K < 5);
    vector_instr_p_t *v = calloc(1, sizeof(vector_instr_p_t));
    if (v == NULL)
        return NULL;
    v->K = K;
    v->cap_idx = 2;
    v->cap = CAPACITY_ARRAY[K][v->cap_idx];
    if ((v->vector = calloc(v->cap, sizeof(instr_t *))) == NULL) {
        free(v);
        return NULL;
    }
    v->size = 0;
    return v;
}

void free_vector_instr_p(vector_instr_p_t *v) {
    if (v == NULL)
        return;
    if (v->vector != NULL)
        free(v->vector);
    free(v);
}

int8_t vector_instr_p_append(vector_instr_p_t *v, instr_t *i) {
    if (v == NULL)
        return 0;
    if (v->size >= v->cap && vector_instr_p_resize(v, 1) == -1)
        return -1;
    v->vector[v->size] = i;
    v->size++;
    return 0;
}

instr_t *vector_instr_p_get(vector_instr_p_t *v, const uint64_t i) {
    if (v == NULL)
        return NULL;
    if (i >= v->size)
        return NULL;
    return v->vector[i];
}

int8_t vector_instr_p_pop(vector_instr_p_t *v) {
    if (v == NULL)
        return 0;
    if (v->size == 0)
        return 0;
    --v->size;
    return v->cap_idx > 2 && v->size == CAPACITY_ARRAY[v->K][v->cap_idx - 2] &&
           vector_instr_p_resize(v, -1);
}

uint64_t vector_instr_p_cap(vector_instr_p_t *v) { return v->cap; }

uint64_t vector_instr_p_size(vector_instr_p_t *v) { return v->size; }
