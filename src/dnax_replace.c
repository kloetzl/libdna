/**
 * SPDX-License-Identifier: MIT
 * Copyright 2018 - 2021 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"
#include "utils.h"

#include <assert.h>

DNA_PUBLIC
char *
dnax_replace(
	const char *restrict table, const char *begin, const char *end, char *dest)
{
	assert(table != NULL);
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	unsigned char *ubegin = (unsigned char *)begin;
	unsigned char *uend = (unsigned char *)end;
	size_t from = 0;
	size_t to = 0;
	size_t length = uend - ubegin;

	for (; from < length; from++) {
		unsigned char c = ubegin[from];
		signed char d = table[c];
		if (UNLIKELY(d == -1)) continue;

		dest[to++] = d;
	}

	return dest + to;
}
