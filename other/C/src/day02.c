#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "../libraries/JC/src/jtime.h"
#include "../libraries/JC/src/jio.h"

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
    return (double)magnitude * rot;
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

int main() {
    clock_t start = clock();
    char** lines;
    uptr* lengths;
    uptr number_lines = read_all_lines("data/02.txt", &lines, &lengths, 0);
    assert(number_lines == 1000);
    complex double sumA = CMPLX(0.0, 0.0), sumB = CMPLX(0.0, 0.0);
    i64 aim = 0;
    for (uptr i = 0; i < number_lines; ++i) {
        sumA += parseA(lines[i]);
        assert(creal(sumA) < 2e53);
        assert(cimag(sumA) < 2e53);
        sumB += parseB(lines[i], &aim);
        assert(creal(sumB) < 2e53);
        assert(cimag(sumB) < 2e53);
    }
    clock_t end = clock();
    printf("Problem A:\t%ld\t\n", (i64) creal(sumA) * (i64) cimag(sumA));
    printf("Problem B:\t%ld\t\n", (i64) creal(sumB) * (i64) cimag(sumB));
    printf("Elapsed time:\t%ld us\n", elapsed_us(start, end));

    for (uptr i = 0; i < number_lines; ++i) {
        free(lines[i]);
    }
    free(lines);
    free(lengths);

    return EXIT_SUCCESS;
}

