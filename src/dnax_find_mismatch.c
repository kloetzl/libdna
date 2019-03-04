/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 (C) Fabian Klötzl
 */

#include "dna.h"

char *
dnax_mismatch(const char *begin, const char *end, const char *other)
{
	size_t length = end - begin;

	for (size_t i = 0; i < length; i++) {
		if (begin[i] != other[i]) return (char *)begin + i;
	}

	return (char *)end;
}
