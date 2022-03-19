/**
 * SPDX-License-Identifier: MIT
 * Copyright 2021 - 2022 (C) Fabian Klötzl
 */

#include "config.h"
#include "dna.h"
#include "dna_internal.h"

#include <assert.h>

DNA_LOCAL
dna4_count_mismatches_rc_fn *
dna4_count_mismatches_rc_select(void)
{
	// As ifunc resolvers are called before any constructors run, we explicitly
	// have to initialize the cpu model detector.
	// https://gcc.gnu.org/onlinedocs/gcc/x86-Built-in-Functions.html
	__builtin_cpu_init();

	if (__builtin_cpu_supports("avx2")) {
		return dna4_count_mismatches_rc_avx2;
	} else if (__builtin_cpu_supports("sse4.2")) {
		return dna4_count_mismatches_rc_sse42;
	} else {
		return dna4_count_mismatches_rc_generic;
	}
}

#if CAN_IFUNC && __has_attribute(ifunc)

DNA_PUBLIC
size_t
dna4_count_mismatches_rc(const char *begin, const char *end, const char *other)
	__attribute__((ifunc("dna4_count_mismatches_rc_select")));

#elif defined(__APPLE__)

void *
dna4_count_mismatches_rc_macho(void) __asm__("_dna4_count_mismatches_rc");

DNA_PUBLIC
void *
dna4_count_mismatches_rc_macho(void)
{
	__asm__(".symbol_resolver _dna4_count_mismatches_rc");
	return (void *)dna4_count_mismatches_rc_select();
}

#else

// If ifunc is unavailable (for instance on macOS or hurd) we have to implement
// the functionality ourselves. Using a function pointer is faster than a
// boolean variable.

size_t
dna4_count_mismatches_rc_callonce(
	const char *begin, const char *end, const char *other);

static dna4_count_mismatches_rc_fn *dna4_count_mismatches_rc_fnptr =
	dna4_count_mismatches_rc_callonce;

DNA_LOCAL
size_t
dna4_count_mismatches_rc_callonce(
	const char *begin, const char *end, const char *other)
{
	dna4_count_mismatches_rc_fnptr = dna4_count_mismatches_rc_select();
	return dna4_count_mismatches_rc_fnptr(begin, end, other);
}

DNA_PUBLIC
size_t
dna4_count_mismatches_rc(const char *begin, const char *end, const char *other)
{
	return dna4_count_mismatches_rc_fnptr(begin, end, other);
}

DNA_LOCAL
DNA_CONSTRUCTOR
void
dna4_count_mismatches_rc_init(void)
{
	dna4_count_mismatches_rc_fnptr = dna4_count_mismatches_rc_select();
}

#endif
