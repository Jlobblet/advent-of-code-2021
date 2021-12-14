#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <sys/mman.h>
#include <time.h>
#include "jarray.h"
#include "jbits.h"
#include "jmmap.h"
#include "jprint.h"
#include "jtime.h"
#include "jvector.h"

typedef struct BingoGrid {
    /// Bit mask to store which values have been called so far.
    /// The coordinates of values that have been called are set to 1.
    BitArray2d hit_mask;
    NDArray values;
} BingoGrid;

void BingoGrid_new(const char* start, BingoGrid* ptr, char** endptr) {
    ptr->hit_mask.rows = 5;
    ptr->hit_mask.cols = 5;
    BitArray2d_init(&ptr->hit_mask);

    ptr->values.dims = 2;
    ptr->values.size = calloc(2, sizeof(uptr));
    ptr->values.size[0] = 5;
    ptr->values.size[1] = 5;
    NDArray_init(&ptr->values);

    char* end;
    for (uptr i = 0; i < 5; i++) {
        for (uptr j = 0; j < 5; j++) {
            NDArray_Backer value = (NDArray_Backer) strtoi64(start, &end, 10);
            NDArray_set(&ptr->values, value, i, j);
            start = end;
        }
    }
    *endptr = end;
}

void BingoGrid_dest(BingoGrid* ptr) {
    free(ptr->values.size);
    BitArray2d_dest(&ptr->hit_mask);
    NDArray_dest(&ptr->values);
}

bool BingoGrid_has_won(BingoGrid* ptr) {
    assert(ptr->values.dims == 2);
    assert(ptr->hit_mask.rows == ptr->values.size[0]);
    assert(ptr->hit_mask.cols == ptr->values.size[1]);
    uptr col_sum;
    for (uptr i = 0; i < ptr->hit_mask.rows; i++) {
        // Take advantage of internal structure here
        // Since rows only have 5 bits, this will contain a row
        if (popcount(*ptr->hit_mask.row_starts[i]) == 5) {
            return true;
        }
    }
    for (uptr j = 0; j < ptr->hit_mask.cols; j++) {
        col_sum = 0;
        for (uptr i = 0; i < ptr->hit_mask.rows; i++) {
            col_sum += BitArray2d_get(&ptr->hit_mask, i, j);
        }
        if (col_sum == 5) {
            return true;
        }
    }
    return false;
}

NDArray_Backer BingoGrid_count_remainder(BingoGrid* ptr) {
    assert(ptr->values.dims == 2);
    assert(ptr->hit_mask.rows == ptr->values.size[0]);
    assert(ptr->hit_mask.cols == ptr->values.size[1]);
    NDArray_Backer sum = 0;
    for (uptr i = 0; i < ptr->hit_mask.rows; i++) {
        for (uptr j = 0; j < ptr->hit_mask.cols; j++) {
            if (BitArray2d_get(&ptr->hit_mask, i, j)) { continue; }
            sum += NDArray_get(&ptr->values, i, j);
        }
    }
    return sum;
}

void BingoGrid_mark(BingoGrid* ptr, NDArray_Backer value) {
    assert(ptr->values.dims == 2);
    assert(ptr->hit_mask.rows == ptr->values.size[0]);
    assert(ptr->hit_mask.cols == ptr->values.size[1]);
    for (uptr i = 0; i < ptr->hit_mask.rows; i++) {
        for (uptr j = 0; j < ptr->hit_mask.cols; j++) {
            if (BitArray2d_get(&ptr->hit_mask, i, j)) { continue; }
            if (NDArray_get(&ptr->values, i, j) == value) {
                BitArray2d_on(&ptr->hit_mask, i, j);
            }
        }
    }
}

void BingoGrid_print(BingoGrid* ptr) {
    assert(ptr->values.dims == 2);
    assert(ptr->hit_mask.rows == ptr->values.size[0]);
    assert(ptr->hit_mask.cols == ptr->values.size[1]);
    for (uptr i = 0; i < ptr->hit_mask.rows; i++) {
        for (uptr j = 0; j < ptr->hit_mask.cols; j++) {
            BitArray2dBacker called = BitArray2d_get(&ptr->hit_mask, i, j);
            if (called) {
                putv("\e[0;32m");
            } else {
                putv("\e[0;31m");
            }
            printf("%*ld", -3, NDArray_get(&ptr->values, i, j));
            putv("\e[0m");
        }
        puts("");
    }
}

int main() {
    clock_t start = clock(), end;
    jc_mmap f;
    iptr size = mmap_read("data/04.txt", &f);

    /// Vector to store numbers to call
    Vector numbers;
    Vector_default(&numbers);
    madvise(f.address, size, MADV_SEQUENTIAL);
    char* ptr = f.address;
    // Numbers are comma-separated on the first line
    while (!isspace(*ptr)) {
        uptr n = strtouptr(ptr, &ptr, 10);
        if (Vector_push_back(&numbers, (void*) n)) {
            puts("uh oh");
            return EXIT_FAILURE;
        }
        // Skip the comma
        ptr++;
    }
    Vector_shrink_to_fit(&numbers);

    /// Vector to store each grid
    Vector bingo_grids;
    Vector_default(&bingo_grids);
    while (*ptr != EOF && *ptr != 0) {
        if (isspace(*ptr)) {
            ptr++;
            continue;
        }
        Vector_push_back(&bingo_grids, calloc(1, sizeof(BingoGrid)));
        BingoGrid_new(ptr, bingo_grids.data[bingo_grids.length - 1], &ptr);
    }
    Vector_shrink_to_fit(&bingo_grids);
    mmap_close(&f);

    /// BitArray to store grids that have been marked as won already
    BitArray won_grids = {
            .length = bingo_grids.length,
    };
    BitArray_init(&won_grids);

    NDArray_Backer part_a = -1, part_b = -1;
    uptr n_won = 0;
    // Iterate over numbers to call
    for (uptr i = 0; i < numbers.length && n_won < bingo_grids.length; i++) {
        NDArray_Backer n = (NDArray_Backer) (uptr) numbers.data[i];
        // Iterate over each grid
        for (uptr g = 0; g < bingo_grids.length; g++) {
            // Skip grids that have already won
            if (BitArray_get(&won_grids, g)) { continue; }
            BingoGrid* grid = (BingoGrid*) bingo_grids.data[g];
            // Mark off the called number on this grid
            BingoGrid_mark(grid, n);
            if (BingoGrid_has_won(grid)) {
                // Mark the grid as having won
                BitArray_on(&won_grids, g);
                // Count the number of grids that have won
                n_won = BitArray_popcount(&won_grids);
                NDArray_Backer* part = NULL;
                if (n_won == 1) {
                    // First grid for part a
                    part = &part_a;
                } else if (n_won == bingo_grids.length) {
                    // Last grid for part B
                    part = &part_b;
                }
                if (part != NULL) {
                    *part = BingoGrid_count_remainder(grid);
#ifndef NDEBUG
                    putv("Sum: ");
                    putvln(*part);
                    putv("Number: ");
                    putvln(n);
                    putvln("Grid: ");
                    BingoGrid_print(grid);
#endif
                    *part *= n;
                }
            }
        }
    }

    end = clock();

    putv("Part A: ");
    putvln(part_a);
    putv("Part B: ");
    putvln(part_b);
    putv("Time taken: ");
    putv(elapsed_us(start, end));
    putvln("us");

    BitArray_dest(&won_grids);
    Vector_dest(&numbers);
    for (uptr i = 0; i < bingo_grids.length; i++) {
        BingoGrid_dest(bingo_grids.data[i]);
        free(bingo_grids.data[i]);
    }
    Vector_dest(&bingo_grids);
    return EXIT_SUCCESS;
}
