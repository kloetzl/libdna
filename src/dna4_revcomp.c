/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018-2019 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"

#include <assert.h>

char *
dna4_revcomp_generic(const char *begin, const char *end, char *restrict dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	size_t length = end - begin;
	for (size_t i = 0; i < length; i++) {
		char c = begin[length - 1 - i];

		dest[i] = c ^= c & 2 ? 4 : 21;
	}

	return dest + length;
}

dna4_revcomp_fn *
dna4_revcomp_select(void)
{
	// As ifunc resolvers are called before any constructors run, we explicitly
	// have to initialize the cpu model detector.
	// https://gcc.gnu.org/onlinedocs/gcc/x86-Built-in-Functions.html
	__builtin_cpu_init();

	if (__builtin_cpu_supports("avx2")) {
		return dna4_revcomp_avx2;
	} else if (__builtin_cpu_supports("sse4.2")) {
		return dna4_revcomp_sse42;
	} else {
		return dna4_revcomp_generic;
	}
}

char *
dna4_revcomp(const char *begin, const char *end, char *dest)
	__attribute__((ifunc("dna4_revcomp_select")));
