/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 (C) Fabian Klötzl
 */

#include "dna.h"
#include <ctype.h>
// #include <emmintrin.h>
#include <assert.h>

// __attribute__((target_clones("avx", "sse2", "default")))
char *dnax_to_dna4(const char *begin, const char *end, char *dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);
	// dest == begin is allowed

	for (; begin < end; begin++) {
		char c = toupper(*begin);
		if (c == 'A' || c == 'C' || c == 'G' || c == 'T') {
			*dest++ = c;
		}
	}

	return dest;
}

// char *dnax_to_dna4_avx(char *begin, char *end)
// {
// 	char *target = begin;
// 	for (; begin < end; begin++) {
// 		char c = *begin;
// 		if (c == 'A' || c == 'C' || c == 'G' || c == 'T') {
// 			*target++ = c;
// 		}
// 	}

// 	return target;
// }

// char *dnax_to_dna4_despace(char *begin, char *end)
// {
// 	size_t pos = 0;
// 	__m128i AA = _mm_set1_epi8('A');
// 	.__m128i CC = _mm_set1_epi8('C');
// 	__m128i GG = _mm_set1_epi8('G');
// 	__m128i TT = _mm_set1_epi8('T');

// 	size_t howmany = end - begin;
// 	size_t i = 0;
// 	for (; i + 15 < howmany; i += 16) {
// 		__m128i x = _mm_loadu_si128((const __m128i *)(begin + i));
// 		// we do it the naive way, could be smarter?
// 		__m128i xAA = _mm_cmpeq_epi8(x, AA);
// 		__m128i xCC = _mm_cmpeq_epi8(x, CC);
// 		__m128i xGG = _mm_cmpeq_epi8(x, GG);
// 		__m128i xTT = _mm_cmpeq_epi8(x, TT);

// 		__m128i any =
// 			_mm_or_si128(_mm_or_si128(xAA, xTT), _mm_or_si128(xCC, xGG));

// 		int mask16 = ~_mm_movemask_epi8(any);
// 		if (mask16 == 0) { // no match!
// 			_mm_storeu_si128((__m128i *)(begin + pos), x);
// 			pos += 16;
// 		} else {
// 			for (int k = 0; k < 16; ++k) {
// 				if (mask16 & (1 << k)) continue;
// 				begin[pos++] = begin[i + k];
// 			}
// 		}
// 	}
// 	for (; i < howmany; i++) {
// 		char c = begin[i];
// 		if (!(c == 'A' || c == 'C' || c == 'G' || c == 'T')) {
// 			continue;
// 		}
// 		begin[pos++] = c;
// 	}
// 	return begin + pos;
// }
