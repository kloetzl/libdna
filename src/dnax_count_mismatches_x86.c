/**
 * SPDX-License-Identifier: MIT
 * Copyright 2018 - 2023 (C) Fabian Klötzl
 */

#include "config.h"
#include "dna.h"
#include "dna_internal.h"

#include <assert.h>

DNA_LOCAL
dnax_count_mismatches_fn *
dnax_count_mismatches_select(void)
{
	// As ifunc resolvers are called before any constructors run, we explicitly
	// have to initialize the cpu model detector.
	// https://gcc.gnu.org/onlinedocs/gcc/x86-Built-in-Functions.html
	__builtin_cpu_init();

	if (__builtin_cpu_supports("avx512bw") &&
		__builtin_cpu_supports("avx512vl")) {
		return dnax_count_mismatches_avx512;
	} else if (__builtin_cpu_supports("avx2")) {
		return dnax_count_mismatches_avx2;
	} else if (__builtin_cpu_supports("sse2")) {
		return dnax_count_mismatches_sse2;
	} else {
		return dnax_count_mismatches_generic;
	}
}

RESOLVER(
	size_t,
	dnax_count_mismatches,
	const char *,
	begin,
	const char *,
	end,
	const char *,
	other)
