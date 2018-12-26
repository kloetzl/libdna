#pragma once

/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2018 © Fabian Klötzl
 */

#include <inttypes.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DNA_VERSION_MINOR 0
#define DNA_VERSION_MAJOR 0
#define DNA_VERSION ((DNA_VERSION_MAJOR << 16) | DNA_VERSION_MINOR)

int
dna_version(void);

/// dna4 - only ACGT

typedef double(dna4_evodist_jc_fn)(
	const char *begin,
	const char *end,
	const char *other,
	size_t *substitutions);

extern double
dna4_evodist_jc(
	const char *begin,
	const char *end,
	const char *other,
	size_t *substitutions);

double
dna4_evodist_jc_generic(
	const char *begin,
	const char *end,
	const char *other,
	size_t *substitutions);

double
dna4_evodist_jc_sse2(
	const char *begin,
	const char *end,
	const char *other,
	size_t *substitutions);

double
dna4_evodist_jc_avx2(
	const char *begin,
	const char *end,
	const char *other,
	size_t *substitutions);

double
dna4_evodist_jc_avx512(
	const char *begin,
	const char *end,
	const char *other,
	size_t *substitutions);

double
dna4_evodist_k80(
	const char *begin,
	const char *end,
	const char *other,
	size_t *transitions,
	size_t *transversions);
double
dna4_gc_content(const char *begin, const char *end);
uint64_t
dna4_pack(const char *begin, size_t k);
char *
dna4_revcomp(const char *begin, const char *end, char *dest);

/// dnax - Any ASCII char or UTF8 byte
// -1 == skip
uint64_t
dnax_pack(const char *table, const char *begin, const char *end, size_t k);
char *
dnax_revcomp(const char *table, const char *begin, const char *end, char *dest);

char *
dnax_translate_quirks(const char *begin, const char *end, char *dest);
#define dnax_translate dnax_translate_quirks

void
dnax_count(size_t *table, const char *begin, const char *end);
char *
dnax_replace(const char *table, const char *begin, const char *end, char *dest);
char *
dnax_mismatch(const char *begin, const char *end, const char *other);
char *
dnax_find_first_of(const char *table, const char *begin, const char *end);
char *
dnax_find_first_not_of(const char *table, const char *begin, const char *end);

extern const char dnax_revcomp_table[];
extern const char dnax_pack_table[];
extern const char dnax_to_dna4_table[];

#ifdef __cplusplus
}
#endif
