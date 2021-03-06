#pragma once

/**
 * SPDX-License-Identifier: MIT
 * Copyright 2019-2020 © Fabian Klötzl
 */

#include <inttypes.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int
dna_version(void);

/// dna4 - only ACGT

extern size_t
dna4_count_mismatches(const char *begin, const char *end, const char *other);

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
dnax_find_first_mismatch(const char *begin, const char *end, const char *other);

extern char *
dnax_find_first_of(const char *table, const char *begin, const char *end);

extern char *
dnax_find_first_not_of(const char *table, const char *begin, const char *end);

extern const char dnax_revcomp_table[];
extern const char dnax_to_dna4_table[];

#ifdef __cplusplus
}
#endif
