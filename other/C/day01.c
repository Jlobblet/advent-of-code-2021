#include <stdio.h>
#include <stdlib.h>
#include <time.h>

clock_t elapsed_us(clock_t start, clock_t parsed);

int main() {
    clock_t start = clock();

    FILE* fp = fopen("01.txt", "r");
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

    size_t count_b = 0, last_sum = values[0] + values[1] + values[2], next_sum;
    for (size_t i = 3; i < length; i++) {
        next_sum = values[i - 2] + values[i - 1] + values[i];
        count_b += next_sum > last_sum;
        last_sum = next_sum;
    }

    clock_t problem_b = clock();

    free(values);

    printf("Parsing   :\t\t%ld us\n", elapsed_us(start, parsed));
    printf("Problem A :\t%lu\t%ld us\n", count_a, elapsed_us(problem_a, parsed));
    printf("Problem B :\t%lu\t%ld us\n", count_b, elapsed_us(problem_b, problem_a));
    printf("Total     :\t\t%ld us\n", elapsed_us(problem_b, start));

    return EXIT_SUCCESS;
}

clock_t elapsed_us(clock_t start, clock_t parsed) { return (parsed - start) * 1000000 / CLOCKS_PER_SEC; }
