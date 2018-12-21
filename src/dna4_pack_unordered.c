/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 (C) Fabian Klötzl
 */

#include "dna.h"

#include <assert.h>

__attribute__((target_clones("avx2", "avx", "sse2", "default"))) uint64_t
dna4_pack_unordered(const char *begin, size_t k)
{
	assert(begin != NULL);
	assert(k <= 32);

	uint64_t res = 0;
	for (size_t i = 0; i < k; i++) {
		res <<= 2;
		res |= (begin[i] & 6) >> 1;
	}

	return res;
}
