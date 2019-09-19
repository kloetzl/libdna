#pragma once

/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2019 © Fabian Klötzl
 */

#ifdef NDEBUG
#undef EXPOSE_INTERNALS
#else
#define EXPOSE_INTERNALS 1
#endif

#define DNA_PUBLIC __attribute__((visibility("default")))

#ifdef EXPOSE_INTERNALS
#define DNA_LOCAL __attribute__((visibility("default")))
#else
#define DNA_LOCAL __attribute__((visibility("hidden")))
#endif

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
