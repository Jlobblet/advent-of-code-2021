#include <ctype.h>
#include <stdio.h>
#include <sys/mman.h>
#include <time.h>
#include <stdbool.h>
#include "jtime.h"
#include "jint.h"
#include "jmmap.h"
#include "jbits.h"

enum CountPhase { Width, Endl };

int main() {
    clock_t start = clock();
    jc_mmap f;
    uptr size = mmap_read("data/03.txt", &f);
    madvise(f.address, size, MADV_WILLNEED);
    madvise(f.address, size, MADV_RANDOM);

    // Count width of line, and width of end line characters
    uptr width = 0, endl_width = 0;
    enum CountPhase phase = Width;
    for (const char* ptr = f.address; ; ptr++) {
        if (isspace(*ptr) && phase == Width) {
            // Found a whitespace character, so swap to counting end of line characters
            phase = Endl;
        } else if (!isspace(*ptr) && phase == Endl) {
            // Start of the next line - stop
            break;
        }
        width += phase == Width;
        endl_width += phase == Endl;
    }

    // Count the number of s
    uptr height = 0;
    for (const char* ptr = f.address; (ptr - f.address) < (iptr)size; ptr += width + endl_width) {
        height++;
    }

    // Filters for oxygen and co2 ratings
    BitArray filter = {
            .length = height,
    },
        oxy_filter = filter, co2_filter = filter,
        filter_ones = filter, filter_zeroes = filter;
    BitArray_init(&oxy_filter);
    BitArray_init(&co2_filter);
    BitArray_init(&filter_ones);
    BitArray_init(&filter_zeroes);
    BitArray_flip(&oxy_filter);
    BitArray_flip(&co2_filter);

    // Iterate over columns
    madvise(f.address, size, MADV_SEQUENTIAL);
    uptr gamma_rate = 0, epsilon_rate = 0;
    for (uptr col = 0; col < width; col++) {
        // Count number of 0s and 1s
        uptr ones_count = 0, zeroes_count = 0,
            oxy_ones_count = 0, oxy_zeroes_count = 0,
            co2_ones_count = 0, co2_zeroes_count = 0;
        for (uptr row = 0; row < height; row++) {
            const char* line = f.address + (width + endl_width) * row;
            bool bit = line[col] == '1';
            BitArray_set(&filter_ones, row, bit);
            BitArray_set(&filter_zeroes, row, !bit);

            ones_count += bit;
            zeroes_count += !bit;

            bool oxy_filtered = BitArray_get(&oxy_filter, row);
            oxy_ones_count += oxy_filtered & bit;
            oxy_zeroes_count += oxy_filtered & !bit;

            bool co2_filtered = BitArray_get(&co2_filter, row);
            co2_ones_count += co2_filtered & bit;
            co2_zeroes_count += co2_filtered & !bit;
        }
        uptr most_common = ones_count > zeroes_count;
        gamma_rate = (gamma_rate << 1) + most_common;
        epsilon_rate = (epsilon_rate << 1) + !most_common;

        if (BitArray_popcount(&oxy_filter) > 1) {
            uptr oxy = oxy_ones_count >= oxy_zeroes_count;
            BitArray_and(&oxy_filter, oxy ? &filter_ones : &filter_zeroes);
        }

        if (BitArray_popcount(&co2_filter) > 1) {
            uptr co2 = co2_ones_count < co2_zeroes_count;
            BitArray_and(&co2_filter, co2 ? &filter_ones : &filter_zeroes);
        }
    }

    // Find the single bit set in the oxy and co2 filters
    uptr oxy_row = height, co2_row = height;
    for (uptr i = 0; i < height && (oxy_row == height || co2_row == height); i++) {
        if (BitArray_get(&oxy_filter, i)) {
            oxy_row = i;
        }
        if (BitArray_get(&co2_filter, i)) {
            co2_row = i;
        }
    }

    // Read the oxy and co2 ratings from their rows
    madvise(f.address, size, MADV_RANDOM);
    uptr oxy_rating = 0, co2_rating = 0;
    const char* oxy_line = f.address + (width + endl_width) * oxy_row,
        *co2_line = f.address + (width + endl_width) * co2_row;
    for (uptr col = 0; col < width; col++) {
        oxy_rating = (oxy_rating << 1) + (oxy_line[col] == '1');
        co2_rating = (co2_rating << 1) + (co2_line[col] == '1');
    }

    madvise(f.address, size, MADV_DONTNEED);

    uptr part_a = gamma_rate * epsilon_rate,
        part_b = oxy_rating * co2_rating;

    clock_t end = clock();

    printf("Part A:\t%lu\n", part_a);
    printf("Part B:\t%lu\n", part_b);
    printf("Time  :\t%lu us\n", elapsed_us(start, end));

    mmap_close(&f);
    BitArray_dest(&oxy_filter);
    BitArray_dest(&co2_filter);
    BitArray_dest(&filter_ones);
    BitArray_dest(&filter_zeroes);
    return 0;
}
