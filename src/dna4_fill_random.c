/**
 * SPDX-License-Identifier: MIT
 * Copyright 2021 - 2022 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"

#include <assert.h>
#include <inttypes.h>
#include <string.h>

// This noise function is based on a method by Squirrel Eiserloh.

DNA_LOCAL const uint32_t NOISE1 =
	0xb5297a4d; // 0b0110'1000'1110'0011'0001'1101'1010'0100
DNA_LOCAL const uint32_t NOISE2 =
	0x68e31da4; // 0b1011'0101'0010'1001'0111'1010'0100'1101
DNA_LOCAL const uint32_t NOISE3 =
	0x1b56c4e9; // 0b0001'1011'0101'0110'1100'0100'1110'1001

DNA_LOCAL const uint32_t NOISE4 = 0xaaea97a5; // determined by fair dice roll

DNA_LOCAL
uint32_t
squirrel3(uint32_t n, uint32_t seed)
{
	n *= NOISE1;
	n += seed;
	n ^= n >> 8;
	n += NOISE2;
	// n is the index into the sequence. Below we use consecutive values of n.
	// So for n, n+1, …, n+4, … we need very different outputs. So we need to
	// propagate a change in the lower bits up to get more random results.
	n ^= n << 13; // was: 8
	n *= NOISE3;
	// The original method would always shift along byte borders. I here changed
	// to shifting down 17 bits, rather than just 8, to increase the entropy of
	// the lower bits.
	n ^= n >> 17;
	return n;
}

DNA_LOCAL
void
dna4_fill_random_generic(char *dest, char *end, uint32_t seed)
{
	assert(dest != NULL);
	assert(end != NULL);
	assert(dest <= end);

	// humans are notoriously bad at picking good seeds. Hence we hash it once
	// to produce a 'more random' seed.
	seed = squirrel3(seed, NOISE4);

	size_t length = end - dest;
	char ACGT[4] = {'A', 'C', 'G', 'T'};

	size_t i = 0;
	for (; i < (length & ~3); i += 4) {
		uint32_t noise = squirrel3(i / 4, seed);

		char buffer[4];
		buffer[0] = ACGT[(noise >> 0) & 3];
		buffer[1] = ACGT[(noise >> 8) & 3];
		buffer[2] = ACGT[(noise >> 16) & 3];
		buffer[3] = ACGT[(noise >> 24) & 3];
		memcpy(dest + i, buffer, sizeof(buffer));
	}

	size_t rest = length - i;
	if (rest) {
		uint32_t noise = squirrel3(i / 4, seed);

		char buffer[4];
		buffer[0] = ACGT[(noise >> 0) & 3];
		buffer[1] = ACGT[(noise >> 8) & 3];
		buffer[2] = ACGT[(noise >> 16) & 3];
		buffer[3] = ACGT[(noise >> 24) & 3];
		memcpy(dest + i, buffer, rest);
	}
}

#if !defined(__x86_64)
DNA_PUBLIC
char *
dna4_fill_random(const char *begin, const char *end, char *dest)
{
	return dna4_fill_random_generic(begin, end, dest);
}
#endif
