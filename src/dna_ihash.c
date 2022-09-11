/**
 * SPDX-License-Identifier: MIT
 * Copyright 2022 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"

DNA_PUBLIC
uint64_t
dna_ihash(uint64_t key)
{
	key *= 0x7b215eedec9e1967llu;
	key ^= key >> 37;
	key += 0x16c5c874ea637686llu;
	key ^= key << 11;
	key *= 0xff7dbf225491d985llu;
	key ^= key >> 15;
	key *= 0x3d5fe41de2ea4e4fllu;
	key ^= key >> 19;
	key += 0x44ee2cfcca48954ellu;
	key ^= key >> 23;
	key *= 0x4f7659e92097460bllu;
	key ^= key >> 29;
	return key;
}
