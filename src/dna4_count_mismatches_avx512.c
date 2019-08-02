/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 (C) Fabian Klötzl
 */

#include "dna.h"
#include "utils.h"

#include <assert.h>
#include <immintrin.h>
#include <string.h>

typedef __m256i vec_type;

size_t
dna4_count_mismatches_avx512(
	const char *begin, const char *end, const char *other)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(other != NULL);
	assert(begin <= end);

	size_t mismatches = 0;
	size_t length = end - begin;

	const size_t vec_bytes = sizeof(vec_type);
	size_t vec_offset = 0;
	size_t vec_length = length / vec_bytes;

	size_t equal = 0;
	for (; vec_offset < vec_length; vec_offset++) {
		vec_type begin_chunk;
		vec_type other_chunk;
		memcpy(&begin_chunk, begin + vec_offset * vec_bytes, vec_bytes);
		memcpy(&other_chunk, other + vec_offset * vec_bytes, vec_bytes);

		unsigned int vmask = _mm256_cmpeq_epi8_mask(begin_chunk, other_chunk);
		equal += __builtin_popcount(vmask);
	}

	mismatches = vec_length * vec_bytes - equal;

	size_t offset = vec_offset * vec_bytes;
	for (; offset < length; offset++) {
		if (begin[offset] != other[offset]) {
			mismatches++;
		}
	}

	return mismatches;
}
