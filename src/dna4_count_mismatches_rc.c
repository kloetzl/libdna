/**
 * SPDX-License-Identifier: MIT
 * Copyright 2021 - 2022 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"
#include "utils.h"

#include <assert.h>

DNA_LOCAL
size_t
dna4_count_mismatches_rc_generic(
	const char *begin, const char *end, const char *other)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(other != NULL);
	assert(begin <= end);

	size_t mismatches = 0;
	size_t i = 0;
	size_t length = end - begin;

	for (; i < length; i++) {
		int val = begin[i] ^ other[length - i - 1];
		if (UNLIKELY((val & 6) != 4)) {
			mismatches++;
		}
	}

	return mismatches;
}

#if !defined(__x86_64)
DNA_PUBLIC
size_t
dna4_count_mismatches_rc(const char *begin, const char *end, const char *other)
{
	return dna4_count_mismatches_rc_generic(begin, end, other);
}
#endif
