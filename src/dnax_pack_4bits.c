/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 - 2020 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"

#include <assert.h>

/*

function generate_table() {
	var a = [];
	a.length = 256;
	a.fill(-1);

	var m = {
		A: 8,
		C: 4,
		G: 2,
		T: 1,
		U: 1,
		R: 0xA,
		Y: 0x5,
		S: 0x6,
		W: 0x9,
		K: 0x3,
		M: 0xC,
		B: 0x7,
		D: 0xB,
		H: 0xD,
		V: 0xE,
		N: 0xF,
		'-': 0,
	};

	for (var nucl in m) {
		a[nucl.charCodeAt(0)] = m[nucl];
		a[nucl.toLowerCase().charCodeAt(0)] = m[nucl];
	}

	return a.join();
}

*/

const char dnax_pack_4bits_table[] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, 0,  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, 8,  7,  4,  11, -1, -1, 2,  13, -1, -1, 3,
	-1, 12, 15, -1, -1, -1, 10, 6,  1,  1,  14, 9,  -1, 5,  -1, -1, -1, -1, -1,
	-1, -1, 8,  7,  4,  11, -1, -1, 2,  13, -1, -1, 3,  -1, 12, 15, -1, -1, -1,
	10, 6,  1,  1,  14, 9,  -1, 5,  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1};

DNA_PUBLIC
size_t
dnax_pack_4bits(const char *begin, const char *end, unsigned char *dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(begin <= end);
	assert(dest != NULL);

	size_t counter = 0;
	const char *ptr = dnax_find_first_of(dnax_pack_4bits_table, begin, end);

	while (ptr < end) {
		unsigned int bits = dnax_pack_4bits_table[*(unsigned char *)ptr];
		counter++;

		ptr = dnax_find_first_of(dnax_pack_4bits_table, ptr + 1, end);
		if (ptr < end) {
			unsigned int high_nibble =
				dnax_pack_4bits_table[*(unsigned char *)ptr];

			counter++;
			bits |= high_nibble << 4;
			ptr = dnax_find_first_of(dnax_pack_4bits_table, ptr + 1, end);
		}

		*dest++ = bits;
	}

	return counter;
}
