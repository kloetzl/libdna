/**
 * SPDX-License-Identifier: MIT
 * Copyright 2021 (C) Fabian Klötzl
 */

#include "dna.h"
#include "utils.h"

#include <assert.h>
#include <immintrin.h>
#include <string.h>

size_t
dna4_count_mismatches_rc_avx2(
	const char *begin, const char *end, const char *other)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(other != NULL);
	assert(begin <= end);

	size_t length = end - begin;
	size_t substitutions = 0;
	size_t offset = 0;

	typedef __m256i vec_type;
	size_t vec_size = sizeof(vec_type);
	size_t vec_offset = 0;
	size_t vec_length = length / vec_size;

	substitutions += vec_size * vec_length;

	vec_type nibblecode = _mm256_setr_epi8(
		'0', 'T', '2', 'G', 'A', '5', '6', 'C', '8', '9', 'a', 'b', 'c', 'd',
		'e', 'f', '0', 'T', '2', 'G', 'A', '5', '6', 'C', '8', '9', 'a', 'b',
		'c', 'd', 'e', 'f');
	vec_type mask = _mm256_set_epi8(
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5,
		6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

	for (; vec_offset < vec_length; vec_offset++) {
		vec_type b;
		memcpy(&b, begin + vec_offset * vec_size, vec_size);
		vec_type o;
		size_t pos = length - (vec_offset + 1) * vec_size;
		memcpy(&o, other + pos, vec_size);

		vec_type reversed = _mm256_shuffle_epi8(o, mask);
		vec_type revcomped = _mm256_shuffle_epi8(nibblecode, reversed);
		vec_type swapped = _mm256_permute2x128_si256(b, b, 1);

		vec_type v3 = _mm256_cmpeq_epi8(revcomped, swapped);

		unsigned int vmask = _mm256_movemask_epi8(v3);
		substitutions -= __builtin_popcount(vmask);
	}

	offset += vec_offset * vec_size;

	for (; offset < length; offset++) {
		int val = begin[offset] ^ other[length - 1 - offset];
		if ((val & 6) != 4) {
			substitutions++;
		}
	}

	return substitutions;
}
