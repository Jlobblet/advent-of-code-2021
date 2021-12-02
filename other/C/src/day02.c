#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "../libraries/JC/src/jtime.h"
#include "../libraries/JC/src/jio.h"

complex double parseA(const char* line);

complex double parseB(const char* line, i64* aim);

iptr advance_line(char* text, char** buf);

int main() {
    clock_t start = clock();
    struct stat s;
    int fd = open("data/02.txt", O_RDONLY);
    if (fd == -1) {
        return EXIT_FAILURE;
    }

    if (fstat(fd, &s) == -1) {
        close(fd);
        return EXIT_FAILURE;
    }

    char* lines = mmap(NULL, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (lines == MAP_FAILED) {
        close(fd);
        return EXIT_FAILURE;
    }
    complex double sumA = CMPLX(0.0, 0.0), sumB = CMPLX(0.0, 0.0);
    i64 aim = 0;
    for (char* startptr = lines, * endptr; advance_line(startptr, &endptr) != -1; startptr = endptr) {
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

    munmap(lines, s.st_size);
    close(fd);

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

iptr advance_line(char* text, char** buf) {
    iptr len = 0;
    if (text[len] == '\0') { return -1; }
    for (; text[len] != '\n'; len++) {
        if (text[len] == '\0') { break; }
    }
    len++;
    *buf = text + len;
    return len;
}
