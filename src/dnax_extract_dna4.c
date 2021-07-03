/**
 * SPDX-License-Identifier: MIT
 * Copyright 2020 - 2021 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"
#include "utils.h"

#include <assert.h>

DNA_LOCAL
char *
dnax_extract_dna4_generic(const char *begin, const char *end, char *dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	return dnax_replace(dnax_to_dna4_table, begin, end, dest);
}

#if !defined(__x86_64)
DNA_PUBLIC
char *
dnax_extract_dna4(const char *begin, const char *end, char *dest)
{
	return dnax_extract_dna4_generic(begin, end, dest);
}
#endif
