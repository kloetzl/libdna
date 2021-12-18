/**
 * SPDX-License-Identifier: MIT
 * Copyright 2018 - 2021 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"

#include <assert.h>

DNA_PUBLIC
char *
dnax_revcomp(
	const char *restrict table, const char *begin, const char *end, char *dest)
{
	assert(table != NULL);
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);
	assert(begin != dest);

	const unsigned char *ubegin = (const unsigned char *)begin;
	const unsigned char *uend = (const unsigned char *)end;
	size_t length = uend - ubegin;
	size_t destlength = 0;

	// ensure that skipping characters will not leave gaps in the output.
	for (size_t i = length; i > 0; i--) {
		unsigned char nucl = ubegin[i - 1];
		signed char value = table[nucl];
		if (value == -1) continue;

		dest[destlength++] = value;
	}

	return dest + destlength;
}
