#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../libraries/JC/src/jtime.h"

int main() {
    clock_t start = clock();

    FILE* fp = fopen("data/01.txt", "r");
    if (fp == NULL) { exit(EXIT_FAILURE); }

    char* buffer;
    size_t buffer_size = 0;

    const size_t initial_size = 8;
    size_t growth_factor = 2;
    size_t capacity = initial_size;
    size_t length = 0;
    unsigned long* values = malloc(initial_size * sizeof(unsigned long));
    if (values == NULL) { exit(EXIT_FAILURE); }

    while (getline(&buffer, &buffer_size, fp) != -1)
    {
        if (length >= capacity) {
            capacity *= growth_factor;
            values = realloc(values, capacity * sizeof(unsigned long));
            if (values == NULL) { exit(EXIT_FAILURE); }
        }
        values[length] = strtoul(buffer, NULL, 10);
        length++;
    }
    fclose(fp);
    if (buffer) { free(buffer); }

    clock_t parsed = clock();

    size_t count_a = 0;
    for (size_t i = 1; i < length; i++) {
        count_a += values[i] > values[i - 1];
    }

    clock_t problem_a = clock();

    size_t count_b = 0;
    for (size_t i = 3; i < length; i++) {
        count_b += values[i] > values[i - 3];
    }

    clock_t problem_b = clock();

    free(values);

    printf("Parsing   :\t\t%ld us\n", elapsed_us(start, parsed));
    printf("Problem A :\t%lu\t%ld us\n", count_a, elapsed_us(parsed, problem_a));
    printf("Problem B :\t%lu\t%ld us\n", count_b, elapsed_us(problem_a, problem_b));
    printf("Total     :\t\t%ld us\n", elapsed_us(start, problem_b));

    return EXIT_SUCCESS;
}
