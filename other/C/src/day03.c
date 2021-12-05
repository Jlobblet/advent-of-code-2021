#include <ctype.h>
#include <stdio.h>
#include <sys/mman.h>
#include <time.h>
#include "../libraries/JC/src/jtime.h"
#include "../libraries/JC/src/jint.h"
#include "../libraries/JC/src/jmmap.h"
#include "../libraries/JC/src/jbits.h"

int main() {
    __attribute__((unused)) clock_t start = clock();
    jc_mmap f;
    uptr size = mmap_read("data/03.txt", &f);
    madvise(f.address, size, MADV_WILLNEED);
    madvise(f.address, size, MADV_RANDOM);

    // Count rows and columns
    uptr width = 0, height = 0, endl_width = 0;
    for (const char* startptr = f.address; !isspace(*startptr); startptr++) {
        width++;
    }

    for (const char* startptr = f.address; *startptr != '\0' && *startptr != EOF; startptr++) {
        height += *startptr == '\n';
    }

    // Count the width of the end of a line
    {
        const char* startptr = f.address;
        while (!isspace(*startptr)) startptr++;
        while (isspace(*startptr)) {
            endl_width++;
            startptr++;
        }
    }

    BitArray2d arr = {
            .rows = height,
            .cols = width,
    };
    BitArray2d_init(&arr);

    for (uptr i = 0; i < arr.rows; i++) {
        for (uptr j = 0; j < arr.cols; j++) {
            BitArray2d_set(&arr, i, j, f.address[j + (endl_width + width) * i] == '1');
        }
    }

    printf("%lu\n", BitArray2d_count_on(&arr));

    return 0;
}

