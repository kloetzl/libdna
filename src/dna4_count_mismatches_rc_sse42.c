/**
 * SPDX-License-Identifier: MIT
 * Copyright 2021 - 2022 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"
#include "utils.h"

#include <assert.h>
#include <emmintrin.h>
#include <string.h>
#include <tmmintrin.h>

DNA_LOCAL
size_t
dna4_count_mismatches_rc_sse42(
	const char *begin, const char *end, const char *other)
{
	size_t substitutions = 0;
	size_t offset = 0;
	size_t length = end - begin;

	typedef __m128i vec_type;
	size_t vec_offset = 0;
	size_t vec_size = sizeof(vec_type);
	size_t vec_length = length / vec_size;

	vec_type nibblecode = _mm_setr_epi8(
		'0', 'T', '2', 'G', 'A', '5', '6', 'C', '8', '9', 'a', 'b', 'c', 'd',
		'e', 'f');
	vec_type mask =
		_mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

	substitutions += vec_size * vec_length;
	for (; vec_offset < vec_length; vec_offset++) {
		vec_type b;
		memcpy(&b, begin + vec_offset * vec_size, sizeof(b));
		vec_type o;
		size_t pos = length - (vec_offset + 1) * vec_size;
		memcpy(&o, other + pos, sizeof(o));

		vec_type reversed = _mm_shuffle_epi8(o, mask);
		vec_type complemented = _mm_shuffle_epi8(nibblecode, b);
		vec_type v3 = _mm_cmpeq_epi8(complemented, reversed);

		unsigned int vmask = _mm_movemask_epi8(v3);
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
