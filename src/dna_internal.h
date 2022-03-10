#pragma once

/**
 * SPDX-License-Identifier: MIT
 * Copyright 2019 - 2022 © Fabian Klötzl
 */

#include "config.h"

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

#if CAN_CONSTRUCTOR && __has_attribute(constructor)
#define DNA_CONSTRUCTOR __attribute__((constructor))
#else
#define DNA_CONSTRUCTOR
#endif

#ifdef __cplusplus
extern "C" {
#endif

// CPU specific mismatch count
typedef size_t(dna4_count_mismatches_fn)(
	const char *begin, const char *end, const char *other);

dna4_count_mismatches_fn dna4_count_mismatches_avx2;
dna4_count_mismatches_fn dna4_count_mismatches_avx512;
dna4_count_mismatches_fn dna4_count_mismatches_generic;
dna4_count_mismatches_fn dna4_count_mismatches_sse2;

// CPU specific rc mismatch count
typedef size_t(dna4_count_mismatches_rev_fn)(
	const char *begin, const char *end, const char *other);

dna4_count_mismatches_rev_fn dna4_count_mismatches_rev_avx2;
dna4_count_mismatches_rev_fn dna4_count_mismatches_rev_generic;
dna4_count_mismatches_rev_fn dna4_count_mismatches_rev_sse2;

// CPU specific revcomp
typedef char *(dna4_revcomp_fn)(const char *begin, const char *end, char *dest);

dna4_revcomp_fn dna4_revcomp_avx2;
dna4_revcomp_fn dna4_revcomp_generic;
dna4_revcomp_fn dna4_revcomp_sse42;

// CPU specific extract
typedef char *(
	dnax_extract_dna4_fn)(const char *begin, const char *end, char *dest);

dnax_extract_dna4_fn dnax_extract_dna4_generic;
dnax_extract_dna4_fn dnax_extract_dna4_sse42;

typedef void(dna4_fill_random_fn)(char *begin, char *end, uint32_t seed);
dna4_fill_random_fn dna4_fill_random_sse42;
dna4_fill_random_fn dna4_fill_random_generic;

#ifdef __cplusplus
}
#endif
