/**
 * SPDX-License-Identifier: MIT
 * Copyright 2018 - 2022 (C) Fabian Klötzl
 */

#include "config.h"
#include "dna.h"
#include "dna_internal.h"

#include <assert.h>

DNA_LOCAL
dnax_extract_dna4_fn *
dnax_extract_dna4_select(void)
{
	// As ifunc resolvers are called before any constructors run, we explicitly
	// have to initialize the cpu model detector.
	// https://gcc.gnu.org/onlinedocs/gcc/x86-Built-in-Functions.html
	__builtin_cpu_init();

	if (__builtin_cpu_supports("sse4.2")) {
		return dnax_extract_dna4_sse42;
	} else {
		return dnax_extract_dna4_generic;
	}
}

#if CAN_IFUNC && __has_attribute(ifunc)

DNA_PUBLIC
char *
dnax_extract_dna4(const char *begin, const char *end, char *dest)
	__attribute__((ifunc("dnax_extract_dna4_select")));

#elif defined(__APPLE__)

void *
dnax_extract_dna4_macho(void) __asm__("_dnax_extract_dna4");

DNA_PUBLIC
void *
dnax_extract_dna4_macho(void)
{
	__asm__(".symbol_resolver _dnax_extract_dna4");
	return (void *)dnax_extract_dna4_select();
}

#else

// If ifunc is unavailable (for instance on macOS or hurd) we have to implement
// the functionality ourselves. Using a function pointer is faster than a
// boolean variable.

char *
dnax_extract_dna4_callonce(const char *begin, const char *end, char *dest);

static dnax_extract_dna4_fn *dnax_extract_dna4_fnptr =
	dnax_extract_dna4_callonce;

DNA_LOCAL
char *
dnax_extract_dna4_callonce(const char *begin, const char *end, char *dest)
{
	dnax_extract_dna4_fnptr = dnax_extract_dna4_select();
	return dnax_extract_dna4_fnptr(begin, end, dest);
}

DNA_PUBLIC
char *
dnax_extract_dna4(const char *begin, const char *end, char *dest)
{
	return dnax_extract_dna4_fnptr(begin, end, dest);
}

DNA_LOCAL
DNA_CONSTRUCTOR
void
dnax_extract_dna4_init(void)
{
	dnax_extract_dna4_fnptr = dnax_extract_dna4_select();
}

#endif
