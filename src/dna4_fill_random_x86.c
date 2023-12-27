/**
 * SPDX-License-Identifier: MIT
 * Copyright 2022 - 2023 (C) Fabian Klötzl
 */

#include "config.h"
#include "dna.h"
#include "dna_internal.h"

#include <assert.h>

DNA_LOCAL
dna4_fill_random_fn *
dna4_fill_random_select(void)
{
	// As ifunc resolvers are called before any constructors run, we explicitly
	// have to initialize the cpu model detector.
	// https://gcc.gnu.org/onlinedocs/gcc/x86-Built-in-Functions.html
	__builtin_cpu_init();

	if (__builtin_cpu_supports("avx2")) {
		return dna4_fill_random_avx2;
	} else if (__builtin_cpu_supports("sse4.2")) {
		return dna4_fill_random_sse42;
	} else {
		return dna4_fill_random_generic;
	}
}

RESOLVER_VOID(void, dna4_fill_random, char *, dest, char *, end, uint32_t, seed)
