/**
 * SPDX-License-Identifier: MIT
 * Copyright 2022 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"
#include "utils.h"

#include <assert.h>
#include <emmintrin.h>
#include <nmmintrin.h>
#include <stdio.h>
#include <string.h>
#include <tmmintrin.h>

typedef __m128i vec_type;

static inline vec_type
chunk_squirrel3(uint32_t n, uint32_t seed)
{

	vec_type offset = _mm_setr_epi32(0, 1, 2, 3);
	vec_type chunk_n = _mm_set1_epi32(n);
	vec_type chunk_seed = _mm_set1_epi32(seed);
	vec_type chunk_noise1 = _mm_set1_epi32(NOISE1);
	vec_type chunk_noise2 = _mm_set1_epi32(NOISE2);
	vec_type chunk_noise3 = _mm_set1_epi32(NOISE3);

	vec_type chunk = _mm_add_epi32(chunk_n, offset);
	chunk = _mm_mullo_epi32(chunk, chunk_noise1);
	chunk = _mm_add_epi32(chunk, chunk_seed);
	chunk = _mm_xor_si128(chunk, _mm_srli_epi32(chunk, 8));
	chunk = _mm_add_epi32(chunk, chunk_noise2);
	chunk = _mm_xor_si128(chunk, _mm_slli_epi32(chunk, 13));
	chunk = _mm_mullo_epi32(chunk, chunk_noise3);
	chunk = _mm_xor_si128(chunk, _mm_srli_epi32(chunk, 17));

	return chunk;
}

DNA_PUBLIC
void
dna4_fill_random_sse42(char *begin, char *end, uint32_t seed)
{
	vec_type nibblecode = _mm_setr_epi8(
		'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T', 'A', 'C',
		'G', 'T');

	vec_type mask7f = _mm_set1_epi8(0x7f);

	assert(begin != NULL);
	assert(end != NULL);
	assert(begin <= end);

	seed = squirrel3(seed, NOISE4);

	size_t length = end - begin;

	size_t vec_bytes = sizeof(vec_type);
	size_t vec_length = length / vec_bytes;

	size_t vec_offset = 0;
	for (; vec_offset < vec_length; vec_offset++) {

		char *to = begin + vec_offset * vec_bytes;
		vec_type chunk = chunk_squirrel3(vec_offset * vec_bytes / 4, seed);
		chunk = _mm_and_si128(chunk, mask7f);
		vec_type mapped = _mm_shuffle_epi8(nibblecode, chunk);
		memcpy(to, &mapped, vec_bytes);
	}

	size_t rest = length - vec_offset * vec_bytes;
	if (rest) {
		char *to = begin + vec_offset * vec_bytes;
		vec_type chunk = chunk_squirrel3(vec_offset * vec_bytes / 4, seed);
		chunk = _mm_and_si128(chunk, mask7f);
		vec_type mapped = _mm_shuffle_epi8(nibblecode, chunk);
		memcpy(to, &mapped, rest);
	}
}
