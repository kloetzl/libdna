/**
 * SPDX-License-Identifier: MIT
 * Copyright 2018 - 2020 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"

#include <assert.h>
#include <emmintrin.h>
#include <smmintrin.h>
#include <string.h>
#include <tmmintrin.h>

typedef __m128i vec_type;

DNA_LOCAL
char *
dna4_revcomp_sse42(const char *begin, const char *end, char *restrict dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	size_t length = end - begin;

	const size_t vec_bytes = sizeof(vec_type);
	size_t vec_offset = 0;
	size_t vec_length = length / vec_bytes;

	for (; vec_offset < vec_length; vec_offset++) {
		vec_type chunk;
		memcpy(&chunk, end - (vec_offset + 1) * vec_bytes, vec_bytes);

		// reverse
		vec_type mask_shuffle =
			_mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
		vec_type reversed = _mm_shuffle_epi8(chunk, mask_shuffle);

		vec_type all_bytes_0 = _mm_set1_epi8(0);
		vec_type all_bytes_2 = _mm_set1_epi8(2);
		vec_type all_bytes_4 = _mm_set1_epi8(4);
		vec_type all_bytes_21 = _mm_set1_epi8(21);

		// cmp
		vec_type anded = _mm_and_si128(reversed, all_bytes_2);
		vec_type iszero = _mm_cmpeq_epi8(anded, all_bytes_0);

		// SSE4.2
		vec_type xor_mask = _mm_blendv_epi8(all_bytes_4, all_bytes_21, iszero);

		// SSSE3
		// vec_type is_AorT = _mm_and_si128(all_bytes_4, iszero);
		// vec_type is_CorG = _mm_andnot_si128(iszero, all_bytes_21);
		// vec_type xor_mask = _mm_or_si128(is_AorT, is_CorG);

		vec_type comp = _mm_xor_si128(xor_mask, reversed);

		size_t offset = vec_offset * vec_bytes;
		memcpy(dest + offset, &comp, vec_bytes);
	}

	for (size_t i = 0; i < length - vec_offset * vec_bytes; i++) {
		char c = begin[i];

		dest[length - 1 - i] = c ^= c & 2 ? 4 : 21;
	}

	return dest + length;
}
