/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2019 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"

#include <assert.h>

static const char dna4_unpack_2bits_table[4] = {'A', 'C', 'G', 'T'};

DNA_PUBLIC
void
dna4_unpack_2bits(char *begin, size_t k, uint64_t packed)
{
	assert(begin != NULL);

	while (k) {
		begin[k - 1] = dna4_unpack_2bits_table[packed & 0x3];
		packed >>= 2;
		k--;
	}
}
