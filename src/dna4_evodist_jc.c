/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 (C) Fabian Klötzl
 */

#include "dna.h"
#include "utils.h"
#include <assert.h>

__attribute__((target_clones("avx", "sse2", "default"))) double
dna4_evodist_jc(const char *begin, const char *end, const char *other,
				size_t *substitutions_ptr)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(other != NULL);
	assert(begin <= end);

	size_t substitutions = 0;
	size_t i = 0;
	size_t length = end - begin;

	for (; i < length; i++) {
		if (begin[i] != other[i]) {
			substitutions++;
		}
	}

	if (substitutions_ptr) *substitutions_ptr = substitutions;

	// math
	double rate = (double)substitutions / length;
	double dist = -0.75 * dna_utils_log(1.0 - (4.0 / 3.0) * rate);

	return dist;
}
