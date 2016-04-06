#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>

#include "bitarray.h"

#define MAX_BITS (1024 * 1024 * 8)

#define SIZE_ITERS 10
#define SETTING_ITERS 10
#define PROB_ZERO 0.1

#define assert( f ) if (!(f)) { printf("Test failure!\n"); }

int main(int argc, char *argv[])
{
    srand(time(NULL));
    bitarray ba;
    for (size_t i = 0; i < SIZE_ITERS; i++) {
        size_t size = rand() % MAX_BITS;
        new_bitarray_bits(&ba, size);

        fill_bitarray(&ba, true);
        assert( count_true(&ba) == size );

        for (size_t j = 0; j < SETTING_ITERS; j++) {
            size_t nzero = 0;
            for (size_t k = 0; k < size; k++) {
                if (((double) rand() / ((double) RAND_MAX)) < PROB_ZERO) {
                    clearbit(&ba, k);
                    nzero += 1;
                }
            }
            assert( count_true(&ba) == size - nzero );
            fill_bitarray(&ba, true);
        }

        free_bitarray(&ba);
    }
    return 0;
}
