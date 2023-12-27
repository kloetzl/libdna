/**
 * SPDX-License-Identifier: MIT
 * Copyright 2018 - 2023 (C) Fabian Klötzl
 */

#include "config.h"
#include "dna.h"
#include "dna_internal.h"

#include <assert.h>

DNA_LOCAL
dnax_count_mismatches_fn *
dnax_count_mismatches_select(void)
{
	// As ifunc resolvers are called before any constructors run, we explicitly
	// have to initialize the cpu model detector.
	// https://gcc.gnu.org/onlinedocs/gcc/x86-Built-in-Functions.html
	__builtin_cpu_init();

	if (__builtin_cpu_supports("avx512bw") &&
		__builtin_cpu_supports("avx512vl")) {
		return dnax_count_mismatches_avx512;
	} else if (__builtin_cpu_supports("avx2")) {
		return dnax_count_mismatches_avx2;
	} else if (__builtin_cpu_supports("sse2")) {
		return dnax_count_mismatches_sse2;
	} else {
		return dnax_count_mismatches_generic;
	}
}

#if CAN_IFUNC && __has_attribute(ifunc)

DNA_PUBLIC
size_t
dnax_count_mismatches(const char *begin, const char *end, const char *other)
	__attribute__((ifunc("dnax_count_mismatches_select")));

#elif defined(__APPLE__) && 0

void *
dnax_count_mismatches_macho(void) __asm__("_dnax_count_mismatches");

DNA_PUBLIC
void *
dnax_count_mismatches_macho(void)
{
	__asm__(".symbol_resolver _dnax_count_mismatches");
	return (void *)dnax_count_mismatches_select();
}

#else

// If ifunc is unavailable (for instance on macOS or hurd) we have to implement
// the functionality ourselves. Using a function pointer is faster than a
// boolean variable.

size_t
dnax_count_mismatches_callonce(
	const char *begin, const char *end, const char *other);

static dnax_count_mismatches_fn *dnax_count_mismatches_fnptr =
	dnax_count_mismatches_callonce;

DNA_LOCAL
size_t
dnax_count_mismatches_callonce(
	const char *begin, const char *end, const char *other)
{
	dnax_count_mismatches_fnptr = dnax_count_mismatches_select();
	return dnax_count_mismatches_fnptr(begin, end, other);
}

DNA_PUBLIC
size_t
dnax_count_mismatches(const char *begin, const char *end, const char *other)
{
	return dnax_count_mismatches_fnptr(begin, end, other);
}

DNA_LOCAL
DNA_CONSTRUCTOR
void
dnax_count_mismatches_init(void)
{
	dnax_count_mismatches_fnptr = dnax_count_mismatches_select();
}

#endif
