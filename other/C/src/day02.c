#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <sys/mman.h>
#include "../libraries/JC/src/jtime.h"
#include "../libraries/JC/src/jint.h"
#include "../libraries/JC/src/jmmap.h"
#include "../libraries/JC/src/jstring.h"

complex double parseA(const char* line);

complex double parseB(const char* line, i64* aim);

int main() {
    clock_t start = clock();
    jc_mmap f;
    iptr size = mmap_read("data/02.txt", &f);
    madvise(f.address, size, MADV_WILLNEED);
    madvise(f.address, size, MADV_SEQUENTIAL);
    complex double sumA = CMPLX(0.0, 0.0), sumB = CMPLX(0.0, 0.0);
    i64 aim = 0;
    for (const char* startptr = f.address, * endptr; advance_line(startptr, &endptr) != -1; startptr = endptr) {
        sumA += parseA(startptr);
        assert(creal(sumA) < 2e53);
        assert(cimag(sumA) < 2e53);
        sumB += parseB(startptr, &aim);
        assert(creal(sumB) < 2e53);
        assert(cimag(sumB) < 2e53);
    }
    clock_t end = clock();
    printf("Problem A:\t%ld\t\n", (i64) creal(sumA) * (i64) cimag(sumA));
    printf("Problem B:\t%ld\t\n", (i64) creal(sumB) * (i64) cimag(sumB));
    printf("Elapsed time:\t%ld us\n", elapsed_us(start, end));

    mmap_close(&f);

    return EXIT_SUCCESS;
}

complex double parseA(const char* line) {
    int offset;
    complex double rot;
    switch (line[0]) {
        case 'f':
            offset = 8;
            rot = CMPLX(1.0, 0.0);
            break;
        case 'd':
            offset = 5;
            rot = CMPLX(0.0, 1.0);
            break;
        case 'u':
            offset = 3;
            rot = CMPLX(0.0, -1.0);
            break;
        default:
            exit(EXIT_FAILURE);
    }
    int magnitude = line[offset] - '0';
    return (double) magnitude * rot;
}

complex double parseB(const char* line, i64* aim) {
    int magnitude;
    switch (line[0]) {
        case 'f':
            magnitude = line[8] - '0';
            return CMPLX(magnitude, *aim * magnitude);
        case 'd':
            *aim += line[5] - '0';
            break;
        case 'u':
            *aim -= line[3] - '0';
            break;
        default:
            exit(EXIT_FAILURE);
    }
    return CMPLX(0.0, 0.0);
}
