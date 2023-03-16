/**
 * SPDX-License-Identifier: MIT
 * Copyright 2018 - 2023 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"
#include "utils.h"

#include <assert.h>
#include <emmintrin.h>
#include <string.h>

typedef __m128i vec_type;

DNA_LOCAL
size_t
dnax_count_mismatches_sse2(
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
	size_t vec_length = (length / vec_bytes) & ~(size_t)1; // round down

	size_t equal = 0;
	for (; vec_offset < vec_length; vec_offset++) {
		vec_type begin_chunk;
		vec_type other_chunk;
		memcpy(&begin_chunk, begin + vec_offset * vec_bytes, vec_bytes);
		memcpy(&other_chunk, other + vec_offset * vec_bytes, vec_bytes);

		vec_type comp = _mm_cmpeq_epi8(begin_chunk, other_chunk);

		unsigned int vmask = _mm_movemask_epi8(comp);
		equal += __builtin_popcount(vmask);

		vec_offset++;
		// second pass
		memcpy(&begin_chunk, begin + vec_offset * vec_bytes, vec_bytes);
		memcpy(&other_chunk, other + vec_offset * vec_bytes, vec_bytes);

		comp = _mm_cmpeq_epi8(begin_chunk, other_chunk);

		vmask = _mm_movemask_epi8(comp);
		equal += __builtin_popcount(vmask);
	}

	mismatches = vec_offset * vec_bytes - equal;

	size_t offset = vec_offset * vec_bytes;

	for (; offset < length; offset++) {
		if (begin[offset] != other[offset]) {
			mismatches++;
		}
	}

	return mismatches;
}
