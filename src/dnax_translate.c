/**
 * SPDX-License-Identifier: MIT
 * Copyright 2018 - 2024 (C) Fabian Klötzl
 */

#include "dna.h"
#include "dna_internal.h"

#include <assert.h>
#include <string.h>

DNA_LOCAL
const char table4096[] = {
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0FLXFFXXLXLXXXXX\0CWXCCXX*"
	"XXXXXXX\0XXXXXXXXXXXXXXX\0SSSSSSSSSSSSSSS\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXX"
	"X\0XXXXXXXXXXXXXXX\0Y*XYYXX*X*XXXXX\0XXXXXXXXXXXXXXX\0XXXXXXX*"
	"XXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXX"
	"X\0XXXXXXXXXXXXXXX\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0VVVVVVVVVVVVVVV\0GGGGG"
	"GGGGGGGGGG\0XXXXXXXXXXXXXXX\0AAAAAAAAAAAAAAA\0XXXXXXXXXXXXXXX\0XXXXXXXXXXX"
	"XXXX\0XXXXXXXXXXXXXXX\0DEXDDXXEXEXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0"
	"XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXX"
	"XXXXXXXXX\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXX"
	"X\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXX"
	"XXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXX"
	"XXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0LLLLLLLLLLLLLLL\0RRRRRRRRRRRRRRR\0XXXXXX"
	"XXXXXXXXX\0PPPPPPPPPPPPPPP\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXX"
	"XXX\0HQXHHXXQXQXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0X"
	"XXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0XLXXXXXLXLXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXX"
	"X\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXX"
	"XXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXX"
	"XXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXX"
	"XXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XZXXXXXZXZX"
	"XXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0"
	"XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0\0\0\0\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXX"
	"XX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XX"
	"XXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXX"
	"XXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	"\0IMXIIXXIIXXIIXX\0SRXSSXXRXRXXXXX\0XXXXXXXXXXXXXXX\0TTTTTTTTTTTTTTT\0XXXX"
	"XXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0NKXNNXXKXKXXXXX\0XXXXXXXXXX"
	"XXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX"
	"\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0XXXXXX"
	"XXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXX"
	"XXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0X"
	"XXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXX"
	"XXXXXXXX\0XXXXXXXXXXXXXXX\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0XXXXXXXXXXXXXXX"
	"\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXX"
	"XXXXXXXXXXX\0XXXXXXXXXXXXXXX\0BXXBBXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXX"
	"XXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX"
	"\0XXXXXXXXXXXXXXX\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0XXXXXXXXXXXXXXX\0XXXXXX"
	"XXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXX"
	"XXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0X"
	"XXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXX"
	"XXXXXXXX\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0XXXXXXXXXXXXXXX\0XRXXXXXRXRXXXXX"
	"\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXX"
	"XXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXX"
	"XXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXX"
	"XXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXX"
	"XXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0X"
	"XXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0\0\0\0\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXX"
	"X\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXX"
	"XXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXX"
	"XXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0\0\0\0\0\0\0\0"
	"\0\0\0\0\0\0\0\0\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXX"
	"XXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXX"
	"XXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0"
	"XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX\0XXXXXXXXXXXXXXX"};

// The following is almost identical to dnax_iupac_codes except for '-'.
DNA_LOCAL
const signed char nuclTo4Bits[] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, 8,  7,  4,  11, -1, -1, 2,  13, -1, -1, 3,
	-1, 12, 15, -1, -1, -1, 10, 6,  1,  1,  14, 9,  -1, 5,  -1, -1, -1, -1, -1,
	-1, -1, 8,  7,  4,  11, -1, -1, 2,  13, -1, -1, 3,  -1, 12, 15, -1, -1, -1,
	10, 6,  1,  1,  14, 9,  -1, 5,  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1};

static int16_t
pack_triplet(const char str[3])
{
	int16_t ret = nuclTo4Bits[(unsigned char)str[0]] << 8 |
				  nuclTo4Bits[(unsigned char)str[1]] << 4 |
				  nuclTo4Bits[(unsigned char)str[2]];

	return ret;
}

DNA_PUBLIC
char *
dnax_translate(const char *begin, const char *end, char *dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	const char *ptr = begin;

	while (ptr + 2 < end) {
		// skip gaps and non-nucleotide codes
		char triplet[4] = {0};
		memcpy(triplet, ptr, 3);

		int16_t prePacked = pack_triplet(triplet);
		int16_t index;

		if (__builtin_expect(prePacked < 0, 0)) {
			// Rare case, the triplet contained a non-nucleotide char.
			const char *ccp = dnax_find_first_of(nuclTo4Bits, ptr, end);
			if (ccp == end) break;

			const char *ddp = dnax_find_first_of(nuclTo4Bits, ccp + 1, end);
			if (ddp == end) break;

			const char *eep = dnax_find_first_of(nuclTo4Bits, ddp + 1, end);
			if (eep == end) break;
			ptr = eep + 1;

			triplet[0] = *ccp;
			triplet[1] = *ddp;
			triplet[2] = *eep;
			index = pack_triplet(triplet);
		} else {
			ptr += 3;
			index = prePacked;
		}

		int code = table4096[index];

		*dest++ = code;
	}

	return dest;
}
