/**
 * SPDX-License-Identifier: MIT
 * Copyright 2019 - 2020 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"

#include <assert.h>

/*

function generate_table() {
	var a = [];
	a.length = 16;
	a.fill(-1);

	var m = {
		A: 8,
		C: 4,
		G: 2,
		T: 1,
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
		a[m[nucl]] = nucl;
	}

	return a.join();
}

*/

static const char dnax_unpack_4bits_table[] = { //
	'-', 'T', 'G', 'K', 'C', 'Y', 'S', 'B',
	'A', 'W', 'R', 'D', 'M', 'H', 'V', 'N'};

DNA_PUBLIC
char *
dnax_unpack_4bits(
	const unsigned char *begin, const unsigned char *end, char *dest)
{
	const unsigned char *ubegin = (const unsigned char *)begin;
	const unsigned char *uend = (const unsigned char *)end;

	for (const unsigned char *uptr = ubegin; uptr < uend; uptr++) {
		unsigned int low_nibble = *uptr & 0xF;
		unsigned int high_nibble = (*uptr & 0xF0) >> 4;
		*dest++ = dnax_unpack_4bits_table[low_nibble];
		*dest++ = dnax_unpack_4bits_table[high_nibble];
	}

	return dest;
}
