/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 (C) Fabian Klötzl
 */

#include "dna.h"
#include "utils.h"

#include <assert.h>

double
dna4_count_mismatches_generic(
	const char *begin,
	const char *end,
	const char *other,
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
		if (UNLIKELY(begin[i] != other[i])) {
			substitutions++;
		}
	}

	if (substitutions_ptr) *substitutions_ptr = substitutions;

	// math
	double rate = (double)substitutions / length;
	double dist = -0.75 * dna_utils_log(1.0 - (4.0 / 3.0) * rate);

	return dist;
}

dna4_count_mismatches_fn *
dna4_count_mismatches_select(void)
{
	// As ifunc resolvers are called before any constructors run, we explicitly
	// have to initialize the cpu model detector.
	// https://gcc.gnu.org/onlinedocs/gcc/x86-Built-in-Functions.html
	__builtin_cpu_init();

	if (__builtin_cpu_supports("avx512bw") &&
		__builtin_cpu_supports("avx512vl")) {
		return dna4_count_mismatches_avx512;
	} else if (__builtin_cpu_supports("avx2")) {
		return dna4_count_mismatches_avx2;
	} else if (__builtin_cpu_supports("sse2")) {
		return dna4_count_mismatches_sse2;
	} else {
		return dna4_count_mismatches_generic;
	}
}

double
dna4_count_mismatches(
	const char *begin,
	const char *end,
	const char *other,
	size_t *substitutions_ptr) __attribute__((ifunc("dna4_count_mismatches_select")));
