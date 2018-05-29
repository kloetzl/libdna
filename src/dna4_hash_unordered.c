/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 (C) Fabian Klötzl
 */

#include "dna.h"

uint64_t dna4_hash_unordered(const char *begin, size_t k)
{
	uint64_t res = 0;
	for (size_t i = 0; i < k; i++) {
		res <<= 2;
		res |= (begin[k] & 6) >> 1;
	}
	return res;
}
