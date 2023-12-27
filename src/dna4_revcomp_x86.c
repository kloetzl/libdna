/**
 * SPDX-License-Identifier: MIT
 * Copyright 2018 - 2023 (C) Fabian Klötzl
 */

#include "config.h"
#include "dna.h"
#include "dna_internal.h"

#include <assert.h>

DNA_LOCAL
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

RESOLVER(
	char *, dna4_revcomp, const char *, begin, const char *, end, char *, dest)
