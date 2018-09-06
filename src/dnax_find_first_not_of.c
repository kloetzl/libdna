/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 (C) Fabian Klötzl
 */

#include "dna.h"

char *
dnax_find_first_not_of(
	const char *restrict table, const char *begin, const char *end)
{
	const unsigned char *ubegin = (const unsigned char *)begin;

	size_t length = end - begin;
	size_t i = 0;

	for (; i < length; i++) {
		signed char c = table[ubegin[i]];
		if (c == -1) return (char *)begin + i;
	}

	return (char *)end;
}
