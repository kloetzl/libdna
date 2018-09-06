/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 (C) Fabian Klötzl
 */

#include "dna.h"
#include "utils.h"

#include <assert.h>

/*

function generate_table() {
	var a = [];
	a.length = 256;
	a.fill(-1);

	var m = {
		A: 'A',
		C: 'C',
		G: 'G',
		T: 'T',
		U: 'T'
	};

	for (var nucl in m) {
		a[nucl.charCodeAt(0)] = m[nucl].charCodeAt(0);
		a[nucl.toLowerCase().charCodeAt(0)] = m[nucl].charCodeAt(0);
	}

	return a.join();
}

*/

const char dnax_to_dna4_table[] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, 65, -1, 67, -1, -1, -1, 71, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, 84, 84, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, 65, -1, 67, -1, -1, -1, 71, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, 84, 84, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1};

char *
dnax_replace(
	const char *restrict table, const char *begin, const char *end, char *dest)
{
	assert(table != NULL);
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	unsigned char *ubegin = (unsigned char *)begin;
	unsigned char *uend = (unsigned char *)end;
	size_t from = 0;
	size_t to = 0;
	size_t length = uend - ubegin;

	for (; from < length; from++) {
		unsigned char c = ubegin[from];
		signed char d = table[c];
		if (UNLIKELY(d == -1)) continue;

		dest[to++] = d;
	}

	return dest + to;
}
