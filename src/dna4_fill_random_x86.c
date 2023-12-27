/**
 * SPDX-License-Identifier: MIT
 * Copyright 2022 (C) Fabian Klötzl
 */

#include "config.h"
#include "dna.h"
#include "dna_internal.h"

#include <assert.h>

DNA_LOCAL
dna4_fill_random_fn *
dna4_fill_random_select(void)
{
	// As ifunc resolvers are called before any constructors run, we explicitly
	// have to initialize the cpu model detector.
	// https://gcc.gnu.org/onlinedocs/gcc/x86-Built-in-Functions.html
	__builtin_cpu_init();

	if (__builtin_cpu_supports("avx2")) {
		return dna4_fill_random_avx2;
	} else if (__builtin_cpu_supports("sse4.2")) {
		return dna4_fill_random_sse42;
	} else {
		return dna4_fill_random_generic;
	}
}

#if CAN_IFUNC && __has_attribute(ifunc)

DNA_PUBLIC
void
dna4_fill_random(char *dest, char *end, uint32_t seed)
	__attribute__((ifunc("dna4_fill_random_select")));

#elif defined(__APPLE__) && 0

void *
dna4_fill_random_macho(void) __asm__("_dna4_fill_random");

DNA_PUBLIC
void *
dna4_fill_random_macho(void)
{
	__asm__(".symbol_resolver _dna4_fill_random");
	return (void *)dna4_fill_random_select();
}

#else

// If ifunc is unavailable (for instance on macOS or hurd) we have to implement
// the functionality ourselves. Using a function pointer is faster than a
// boolean variable.

void
dna4_fill_random_callonce(char *dest, char *end, uint32_t seed);

static dna4_fill_random_fn *dna4_fill_random_fnptr = dna4_fill_random_callonce;

DNA_LOCAL
void
dna4_fill_random_callonce(char *dest, char *end, uint32_t seed)
{
	dna4_fill_random_fnptr = dna4_fill_random_select();
	dna4_fill_random_fnptr(dest, end, seed);
}

DNA_PUBLIC
void
dna4_fill_random(char *dest, char *end, uint32_t seed)
{
	dna4_fill_random_fnptr(dest, end, seed);
}

DNA_LOCAL
DNA_CONSTRUCTOR
void
dna4_fill_random_init(void)
{
	dna4_fill_random_fnptr = dna4_fill_random_select();
}

#endif
