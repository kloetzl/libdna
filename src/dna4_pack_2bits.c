/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 - 2020 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"

#include <assert.h>

/**
 * Somewhat surprisingly, using a table is faster than twiddling. I am guessing
 * this is because of the smaller number of instructions.
 */

/*

function generate_table() {
	var a = [];
	a.length = 256;
	a.fill(-1);

	var m = {
		A: 0,
		C: 1,
		G: 2,
		T: 3
	};

	for (var nucl in m) {
		a[nucl.charCodeAt(0)] = m[nucl];
	}

	return a.join();
}

*/

static const char dna4_pack_2bits_table[] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, 0,  -1, 1,  -1, -1, -1, 2,  -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, 3,  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1};

DNA_PUBLIC
uint64_t
dna4_pack_2bits(const char *begin, size_t k)
{
	assert(begin != NULL);
	assert(k <= 32);

	const unsigned char *ubegin = (const unsigned char *)begin;

	uint64_t res = 0;
	for (size_t i = 0; i < k; i++) {
		char c = dna4_pack_2bits_table[ubegin[i]];
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
