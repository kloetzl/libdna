/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 (C) Fabian Klötzl
 */

#include "dna.h"

#include <assert.h>

/*

function generate_table() {
	var a = [];
	a.length = 256;
	a.fill(-1);

	var m = {
		A: 0,
		C: 1,
		G: 2,
		T: 3,
		U: 3,
	};

	for (var nucl in m) {
		a[nucl.charCodeAt(0)] = m[nucl];
		a[nucl.toLowerCase().charCodeAt(0)] = m[nucl];
	}

	return a.join();
}

*/

const char dnax_hash_table[] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, 0,  -1, 1,  -1, -1, -1, 2,  -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, 3,  3,  -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, 0,  -1, 1,  -1, -1, -1, 2,  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, 3,  3,  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1};

uint64_t
dnax_hash(const char *restrict table, const char *begin, size_t k)
{
	assert(table != NULL);
	assert(begin != NULL);
	assert(k <= 32);

	uint64_t result = 0;
	size_t readpos = 0;

	const unsigned char *ubegin = (const unsigned char *)begin;

	for (size_t i = 0; i < k;) {
		signed char value = table[ubegin[readpos++]];
		if (value == -1) continue;

		result <<= 2;
		result |= value;
		i++;
	}

	return result;
}
