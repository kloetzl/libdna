/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 (C) Fabian Klötzl
 */

#include "dna.h"

#include <assert.h>

/**
 * Somewhat surprisingly, using a table is faster than twiddling. I am guessing
 * this is because of the smaller number of instructions.
 */

uint64_t
dna4_pack(const char *begin, size_t k)
{
	assert(begin != NULL);
	assert(k <= 32);

	const unsigned char *ubegin = (const unsigned char *)begin;

	uint64_t res = 0;
	for (size_t i = 0; i < k; i++) {
		char c = dnax_pack_table[ubegin[i]];
		res <<= 2;
		res |= c;
	}

	return res;
}

// uint64_t dna4_pack_direct(const char *begin, size_t k)
// {
// 	uint64_t res = 0;
// 	for (size_t i = 0; i < k; i++) {
// 		res <<= 2;
// 		char c = (begin[i] & 6) >> 1;
// 		res |= c ^ (c >> 1);
// 	}
// 	return res;
// }