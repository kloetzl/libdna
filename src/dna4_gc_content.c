/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 - 2020 (C) Fabian Klötzl
 */

#include "dna.h"

#include <assert.h>

__attribute__((target_clones("avx2", "avx", "sse2", "default"))) double
dna4_gc_content(const char *begin, const char *end)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(begin <= end);

	size_t gc_count = 0;
	size_t i = 0;
	size_t length = end - begin;

	for (; i < length; i++) {
		char c = begin[i];
		if (c == 'C' || c == 'G') {
			gc_count++;
		}
	}

	return (double)gc_count / length;
}
