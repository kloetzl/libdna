#pragma once

/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2019 © Fabian Klötzl
 */

#ifdef __cplusplus
extern "C" {
#endif

// CPU specific mismatch count
typedef size_t(dna4_count_mismatches_fn)(
	const char *begin, const char *end, const char *other);

size_t
dna4_count_mismatches_generic(
	const char *begin, const char *end, const char *other);

size_t
dna4_count_mismatches_sse2(
	const char *begin, const char *end, const char *other);

size_t
dna4_count_mismatches_avx2(
	const char *begin, const char *end, const char *other);

size_t
dna4_count_mismatches_avx512(
	const char *begin, const char *end, const char *other);

// CPU specific revcomp
typedef char *(dna4_revcomp_fn)(const char *begin, const char *end, char *dest);

char *
dna4_revcomp_generic(const char *begin, const char *end, char *dest);
char *
dna4_revcomp_sse42(const char *begin, const char *end, char *dest);
char *
dna4_revcomp_avx2(const char *begin, const char *end, char *dest);

#ifdef __cplusplus
}
#endif
