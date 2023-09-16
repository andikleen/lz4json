/*
 * Dump mozilla style lz4json files.
 *
 * Copyright (c) 2014 Intel Corporation
 * Author: Andi Kleen
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/* File format reference:
   https://dxr.mozilla.org/mozilla-central/source/toolkit/components/lz4/lz4.js
 */
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#ifndef __APPLE__
#include <endian.h>
#else
#define htole32(x) x /* assume apple targets are little endian */
#endif

#include "lz4.h"

/**
 * Decompress a mozilla style jsonlz4 file to stdout
 */
void decompress_jsonlz4_file(char *filepath) {
    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        perror(filepath);
        return;
    }
    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror(filepath);
        exit(1);
    }
    if (st.st_size < 12) {
        fprintf(stderr, "%s: file too short\n", filepath);
        exit(1);
    }

    char *map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (map == (char *)-1) {
        perror(filepath);
        exit(1);
    }
    if (memcmp(map, "mozLz40", 8)) {
        fprintf(stderr, "%s: not a mozLZ4a file\n", filepath);
        exit(1);
    }
    size_t outsz = htole32(*(uint32_t *)(map + 8));
    char *out = malloc(outsz);
    if (!out) {
        fprintf(stderr, "Cannot allocate memory\n");
        exit(1);
    }
    if (LZ4_decompress_safe_partial(map + 12, out, st.st_size - 12, outsz, outsz) < 0) {
        fprintf(stderr, "%s: decompression error\n", filepath);
        exit(1);
    }
    ssize_t decsz = write(STDOUT_FILENO, out, outsz);
    if (decsz < 0 || decsz != outsz) {
        if (decsz >= 0) {
            errno = EIO;
        }
        perror("Error writing file to stdout!\n");
        exit(1);
    }
    free(out);
    munmap(map, st.st_size);
    close(fd);
}

int main(int argc, char **argv) {
    if (argc == 2 && ((strcmp(argv[1], "--help") == 0) || strcmp(argv[1], "-h") == 0)) {
        fprintf(stderr, "Usage: %s [file...]\n", argv[0]);
        fprintf(stderr, "Decompresses mozilla style lz4json files.\nhttps://github.com/andikleen/lz4json/\n");
        return 0;
    }
    while (*++argv) {
        decompress_jsonlz4_file(*argv);
    }
    return 0;
}
