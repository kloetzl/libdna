/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2019 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"

#include <assert.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <smmintrin.h>
#include <string.h>
#include <tmmintrin.h>

typedef __m256i vec_type;

DNA_LOCAL
char *
dna4_revcomp_avx2(const char *begin, const char *end, char *restrict dest)
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
		memcpy(&chunk, begin + vec_offset * vec_bytes, vec_bytes);

		// reverse
		vec_type mask_shuffle = _mm256_set_epi8(
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4,
			5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
		vec_type almostreversed = _mm256_shuffle_epi8(chunk, mask_shuffle);
		vec_type reversed =
			_mm256_permute2x128_si256(almostreversed, almostreversed, 1);

		vec_type all_bytes_0 = _mm256_set1_epi8(0);
		vec_type all_bytes_2 = _mm256_set1_epi8(2);
		vec_type all_bytes_4 = _mm256_set1_epi8(4);
		vec_type all_bytes_21 = _mm256_set1_epi8(21);

		// cmp
		vec_type anded = _mm256_and_si256(reversed, all_bytes_2);
		vec_type iszero = _mm256_cmpeq_epi8(anded, all_bytes_0);

		vec_type xor_mask =
			_mm256_blendv_epi8(all_bytes_4, all_bytes_21, iszero);
		vec_type comp = _mm256_xor_si256(xor_mask, reversed);

		memcpy(
			dest + length - vec_offset * vec_bytes - vec_bytes, &comp,
			vec_bytes);
	}

	for (size_t i = vec_offset * vec_bytes; i < length; i++) {
		char c = begin[i];

		dest[length - 1 - i] = c ^= c & 2 ? 4 : 21;
	}

	return dest + length;
}
