/**
 * SPDX-License-Identifier: MIT
 * Copyright 2018-2020 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"

#include <assert.h>

DNA_LOCAL
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

DNA_LOCAL
dna4_revcomp_fn *
dna4_revcomp_select(void)
{
#ifdef __x86_64
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
#else
	return dna4_revcomp_generic;
#endif
}

#if CAN_IFUNC && __has_attribute(ifunc)

DNA_PUBLIC
char *
dna4_revcomp(const char *begin, const char *end, char *dest)
	__attribute__((ifunc("dna4_revcomp_select")));

#else

// If ifunc is unavailable (for instance on macOS or hurd) we have to implement
// the functionality ourselves. Using a function pointer is faster than a
// boolean variable.

char *
dna4_revcomp_callonce(const char *begin, const char *end, char *dest);

static dna4_revcomp_fn *dna4_revcomp_fnptr = dna4_revcomp_callonce;

DNA_LOCAL
char *
dna4_revcomp_callonce(const char *begin, const char *end, char *dest)
{
	dna4_revcomp_fnptr = dna4_revcomp_select();
	return dna4_revcomp_fnptr(begin, end, dest);
}

DNA_PUBLIC
char *
dna4_revcomp(const char *begin, const char *end, char *dest)
{
	return dna4_revcomp_fnptr(begin, end, dest);
}

#endif
