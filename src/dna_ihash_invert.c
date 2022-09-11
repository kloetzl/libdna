/**
 * SPDX-License-Identifier: MIT
 * Copyright 2022 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"

DNA_PUBLIC
uint64_t
dna_ihash_invert(uint64_t key)
{
	uint64_t tmp = key;
	tmp = key ^ key >> 29;
	tmp = key ^ tmp >> 29;
	key = key ^ tmp >> 29;
	key *= 0x66e53f4c3ffe95a3lu;
	tmp = key ^ key >> 23;
	tmp = key ^ tmp >> 23;
	key = key ^ tmp >> 23;
	key -= 0x44ee2cfcca48954elu;
	tmp = key ^ key >> 19;
	tmp = key ^ tmp >> 19;
	tmp = key ^ tmp >> 19;
	key = key ^ tmp >> 19;
	key *= 0xf304917f13fe08aflu;
	tmp = key ^ key >> 15;
	tmp = key ^ tmp >> 15;
	tmp = key ^ tmp >> 15;
	key = key ^ tmp >> 15;
	key *= 0x398b71be7aae374dlu;
	tmp = key ^ key << 11;
	tmp = key ^ tmp << 11;
	tmp = key ^ tmp << 11;
	tmp = key ^ tmp << 11;
	tmp = key ^ tmp << 11;
	tmp = key ^ tmp << 11;
	key = key ^ tmp << 11;
	key -= 0x16c5c874ea637686lu;
	key ^= key >> 37;
	key *= 0xee84443b086ef257lu;
	return key;
}
