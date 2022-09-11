#pragma once

/**
 * SPDX-License-Identifier: MIT
 * Copyright 2019-2022 © Fabian Klötzl
 */

#include <inttypes.h>
#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int
dna_version(void);

extern uint64_t
dna_ihash(uint64_t key);

extern uint64_t
dna_ihash_invert(uint64_t key);

/// dna4 - only ACGT

extern size_t
dna4_count_mismatches(const char *begin, const char *end, const char *other);

extern size_t
dna4_count_mismatches_rc(const char *begin, const char *end, const char *other);

extern void
dna4_fill_random(char *dest, char *end, uint32_t seed);

extern char *
dna4_revcomp(const char *begin, const char *end, char *dest);

extern uint64_t
dna4_pack_2bits(const char *begin, size_t k);

extern void
dna4_unpack_2bits(char *begin, size_t k, uint64_t packed);

/// dnax - Any ASCII char or UTF8 byte
// -1 == skip

extern size_t
dnax_pack_4bits(const char *begin, const char *end, unsigned char *dest);

extern char *
dnax_unpack_4bits(
	const unsigned char *begin, const unsigned char *end, char *dest);

extern char *
dnax_revcomp(const char *table, const char *begin, const char *end, char *dest);

extern char *
dnax_translate(const char *begin, const char *end, char *dest);

extern void
dnax_count(size_t *table, const char *begin, const char *end);

extern char *
dnax_extract_dna4(const char *begin, const char *end, char *dest);

extern char *
dnax_replace(const char *table, const char *begin, const char *end, char *dest);

extern char *
dnax_find_first_not_of(const char *table, const char *begin, const char *end);

extern const char dnax_revcomp_table[];
extern const char dnax_to_dna4_table[];
extern const char dnax_iupac_codes[];

inline char *
dnax_find_first_mismatch(const char *begin, const char *end, const char *other)
{
	size_t length = end - begin;
	size_t i = 0;

	const size_t chunk_size = 8;

	size_t chunked_length = length - (length % chunk_size);
	for (; i < chunked_length; i += chunk_size) {
		int check = memcmp(begin + i, other + i, chunk_size);
		if (check) {
			break;
		}
	}

	for (; i < length; i++) {
		if (begin[i] != other[i]) break;
	}

	return (char *)begin + i;
}

inline char *
dnax_find_first_of(const char *table, const char *begin, const char *end)
{
	const unsigned char *ubegin = (const unsigned char *)begin;

	size_t length = end - begin;
	size_t i = 0;

	for (; i < length; i++) {
		signed char c = table[ubegin[i]];
		if (c != -1) return (char *)begin + i;
	}

	return (char *)end;
}

#ifdef __cplusplus
}
#endif
