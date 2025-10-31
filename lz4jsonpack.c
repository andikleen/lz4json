/*
 * Compress mozilla style lz4json files.
 *
 * Copyright (c) 2014 Intel Corporation
 * Copyright (c) 2025 SUSE
 * Author: Andi Kleen
 * Author: Jiri Slaby
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
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#ifndef __APPLE__
#include <endian.h>
#else
#define htole32(x) x /* assume apple targets are little endian */
#endif

#include "lz4.h"

int main(int, char **av)
{
	while (*++av) {
		int fd = open(*av, O_RDONLY);
		if (fd < 0) {
			perror(*av);
			continue;
		}
		char outfile[strlen(*av) + strlen("lz4") + 1];
		strcpy(outfile, *av);
		strcat(outfile, "lz4");
		int fd2 = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd2 < 0) {
			perror(outfile);
			exit(1);
		}
		struct stat st;
		if (fstat(fd, &st) < 0) {
			perror(*av);
			exit(1);
		}
		if (st.st_size > LZ4_MAX_INPUT_SIZE) {
			exit(1);
		}
		char *map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
		if (map == (char *)-1) {
			perror(*av);
			exit(1);
		}
		size_t outsz = LZ4_compressBound(st.st_size);
		char *out = malloc(outsz);
		if (!out) {
			fprintf(stderr, "Cannot allocate memory\n");
			exit(1);
		}
		int compsize = LZ4_compress_default(map, out, st.st_size, outsz);
		if (compsize < 0) {
			fprintf(stderr, "%s: compression error\n", *av);
			exit(1);
		}

		ssize_t decsz = write(fd2, "mozLz40", 8);
		if (decsz < 8) {
			if (decsz >= 0)
				errno = EIO;
			perror("write");
			exit(1);
		}
		uint32_t outszLE = htole32(st.st_size);
		decsz = write(fd2, &outszLE, 4);
		if (decsz < 4) {
			if (decsz >= 0)
				errno = EIO;
			perror("write");
			exit(1);
		}
		decsz = write(fd2, out, compsize);
		if (decsz < 0 || decsz != compsize) {
			if (decsz >= 0)
				errno = EIO;
			perror("write");
			exit(1);
		}
		free(out);
		munmap(map, st.st_size);
		close(fd2);
		close(fd);
	}

	return 0;
}

