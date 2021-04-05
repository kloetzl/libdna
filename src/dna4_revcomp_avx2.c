/**
 * SPDX-License-Identifier: MIT
 * Copyright 2019 - 2020 (C) Fabian Klötzl
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

	vec_type mask_shuffle = _mm256_set_epi8(
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5,
		6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

	vec_type nibblecode = _mm256_setr_epi8(
		0, 'T', 0, 'G', 'A', 0, 0, 'C', 0, 0, 0, 0, 0, 0, 0, 0, 0, 'T', 0, 'G',
		'A', 0, 0, 'C', 0, 0, 0, 0, 0, 0, 0, 0);

	for (; vec_offset < vec_length; vec_offset++) {
		vec_type chunk;
		memcpy(&chunk, end - (vec_offset + 1) * vec_bytes, vec_bytes);

		vec_type almostreversed = _mm256_shuffle_epi8(chunk, mask_shuffle);
		vec_type reversed =
			_mm256_permute2x128_si256(almostreversed, almostreversed, 1);

		vec_type mapped = _mm256_shuffle_epi8(nibblecode, reversed);

		memcpy(dest + vec_offset * vec_bytes, &mapped, vec_bytes);
	}

	for (size_t i = 0; i < length - vec_offset * vec_bytes; i++) {
		char c = begin[i];

		dest[length - 1 - i] = c ^= c & 2 ? 4 : 21;
	}

	return dest + length;
}
