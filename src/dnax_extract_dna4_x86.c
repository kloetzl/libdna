/**
 * SPDX-License-Identifier: MIT
 * Copyright 2018 - 2023 (C) Fabian Klötzl
 */

#include "config.h"
#include "dna.h"
#include "dna_internal.h"

#include <assert.h>

DNA_LOCAL
dnax_extract_dna4_fn *
dnax_extract_dna4_select(void)
{
	// As ifunc resolvers are called before any constructors run, we explicitly
	// have to initialize the cpu model detector.
	// https://gcc.gnu.org/onlinedocs/gcc/x86-Built-in-Functions.html
	__builtin_cpu_init();

	if (__builtin_cpu_supports("sse4.2")) {
		return dnax_extract_dna4_sse42;
	} else {
		return dnax_extract_dna4_generic;
	}
}

RESOLVER(
	char *,
	dnax_extract_dna4,
	const char *,
	begin,
	const char *,
	end,
	char *,
	dest)
