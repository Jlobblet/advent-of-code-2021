#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../libraries/JC/jtime.h"

int main() {
    clock_t start = clock();

    const int buffer_size = 32;
    char buffer[buffer_size];

    size_t length = 0;
    unsigned long values[4];

    size_t count_a = 0, count_b = 0;

    FILE* fp = fopen("data/01.txt", "r");
    if (fp == NULL) { exit(EXIT_FAILURE); }

    if (fgets(buffer, buffer_size, fp) == NULL) { exit(EXIT_FAILURE); }
    values[length++] = strtoul(buffer, NULL, 10);
    while (fgets(buffer, buffer_size, fp) != NULL && length < 3)
    {
        values[length++] = strtoul(buffer, NULL, 10);
        count_a += values[1] > values[0];
    }
    size_t length_m = length - 1, length_mm = length - 3;
    while (fgets(buffer, buffer_size, fp) != NULL)
    {
        size_t index = (length++) % 4;
        values[index] = strtoul(buffer, NULL, 10);
        size_t index_m = (length_m++) % 4;
        count_a += values[index] > values[index_m];
        size_t index_mm = (length_mm++) % 4;
        count_b += values[index] > values[index_mm];
    }
    fclose(fp);

    clock_t done = clock();

    printf("Problem A :\t%lu\n", count_a);
    printf("Problem B :\t%lu\n", count_b);
    printf("Total     :\t%ld us\n", elapsed_us(start, done));

    return EXIT_SUCCESS;
}
