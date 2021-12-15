#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include "jarray.h"
#include "jbits.h"
#include "jint.h"
#include "jmmap.h"
#include "jprint.h"
#include "jtime.h"
#include "jvector.h"

typedef enum LineDirection {
    None  = 0b0000,
    Up    = 0b0001,
    Right = 0b0010,
    Down  = 0b0100,
    Left  = 0b1000,
} LineDirection;

typedef struct Line {
    uptr x1, y1, x2, y2;
    LineDirection direction;
} Line;

iptr Line_new(Line* line, char* startptr, char** endptr) {
    while (isspace(*startptr)) { startptr++; }

    line->x1 = strtouptr(startptr, &startptr, 10);
    if (*startptr != ',') { return -1; }
    startptr++;
    line->y1 = strtouptr(startptr, &startptr, 10);

    if (strncmp(startptr, " -> ", 4) != 0) { return -1; }
    startptr += 4;

    line->x2 = strtouptr(startptr, &startptr, 10);
    if (*startptr != ',') { return -1; }
    startptr++;
    line->y2 = strtouptr(startptr, &startptr, 10);

    line->direction = 0;
    if (line->x2 > line->x1) {
        line->direction |= Right;
    } else if (line->x2 < line->x1) {
        line->direction |= Left;
    }
    if (line->y2 > line->y1) {
        line->direction |= Up;
    } else if (line->y2 < line->y1) {
        line->direction |= Down;
    }
    assert(line->direction);

    if (endptr != NULL) { *endptr = startptr; }

    return 0;
}

bool Line_is_diagonal(Line* line) {
    // Since the direction is stored as a flag, a horizontal line only has one bit set
    return popcount(line->direction) > 1;
}

int main() {
    clock_t start = clock();
    jc_mmap f;
    iptr size = mmap_read("data/05.txt", &f);
    madvise(f.address, size, MADV_WILLNEED);
    madvise(f.address, size, MADV_SEQUENTIAL);

    // Read in input data
    Vector lines;
    Vector_default(&lines);
    char* ptr = f.address;
    while (*ptr != EOF && *ptr != 0) {
        if (isspace(*ptr)) {
            ptr++;
            continue;
        }
        Vector_push_back(&lines, calloc(1, sizeof(Line)));
        if (Line_new(lines.data[lines.length - 1], ptr, &ptr) == -1) {
            fputvln("Oh no", stderr);
        }
    }
    Vector_shrink_to_fit(&lines);
    assert(lines.length);
    mmap_close(&f);

    clock_t parse = clock();

    // Get size of grid
    uptr grid_size[2] = {0, 0};
    for (uptr i = 0; i < lines.length; i++) {
        Line* line = (Line*) lines.data[i];
        if (line->x1 > grid_size[0]) { grid_size[0] = line->x1; }
        if (line->x2 > grid_size[0]) { grid_size[0] = line->x2; }
        if (line->y1 > grid_size[1]) { grid_size[1] = line->y1; }
        if (line->y2 > grid_size[1]) { grid_size[1] = line->y2; }
    }
    // Add 1 to account for lines being inclusive, so 0 and max are both valid values
    // Hence the total size along any given dimension is max + 1
    grid_size[0]++;
    grid_size[1]++;
    assert(grid_size[0]);
    assert(grid_size[1]);

    // Create grids
    NDArray grid = {
            .dims = 2,
            .size = grid_size,
    }, diag_grid = grid;
    NDArray_init(&grid);
    NDArray_init(&diag_grid);

    // Mark grids
    for (uptr i = 0; i < lines.length; i++) {
        Line* line = (Line*) lines.data[i];
        bool is_diagonal = Line_is_diagonal(line);
        uptr dx, dy;
        if (line->direction & Right) {
            dx = 1;
        } else if (line->direction & Left) {
            dx = -1;
        } else {
            dx = 0;
        }
        if (line->direction & Up) {
            dy = 1;
        } else if (line->direction & Down) {
            dy = -1;
        } else {
            dy = 0;
        }
        for (uptr x = line->x1, y = line->y1; x != line->x2 + dx || y != line->y2 + dy; x += dx, y += dy) {
            if (!is_diagonal) {
                *NDArray_ptr(&grid, x, y) += 1;
            }
            *NDArray_ptr(&diag_grid, x, y) += 1;
        }
    }

    uptr part_a = 0, part_b = 0;
    for (uptr i = 0; i < grid_size[0]; i++) {
        for (uptr j = 0; j < grid_size[1]; j++) {
            part_a += NDArray_get(&grid, i, j) >= 2;
            part_b += NDArray_get(&diag_grid, i, j) >= 2;
        }
    }

    clock_t solve = clock();

    putv("Part A      : "); putvln(part_a);
    putv("Part B      : "); putvln(part_b);
    putv("Parsing time: "); putv(elapsed_us(start, parse)); putvln("us");
    putv("Solving time: "); putv(elapsed_us(parse, solve)); putvln("us");
    putv("Total   time: "); putv(elapsed_us(start, solve)); putvln("us");

    NDArray_dest(&grid);
    NDArray_dest(&diag_grid);
    for (uptr i = 0; i < lines.length; i++) {
        free(lines.data[i]);
    }
    Vector_dest(&lines);
    return EXIT_SUCCESS;
}
