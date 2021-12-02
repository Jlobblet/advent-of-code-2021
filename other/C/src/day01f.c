#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "../libraries/JC/jtime.h"

int parse(char** start, char** end, unsigned long* buf) {
    *buf = strtoul(*start, end, 10);
    if (*start == *end) {
        return -1;
    }
    *start = *end;
    return 0;
}

int main() {
    clock_t start = clock();

    size_t length = 0;
    unsigned long values[4];

    size_t count_a = 0, count_b = 0;

    int fd = open("data/01.txt", O_RDONLY);
    struct stat s;
    if (fstat(fd, &s) == -1) {
        close(fd);
        exit(EXIT_FAILURE);
    }
    char* address = mmap(0, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    madvise(address, s.st_size, MADV_WILLNEED);
    madvise(address, s.st_size, MADV_SEQUENTIAL);
    if (address == MAP_FAILED) {
        close(fd);
        exit(EXIT_FAILURE);
    }

    char* startptr = address;
    char* endptr;
    if (parse(&startptr, &endptr, &values[length++]) == -1) {
        close(fd);
        exit(EXIT_FAILURE);
    }

    while (parse(&startptr, &endptr, &values[length++]) != -1 && length < 3)
    {
        count_a += values[1] > values[0];
    }
    size_t length_m = length - 1, length_mm = length - 3;
    while (parse(&startptr, &endptr, &values[length % 4]) != -1)
    {
        size_t index = (length++) % 4;
        size_t index_m = (length_m++) % 4;
        size_t index_mm = (length_mm++) % 4;
        count_a += values[index] > values[index_m];
        count_b += values[index] > values[index_mm];
    }
    close(fd);
    munmap(address, s.st_size);

    clock_t done = clock();

    printf("Problem A :\t%lu\n", count_a);
    printf("Problem B :\t%lu\n", count_b);
    printf("Total     :\t%ld us\n", elapsed_us(start, done));

    return EXIT_SUCCESS;
}
