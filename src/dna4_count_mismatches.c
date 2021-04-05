/**
 * SPDX-License-Identifier: MIT
 * Copyright 2018 - 2021 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"
#include "utils.h"

#include <assert.h>

DNA_LOCAL
size_t
dna4_count_mismatches_generic(
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
		if (UNLIKELY(begin[i] != other[i])) {
			mismatches++;
		}
	}

	return mismatches;
}

#if !defined(__ARM_NEON) && !defined(__x86_64)
DNA_PUBLIC
size_t
dna4_count_mismatches(const char *begin, const char *end, const char *other)
{
	return dna4_count_mismatches_generic(begin, end, other);
}
#endif
