/**
 * SPDX-License-Identifier: MIT
 * Copyright 2022 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"
#include "utils.h"

#include <assert.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <smmintrin.h>
#include <stdio.h>
#include <string.h>
#include <tmmintrin.h>

typedef __m256i vec_type;

static inline vec_type
chunk_squirrel3(uint32_t n, uint32_t seed)
{

	vec_type offset = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
	vec_type chunk_n = _mm256_set1_epi32(n);
	vec_type chunk_seed = _mm256_set1_epi32(seed);
	vec_type chunk_noise1 = _mm256_set1_epi32(NOISE1);
	vec_type chunk_noise2 = _mm256_set1_epi32(NOISE2);
	vec_type chunk_noise3 = _mm256_set1_epi32(NOISE3);

	vec_type chunk = _mm256_add_epi32(chunk_n, offset);
	chunk = _mm256_mullo_epi32(chunk, chunk_noise1);
	chunk = _mm256_add_epi32(chunk, chunk_seed);
	chunk = _mm256_xor_si256(chunk, _mm256_srli_epi32(chunk, 8));
	chunk = _mm256_add_epi32(chunk, chunk_noise2);
	chunk = _mm256_xor_si256(chunk, _mm256_slli_epi32(chunk, 13));
	chunk = _mm256_mullo_epi32(chunk, chunk_noise3);
	chunk = _mm256_xor_si256(chunk, _mm256_srli_epi32(chunk, 17));

	return chunk;
}

DNA_PUBLIC
void
dna4_fill_random_avx2(char *begin, char *end, uint32_t seed)
{
	vec_type nibblecode = _mm256_setr_epi8(
		'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T', 'A', 'C',
		'G', 'T', 'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T',
		'A', 'C', 'G', 'T');

	vec_type mask7f = _mm256_set1_epi8(0x7f);

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
		chunk = _mm256_and_si256(chunk, mask7f);
		vec_type mapped = _mm256_shuffle_epi8(nibblecode, chunk);
		memcpy(to, &mapped, vec_bytes);
	}

	size_t rest = length - vec_offset * vec_bytes;
	if (rest) {
		char *to = begin + vec_offset * vec_bytes;
		vec_type chunk = chunk_squirrel3(vec_offset * vec_bytes / 4, seed);
		chunk = _mm256_and_si256(chunk, mask7f);
		vec_type mapped = _mm256_shuffle_epi8(nibblecode, chunk);
		memcpy(to, &mapped, rest);
	}
}
