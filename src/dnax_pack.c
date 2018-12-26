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

const char dnax_pack_table[] = {
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

/* The signature of `dnax_pack` used to be as follows

uint64_t
dnax_pack(
	const char *restrict table,
	const char *begin,
	size_t k)

The problem is that this requires the user to prescan the string for k valid
characters prior to the call. Otherwise this function would run off the end. In
order to be safe one would have to duplicate the work, basically. So I changed
the signature to a safer one:

uint64_t
dnax_pack(
	const char *restrict table,
	const char *begin,
	const char *end,
	size_t k)

This takes care of the buffer overflow but leaves further issues. I expect the
user to call dnax_pack in a loop.

Case 1: overlapping k-mers

	dnax_pack(table, begin, end, k)
	dnax_pack(table, begin + 1, end, k)
	dnax_pack(table, begin + 2, end, k)
	…

Case 2: unique k-mers

	dnax_pack(table, begin, end, k)
	dnax_pack(table, begin + k, end, k)
	dnax_pack(table, begin + k * 2, end, k)
	…

Due to skipped nucleotides the starting position of the next k-mer may be
non-trivial to compute. So it would be cool if dnax_pack would be able to return
the starting position of the next k-mer. That would require an extra output
parameter. But until I have figured out which use-case is preferred, this
mediocre but safe API will have to do.

*/

uint64_t
dnax_pack(
	const char *restrict table, const char *begin, const char *end, size_t k)
{
	assert(table != NULL);
	assert(begin != NULL);
	assert(k <= 32);
	assert(end != NULL);
	assert(begin <= end);
	// The code should work even if the assert is violated.
	assert(begin + k <= end);

	uint64_t result = 0;
	size_t readpos = 0;
	size_t length = end - begin;

	const unsigned char *ubegin = (const unsigned char *)begin;

	for (size_t i = 0; i < k && readpos < length; readpos++) {
		signed char value = table[ubegin[readpos]];
		if (value == -1) continue;

		result <<= 2;
		result |= value;
		i++;
	}

	return result;
}
