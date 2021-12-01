#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

usize read_all_lines(const char* filepath, char*** lines, usize** lengths, usize n) {
    // Open file
    FILE* fp = fopen(filepath, "r");
    if (fp == NULL) {
        // Don't need to set errno as fopen has
        return -1;
    }
    if (n == 0) {
        // Allocate arrays
        n = 1;
        *lines = (char**)malloc(sizeof(char*));
        if (*lines == NULL) {
            errno = ENOMEM;
            return -1;
        }
        *lengths = (usize*)malloc(sizeof(usize));
        if (*lengths == NULL) {
            errno = ENOMEM;
            return -1;
        }
    }
    // How much the buffers grow each time they hit capacity
    const usize growth_factor = 2;
    // Current index
    usize i = 0;
    // Length of the buffer getline allocates
    usize buffer_length = 0;
    while (getline((*lines) + i, &buffer_length, fp) != -1) {
        if (i >= n) {
            n *= growth_factor;
            *lines = (char**)realloc(*lines, n * sizeof(char*));
            if (*lines == NULL) {
                errno = ENOMEM;
                return -1;
            }
            *lengths = (usize*)realloc(*lengths, n * sizeof(usize));
            if (*lengths == NULL) {
                errno = ENOMEM;
                return -1;
            }
        }
        (*lengths)[i] = buffer_length;
        // Reset buffer length so that getline can allocate again
        buffer_length = 0;
        // Increment index
        ++i;
    }
    // Shrink arrays to minimum size
    *lines = (char**)realloc(*lines, n * sizeof(char*));
    if (*lines == NULL) {
        errno = ENOMEM;
        return -1;
    }
    *lengths = (usize*)realloc(*lengths, i * sizeof(usize));
    if (*lines == NULL) {
        errno = ENOMEM;
        return -1;
    }
    return i;
}


clock_t elapsed_us(clock_t start, clock_t end) { return ((end - start) * 1000000) / CLOCKS_PER_SEC; }

i8 strtoi8(const char* nptr, char** endptr, int base) {
    long parsed = strtol(nptr, endptr, base);
    if (parsed > I8_MAX) {
        errno = ERANGE;
        return I8_MAX;
    }
    if (parsed < I8_MIN) {
        errno = ERANGE;
        return I8_MIN;
    }
    return (i8)parsed;
}

u8 strtou8(const char* nptr, char** endptr, int base) {
    unsigned long parsed = strtoul(nptr, endptr, base);
    if (parsed > U8_MAX) {
        errno = ERANGE;
        return U8_MAX;
    }
    return (u8)parsed;
}

i16 strtoi16(const char* nptr, char** endptr, int base) {
    long parsed = strtol(nptr, endptr, base);
    if (parsed > I16_MAX) {
        errno = ERANGE;
        return I16_MAX;
    }
    if (parsed < I16_MIN) {
        errno = ERANGE;
        return I16_MIN;
    }
    return (i16)parsed;
}

u16 strtou16(const char* nptr, char** endptr, int base) {
    unsigned long parsed = strtoul(nptr, endptr, base);
    if (parsed > U16_MAX) {
        errno = ERANGE;
        return U16_MAX;
    }
    return (u16)parsed;
}

i32 strtoi32(const char* nptr, char** endptr, int base) {
    long parsed = strtol(nptr, endptr, base);
    if (parsed > I32_MAX) {
        errno = ERANGE;
        return I32_MAX;
    }
    if (parsed < I32_MIN) {
        errno = ERANGE;
        return I32_MIN;
    }
    return (i32)parsed;
}

u32 strtou32(const char* nptr, char** endptr, int base) {
    unsigned long parsed = strtoul(nptr, endptr, base);
    if (parsed > U32_MAX) {
        errno = ERANGE;
        return U32_MAX;
    }
    return (u32)parsed;
}

i64 strtoi64(const char* nptr, char** endptr, int base) {
    long parsed = strtol(nptr, endptr, base);
    if (parsed > I64_MAX) {
        errno = ERANGE;
        return I64_MAX;
    }
    if (parsed < I64_MIN) {
        errno = ERANGE;
        return I64_MIN;
    }
    return (i64)parsed;
}

u64 strtou64(const char* nptr, char** endptr, int base) {
    unsigned long parsed = strtoul(nptr, endptr, base);
    if (parsed > U64_MAX) {
        errno = ERANGE;
        return U64_MAX;
    }
    return (u64)parsed;
}

isize strotoisize(const char* nptr, char** endptr, int base) {
    long parsed = strtol(nptr, endptr, base);
    if (parsed > ISIZE_MAX) {
        errno = ERANGE;
        return ISIZE_MAX;
    }
    if (parsed < ISIZE_MIN) {
        errno = ERANGE;
        return ISIZE_MIN;
    }
    return (i64)parsed;}

usize strotousize(const char* nptr, char** endptr, int base) {
    unsigned long parsed = strtoul(nptr, endptr, base);
    if (parsed > USIZE_MAX) {
        errno = ERANGE;
        return USIZE_MAX;
    }
    return (u64)parsed;
}
