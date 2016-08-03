#ifndef BITARRAY_H
#define BITARRAY_H

#include <limits.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

extern const unsigned char c_bytepopcount[256];

#define CHAR_POPCOUNT(i) ((size_t)(c_bytepopcount[i]))
#ifdef _WIN32
#include <nmmintrin.h>
#define INT32_POPCOUNT(i) (_mm_popcnt_u32(i))
#define INT64_POPCOUNT(i) (_mm_popcnt_u64(i))
#else
#define INT32_POPCOUNT(i) (__builtin_popcount(i))
#define INT64_POPCOUNT(i) (__builtin_popcountll(i))
#endif

//DEBUG
#define BITARRAY_UNIT 0

#if CHAR_BIT == 8
#define L2CHAR_BIT 3
#else
#error "CHAR_BIT on this system is not supported."
#endif

#ifndef BITARRAY_UNIT

#if UINT_MAX==4294967295
#define BITARRAY_UNIT 2
#elif UINT_MAX==18446744073709551615
#define BITARRAY_UNIT 1
#else
#define BITARRAY_UNIT 0
#endif

#endif

#if BITARRAY_UNIT==1

typedef uint64_t bitunit;
#define L2BITUNIT_BYTES 3
#define POPCOUNT(i) (INT64_POPCOUNT(i))

#elif BITARRAY_UNIT==2

typedef uint32_t bitunit;
#define L2BITUNIT_BYTES 2
#define POPCOUNT(i) (INT32_POPCOUNT(i))

#else

typedef unsigned char bitunit;
#define L2BITUNIT_BYTES 0
#define POPCOUNT(i) ((size_t)(c_bytepopcount[i]))

#endif

#define BITUNIT_BYTES (sizeof(bitunit))
#define BITUNIT_BITS (sizeof(bitunit) * CHAR_BIT)
#define L2BITUNIT_BITS (L2BITUNIT_BYTES + L2CHAR_BIT)

#define BITOF(b) ((b) & (BITUNIT_BITS - 1))
#define UNITOF(b) ((b) >> L2BITUNIT_BITS)
#define MASKOF(b) (((bitunit) 1) << BITOF(b))

#define UNITSFORBITS(b) (UNITOF(b) + (BITOF(b) && 1))

#define CLEARBIT(a, b) ((a)[UNITOF(b)] &= ~MASKOF(b))
#define SETBIT(a, b) ((a)[UNITOF(b)] |= MASKOF(b))
#define TOGGLEBIT(a, b) ((a)[UNITOF(b)] ^= MASKOF(b))
#define TESTBIT(a, b) ((a)[UNITOF(b)] & MASKOF(b))

typedef struct bitarray {
    bitunit* bits;
    size_t nbits;
    size_t nunits;
    size_t noverflowbits;
} bitarray;

void init_bitarray_view(bitarray *ba, size_t bits);
void new_bitarray_bits(bitarray *ba, size_t bits);
void new_bitarray_bytes(bitarray *ba, size_t bytes);
void free_bitarray(bitarray *ba);

bool testbit(bitarray *ba, size_t bit);
void setbit(bitarray *ba, size_t bit);
void clearbit(bitarray *ba, size_t bit);
void fill_bitarray(bitarray *ba, bool b);
size_t count_true(bitarray* ba);
size_t count_true_bits(bitarray *ba, size_t bits);

#endif /* BITARRAY_H */
