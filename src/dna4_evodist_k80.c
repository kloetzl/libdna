/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 (C) Fabian Klötzl
 */

#include "dna.h"
#include "utils.h"

/*__attribute__((target_clones("avx", "sse2", "default"))) */
double dna4_evodist_k80(const char *begin, const char *end, const char *other,
						size_t *transitions_ptr, size_t *transversions_ptr)
{
	size_t transitions = 0, transversions = 0;
	size_t i = 0;
	size_t length = end - begin;

	for (; i < length; i++) {
		if (begin[i] != other[i]) {
			if ((begin[i] == 'G' || begin[i] == 'C') ^
				(other[i] == 'G' || other[i] == 'C')) {
				transversions++;
			} else {
				transitions++;
			}
		}
	}

	if (transitions_ptr) *transitions_ptr = transitions;
	if (transversions_ptr) *transversions_ptr = transversions;

	// math
	double P = (double)transitions / length;
	double Q = (double)transversions / length;

	double tmp = 1.0 - 2.0 * P - Q;
	double dist = -0.25 * dna_utils_log((1.0 - 2.0 * Q) * tmp * tmp);

	return dist;
}
