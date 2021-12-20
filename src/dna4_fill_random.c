/**
 * SPDX-License-Identifier: MIT
 * Copyright 2021 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"

#include <assert.h>
#include <inttypes.h>

static uint32_t NOISE1 =
	0xb5297a4d; // 0b0110'1000'1110'0011'0001'1101'1010'0100
static uint32_t NOISE2 =
	0x68e31da4; // 0b1011'0101'0010'1001'0111'1010'0100'1101
static uint32_t NOISE3 =
	0x1b56c4e9; // 0b0001'1011'0101'0110'1100'0100'1110'1001

// This noise function is based on a method by Squirrel Eiserloh.

static uint32_t
squirrel3(uint32_t n, uint32_t seed)
{
	n *= NOISE1;
	n += seed;
	n ^= n >> 8;
	n += NOISE2;
	n ^= n << 8;
	n *= NOISE3;
	n ^= n >> 8;
	return n;
}

DNA_PUBLIC
void
dna4_fill_random(char *dest, char *end, uint32_t seed)
{
	assert(dest != NULL);
	assert(end != NULL);
	assert(dest <= end);

	size_t length = end - dest;
	char table[4] = {'A', 'C', 'G', 'T'};
	for (size_t i = 0; i < length; i++) {
		uint32_t rnd = squirrel3(i, seed);
		dest[i] = table[rnd & 3];
	}
}
