/**
 * SPDX-License-Identifier: MIT
 * Copyright 2020 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"
#include "utils.h"

#include <assert.h>

DNA_PUBLIC
char *
dnax_extract_dna4(const char *begin, const char *end, char *dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	return dnax_replace(dnax_to_dna4_table, begin, end, dest);
}
