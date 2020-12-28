/**
 * SPDX-License-Identifier: MIT
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
		A: 'T',
		B: 'V',
		C: 'G',
		D: 'H',
		G: 'C',
		H: 'D',
		K: 'M',
		M: 'K',
		N: 'N',
		R: 'Y',
		S: 'S',
		T: 'A',
		U: 'A', // non-standard
		V: 'B',
		W: 'W',
		Y: 'R',
	};

	for (var nucl in m) {
		a[nucl.charCodeAt(0)] = m[nucl].charCodeAt(0);
		a[nucl.toLowerCase().charCodeAt(0)] =
m[nucl].toLowerCase().charCodeAt(0);
	}

	return a.join();
}

*/

// TODO: make 256

// IUPAC Code see iupac(7)
DNA_PUBLIC
const char dnax_revcomp_table[] = {
	-1, -1,  -1,  -1,  -1,  -1, -1, -1,  -1,  -1,  -1, -1,  -1, -1,  -1,  -1,
	-1, -1,  -1,  -1,  -1,  -1, -1, -1,  -1,  -1,  -1, -1,  -1, -1,  -1,  -1,
	-1, -1,  -1,  -1,  -1,  -1, -1, -1,  -1,  -1,  -1, -1,  -1, -1,  -1,  -1,
	-1, -1,  -1,  -1,  -1,  -1, -1, -1,  -1,  -1,  -1, -1,  -1, -1,  -1,  -1,
	-1, 84,  86,  71,  72,  -1, -1, 67,  68,  -1,  -1, 77,  -1, 75,  78,  -1,
	-1, -1,  89,  83,  65,  65, 66, 87,  -1,  82,  -1, -1,  -1, -1,  -1,  -1,
	-1, 116, 118, 103, 104, -1, -1, 99,  100, -1,  -1, 109, -1, 107, 110, -1,
	-1, -1,  121, 115, 97,  97, 98, 119, -1,  114, -1, -1,  -1, -1,  -1,  -1,
	-1, -1,  -1,  -1,  -1,  -1, -1, -1,  -1,  -1,  -1, -1,  -1, -1,  -1,  -1,
	-1, -1,  -1,  -1,  -1,  -1, -1, -1,  -1,  -1,  -1, -1,  -1, -1,  -1,  -1,
	-1, -1,  -1,  -1,  -1,  -1, -1, -1,  -1,  -1,  -1, -1,  -1, -1,  -1,  -1,
	-1, -1,  -1,  -1,  -1,  -1, -1, -1,  -1,  -1,  -1, -1,  -1, -1,  -1,  -1,
	-1, -1,  -1,  -1,  -1,  -1, -1, -1,  -1,  -1,  -1, -1,  -1, -1,  -1,  -1,
	-1, -1,  -1,  -1,  -1,  -1, -1, -1,  -1,  -1,  -1, -1,  -1, -1,  -1,  -1,
	-1, -1,  -1,  -1,  -1,  -1, -1, -1,  -1,  -1,  -1, -1,  -1, -1,  -1,  -1,
	-1, -1,  -1,  -1,  -1,  -1, -1, -1,  -1,  -1,  -1, -1,  -1, -1,  -1,  -1};

DNA_PUBLIC
char *
dnax_revcomp(
	const char *restrict table, const char *begin, const char *end, char *dest)
{
	assert(table != NULL);
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);
	assert(begin != dest);

	const unsigned char *ubegin = (const unsigned char *)begin;
	const unsigned char *uend = (const unsigned char *)end;
	size_t length = uend - ubegin;
	size_t destlength = 0;

	// ensure that skipping characters will not leave gaps in the output.
	for (size_t i = length; i > 0; i--) {
		unsigned char nucl = ubegin[i - 1];
		signed char value = table[nucl];
		if (value == -1) continue;

		dest[destlength++] = value;
	}

	return dest + destlength;
}
