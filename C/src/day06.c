#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/mman.h>
#include <ctype.h>
#include "jmmap.h"
#include "jint.h"
#include "jprint.h"
#include "jtime.h"

void evolve(uptr (* fish_count)[9]) {
    uptr new_fish_count[9];
    // Numbers 1-8 are moved to the left one step.
    memcpy(new_fish_count, &(*fish_count)[1], 8 * sizeof(uptr));
    // The zeroes are handled specially, being added to 6 as well as to 8.
    new_fish_count[8] = (*fish_count)[0];
    new_fish_count[6] += (*fish_count)[0];
    // Copy the results back into the buffer
    memcpy(fish_count, new_fish_count, 9 * sizeof(uptr));
}

int main() {
    clock_t start = clock();
    jc_mmap f;
    uptr size = mmap_read("data/06.txt", &f);
    madvise(f.address, size, MADV_WILLNEED);
    madvise(f.address, size, MADV_SEQUENTIAL);

    // Count of how many fish have each number, where the number is the index of the array
    // Valid numbers are 0, 1, 2, 3, 4, 5, 6, 7, 8 - so 9 elements.
    uptr fish_count[9] = {0};

    char* ptr = f.address;
    while (*ptr != EOF && *ptr != 0) {
        if (isspace(*ptr)) {
            ptr++;
            continue;
        }
        uptr n = strtouptr(ptr, &ptr, 10);
        fish_count[n]++;
        // Skip the comma
        ptr++;
    }

    mmap_close(&f);
    clock_t parsed = clock();

    // Evolve the model for 80 days
    uptr i;
    for (i = 0; i < 80; i++) {
        evolve(&fish_count);
    }

    // Sum the number of fish alive
    uptr part_a = 0;
    for (uptr j = 0; j < 9; j++) {
        part_a += fish_count[j];
    }

    clock_t part_a_time = clock();

    // Evolve for 256 days
    for (; i < 256; i++) {
        evolve(&fish_count);
    }

    uptr part_b = 0;
    for (uptr j = 0; j < 9; j++) {
        part_b += fish_count[j];
    }

    clock_t part_b_time = clock();

    putv("Part A      : "); putvln(part_a);
    putv("Part B      : "); putvln(part_b);
    putv("Parsing time: "); putv(elapsed_us(start, parsed)); putvln("us");
    putv("Part A  time: "); putv(elapsed_us(parsed, part_a_time)); putvln("us");
    putv("Part B  time: "); putv(elapsed_us(part_a_time, part_b_time)); putvln("us");
    putv("Total   time: "); putv(elapsed_us(start, part_b_time)); putvln("us");

    return EXIT_SUCCESS;
}
