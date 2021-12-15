#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "jmmap.h"
#include "jtime.h"

int compare(const void *l, const void *r) {
    double a = *(const double*)l;
    double b = *(const double*)r;
    if (a < b) {
        return -1;
    } else if (a > b) {
        return 1;
    } else {
        return 0;
    }
}

int main() {
    clock_t start = clock();
    jc_mmap f;
    iptr size = mmap_read("data/07.txt", &f);
    if (size == -1) {
        return EXIT_FAILURE;
    }

    madvise(f.address, size, MADV_WILLNEED);
    madvise(f.address, size, MADV_SEQUENTIAL);
    uptr length = 0, capacity = 1, growth_factor = 2;
    long* values = calloc(capacity, sizeof(long));
    if (values == NULL) {
        mmap_close(&f);
        return EXIT_FAILURE;
    }

    char* ptr = f.address;
    while (*ptr != EOF && *ptr != 0) {
        if (isspace(*ptr)) {
            ptr++;
            continue;
        }
        // Grow if full
        if (length == capacity) {
            capacity *= growth_factor;
            values = realloc(values, capacity * sizeof(long));
        }
        // Parse value
        long v = strtol(ptr, &ptr, 10);
        values[length] = v;
        length++;
        // Skip comma
        ptr++;
    }
    mmap_close(&f);
    clock_t parsed = clock();

    long* sorted = calloc(length, sizeof(long));
    if (sorted == NULL) {
        free(values);
        return EXIT_FAILURE;
    }
    memcpy(sorted, values, length * sizeof(long));
    qsort(sorted, length, sizeof(long), compare);
    long mean = 0;
    for (uptr i = 0; i < length; i++) {
        mean += sorted[i];
    }
    mean /= (iptr)length;
    long median = sorted[length / 2], part_a = 0, part_b1 = 0, part_b2 = 0;
    clock_t stats = clock();
    free(sorted);

    for (uptr i = 0; i < length; i++) {
        part_a += labs(values[i] - median);
        long abs_distance_from_mean = labs(values[i] - mean);
        long abs_distance_from_mean_m = labs(values[i] - mean - 1);
        part_b1 += (abs_distance_from_mean * (abs_distance_from_mean + 1)) / 2;
        part_b2 += (abs_distance_from_mean_m * (abs_distance_from_mean_m + 1)) / 2;
    }

    long part_b = part_b1 < part_b2 ? part_b1 : part_b2;
    clock_t solved = clock();

    printf("Part A      : %ld\n", part_a);
    printf("Part B      : %ld\n", part_b);
    printf("Parsing time: %ldus\n", elapsed_us(start, parsed));
    printf("Stats   time: %ldus\n", elapsed_us(parsed, stats));
    printf("Solving time: %ldus\n", elapsed_us(stats, solved));
    printf("Total   time: %ldus\n", elapsed_us(start, solved));

    free(values);
    return EXIT_SUCCESS;
}
