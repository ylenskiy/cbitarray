#include "bitarray.h"
#include <stdlib.h>
#include <limits.h>

const unsigned char c_bytepopcount[] =
       {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};

void init_bitarray_view(bitarray *ba, size_t bits) {
    ba->nbits = bits;

    if (bits == 0) {
        ba->nunits = 0;
        return;
    }

    ba->nunits = UNITSFORBITS(bits);
    ba->noverflowbits = BITOF(bits);
}

void new_bitarray_bits(bitarray *ba, size_t bits) {
    init_bitarray_view(ba, bits);
    if (bits == 0) {
        ba->bits = NULL;
    } else {
        ba->bits = (bitunit*) malloc(sizeof(bitunit) * ba->nunits);

    }
}

void new_bitarray_bytes(bitarray *ba, size_t bytes) {
    new_bitarray_bits(ba, bytes * CHAR_BIT);
}

void free_bitarray(bitarray *ba) {
    free(ba->bits);
}

bool testbit(bitarray *ba, size_t bit) {
    return TESTBIT(ba->bits, bit);
}

void setbit(bitarray *ba, size_t bit) {
    SETBIT(ba->bits, bit);
}

void clearbit(bitarray *ba, size_t bit) {
    CLEARBIT(ba->bits, bit);
}

void fill_bitarray(bitarray *ba, bool b) {
    int f = 0;
    if (b) { f = ~f; }
    memset((void *)ba->bits, f, ba->nunits * BITUNIT_BYTES);
}

static inline bitunit overflow_mask(size_t n_overflow) {
    bitunit mask = (((bitunit)1) << n_overflow) - ((bitunit)1);
    return mask ? mask : ~0;
}

size_t count_true(bitarray* ba) {
    bitunit last = ba->bits[ba->nunits - 1];
    ba->bits[ba->nunits - 1] &= overflow_mask(ba->noverflowbits);

    size_t nints = (ba->nunits * BITUNIT_BYTES) / sizeof(uint32_t);
    size_t noverflowbytes = (ba->nunits * BITUNIT_BYTES) % sizeof(uint32_t);

    uint32_t* uibits = (uint32_t*)ba->bits;

    size_t c = 0;
    for (size_t i = 0; i < nints; i++) {
        c += INT32_POPCOUNT(uibits[i]);
    }

    unsigned char* overflow = (unsigned char*) (uibits + nints);
    for (size_t i = 0; i < noverflowbytes; i++) {
        c += CHAR_POPCOUNT(overflow[i]);
    }

    ba->bits[ba->nunits - 1] = last;

    return c;
}

size_t count_true_bits(bitarray *ba, size_t bits) {
    if (bits == ba->nbits) {
        return count_true(ba);
    }
    size_t noverflow = BITOF(bits);
    size_t nunits = UNITSFORBITS(bits);

    size_t nints = ((nunits - 1) * BITUNIT_BYTES) / sizeof(uint32_t);
    size_t int_noverflow = ((nunits - 1) * BITUNIT_BYTES) % sizeof(uint32_t);

    uint32_t* uibits = (uint32_t*)ba->bits;

    size_t c = 0;
    for (size_t i = 0; i < nints; i++) {
        c += INT32_POPCOUNT(uibits[i]);
    }
    unsigned char* int_overflow = (unsigned char*)(uibits + nints);
    for (size_t i = 0; i < int_noverflow ; i++) {
        c += CHAR_POPCOUNT(int_overflow[i]);
    }
    bitunit* last_unit = (bitunit*)(int_overflow + int_noverflow);
    c += POPCOUNT(*last_unit & overflow_mask(noverflow));
    return c;
}
