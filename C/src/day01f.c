#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include "jint.h"
#include "jmmap.h"
#include "jtime.h"

char* parse(char** ptr, u64* buf) {
    char* end;
    *buf = strtou64(*ptr, &end, 10);
    if (*ptr == end) {
        return NULL;
    }
    return end;
}

int main() {
    clock_t start = clock();

    uptr count_a = 0, count_b = 0;

    iptr size;
    jc_mmap file;
    if ((size = mmap_read("data/01.txt", &file)) == -1) {
        return EXIT_FAILURE;
    }
    madvise(file.address, size, MADV_WILLNEED);
    madvise(file.address, size, MADV_SEQUENTIAL);

    char* ptr = file.address;
    uptr length = 0;
    u64 values[4];
    if ((ptr = parse(&ptr, &values[length])) == NULL) {
        mmap_close(&file);
        exit(EXIT_FAILURE);
    }

    for (length = 1; length < 3 && (ptr = parse(&ptr, &values[length])) != NULL; length++) {
        count_a += values[length] > values[length - 1];
    }

    for (length = 3; (ptr = parse(&ptr, &values[length % 4])) != NULL; length++) {
        uptr index = length % 4, index_m1 = (length - 1) % 4, index_m3 = (length - 3) % 4;
        count_a += values[index] > values[index_m1];
        count_b += values[index] > values[index_m3];
    }

    clock_t done = clock();

    printf("Problem A :\t%lu\n", count_a);
    printf("Problem B :\t%lu\n", count_b);
    printf("Total     :\t%ld us\n", elapsed_us(start, done));

    return EXIT_SUCCESS;
}
