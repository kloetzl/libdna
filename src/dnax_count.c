/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 (C) Fabian Klötzl
 */

#include "dna.h"
#include <string.h>
#include <assert.h>

// CHAR_BIT

void dnax_count(size_t *restrict table, const char *begin, const char *end)
{
	assert(table != NULL);
	assert(begin != NULL);
	assert(end != NULL);
	assert(begin <= end);

	memset(table, 0, sizeof(*table) * 128);

	const unsigned char *ubegin = (const unsigned char *)begin;
	const unsigned char *uend = (const unsigned char *)end;

	size_t length = uend - ubegin;
	for (size_t i = 0; i < length; i++) {
		table[ubegin[i]]++;
	}
}
