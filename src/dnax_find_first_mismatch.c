/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 - 2020 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"

#include <assert.h>
#include <string.h>

DNA_PUBLIC
char *
dnax_find_first_mismatch(const char *begin, const char *end, const char *other)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(begin <= end);
	assert(other != NULL);
	assert(begin != other);

	size_t length = end - begin;
	size_t i = 0;

	static const size_t chunk_size = 8;

	size_t chunked_length = length - (length % chunk_size);
	for (; i < chunked_length; i += chunk_size) {
		int check = memcmp(begin + i, other + i, chunk_size);
		if (check) {
			break;
		}
	}

	for (; i < length; i++) {
		if (begin[i] != other[i]) break;
	}

	return (char *)begin + i;
}
