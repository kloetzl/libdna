/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 (C) Fabian Klötzl
 */

#include "dna.h"

char *dnax_replace(const char *restrict table, const char *begin,
				   const char *end, char *dest)
{
	unsigned char *ubegin = (unsigned char *)begin;
	unsigned char *uend = (unsigned char *)end;
	size_t from = 0;
	size_t to = 0;
	size_t length = uend - ubegin;

	for (; from < length; from++) {
		unsigned char c = ubegin[from];
		signed char d = table[c];
		if (d == -1) continue;

		dest[to++] = d;
	}

	return dest + to;
}
