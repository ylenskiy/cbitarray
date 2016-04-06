#include "cbench.h"

#include "bitarray.h"

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include <time.h>

#define SMALL_BUF_SIZE (256*1024*8)
#define LARGE_BUF_SIZE (1000 * SMALL_BUF_SIZE)

CBENCH_DEF(FillBuffer) {
    size_t bufsize = *(size_t*)CBENCH_args;

    bitarray ba;

    CBENCH_INIT("Fill buffer of size %llu with true", bufsize) {
        new_bitarray_bits(&ba, bufsize);
        CBENCH_MEASURE_START
        fill_bitarray(&ba, true);
        CBENCH_MEASURE_STOP
        free_bitarray(&ba);
    }
}

CBENCH_DEF(CountBits) {
    size_t bufsize = *(size_t*)CBENCH_args;

    bitarray ba;
    new_bitarray_bits(&ba, bufsize);
    srand(time(NULL));
    for (size_t i = 0; i < ba.nbits; i++) {
        if (rand() % 2 == 1) {
            setbit(&ba, i);
        } else {
            clearbit(&ba, i);
        }
    }
    CBENCH_INIT("Count set bits in buffer of size %llu", bufsize) {
        CBENCH_MEASURE_START
        count_true(&ba);
        CBENCH_MEASURE_STOP
    }
    free_bitarray(&ba);
}

CBENCH_DEF(ClearBits) {
    size_t bufsize = ((size_t*) CBENCH_args)[0];
    size_t nbits_to_set = ((size_t*) CBENCH_args)[1];

    size_t* bits_to_set = (size_t*)malloc(nbits_to_set * sizeof(*bits_to_set));
    srand(time(NULL));
    int bits_chosen = 0;
    while (bits_chosen < nbits_to_set) {
        for (size_t i = 0;
             (bits_chosen < nbits_to_set) && (i < bufsize);
             i++) {
            if (rand() % 2 == 0) {
                bits_to_set[bits_chosen++] = i;
            }
        }
    }

    bitarray ba;
    new_bitarray_bits(&ba, bufsize);

    CBENCH_INIT("Clear %llu bits from size %llu bitarray", nbits_to_set, bufsize) {
        fill_bitarray(&ba, true);
        CBENCH_MEASURE_START
        for (size_t i = 0; i < nbits_to_set; i++) {
            CLEARBIT(ba.bits, i);
        }
        CBENCH_MEASURE_STOP
    }

    free_bitarray(&ba);
    free(bits_to_set);
}

int main(int argc, char *argv[])
{
    printf("Bitunit bytes: %llu\n", BITUNIT_BYTES);

    size_t bufsize;

    bufsize = SMALL_BUF_SIZE;
    do_bench_print_summary(CBENCH_FillBuffer,
                           5,
                           1000,
                           &bufsize);
    bufsize = LARGE_BUF_SIZE;
    do_bench_print_summary(CBENCH_FillBuffer,
                           5,
                           10,
                           &bufsize);

    bufsize = SMALL_BUF_SIZE;
    do_bench_print_summary(CBENCH_CountBits,
                           5,
                           1000,
                           &bufsize);

    bufsize = LARGE_BUF_SIZE;
    do_bench_print_summary(CBENCH_CountBits,
                           5,
                           10,
                           &bufsize);

    size_t flipbitarg[] = {SMALL_BUF_SIZE, (size_t)floor(SMALL_BUF_SIZE * 0.75)};
    do_bench_print_summary(CBENCH_ClearBits,
                           5,
                           100,
                           flipbitarg);

    return 0;
}
