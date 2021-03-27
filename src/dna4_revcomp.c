/**
 * SPDX-License-Identifier: MIT
 * Copyright 2018-2020 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"

#include <assert.h>

DNA_LOCAL
char *
dna4_revcomp_generic(const char *begin, const char *end, char *restrict dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	size_t length = end - begin;
	for (size_t i = 0; i < length; i++) {
		char c = begin[length - 1 - i];

		dest[i] = c ^= c & 2 ? 4 : 21;
	}

	return dest + length;
}

#if !defined(__ARM_NEON) && !defined(__x86_64)
DNA_PUBLIC
char *
dna4_revcomp(const char *begin, const char *end, char *restrict dest)
{
	return dna4_revcomp_generic(begin, end, dest);
}
#endif
