#pragma once

/**
 * SPDX-License-Identifier: MIT
 * Copyright 2019 - 2023 © Fabian Klötzl
 */

#include "config.h"

#include <inttypes.h>
#include <stddef.h>

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

// Declarations for fill_random
extern const uint32_t NOISE1;
extern const uint32_t NOISE2;
extern const uint32_t NOISE3;
extern const uint32_t NOISE4;

extern uint32_t
squirrel3(uint32_t n, uint32_t seed);

// CPU specific mismatch count
typedef size_t(dnax_count_mismatches_fn)(
	const char *begin, const char *end, const char *other);

dnax_count_mismatches_fn dnax_count_mismatches_avx2;
dnax_count_mismatches_fn dnax_count_mismatches_avx512;
dnax_count_mismatches_fn dnax_count_mismatches_generic;
dnax_count_mismatches_fn dnax_count_mismatches_sse2;

// CPU specific rc mismatch count
typedef size_t(dna4_count_mismatches_rc_fn)(
	const char *begin, const char *end, const char *other);

dna4_count_mismatches_rc_fn dna4_count_mismatches_rc_avx2;
dna4_count_mismatches_rc_fn dna4_count_mismatches_rc_generic;
dna4_count_mismatches_rc_fn dna4_count_mismatches_rc_sse42;

// CPU specific revcomp
typedef char *(dna4_revcomp_fn)(const char *begin, const char *end, char *dest);

dna4_revcomp_fn dna4_revcomp_avx2;
dna4_revcomp_fn dna4_revcomp_generic;
dna4_revcomp_fn dna4_revcomp_sse42;

// CPU specific extract
typedef char *(dnax_extract_dna4_fn)(const char *begin,
									 const char *end,
									 char *dest);

dnax_extract_dna4_fn dnax_extract_dna4_generic;
dnax_extract_dna4_fn dnax_extract_dna4_sse42;

// CPU specific fill_random
typedef void(dna4_fill_random_fn)(char *begin, char *end, uint32_t seed);
dna4_fill_random_fn dna4_fill_random_avx2;
dna4_fill_random_fn dna4_fill_random_sse42;
dna4_fill_random_fn dna4_fill_random_generic;

// Macros that help with defining a runtime resolver for function multi
// versioning.

#define STRINGIFY(s) #s
#define GET_1(_1, ...) _1
#define GET_2(_1, _2, ...) _2
#define GET_3(_1, _2, _3, ...) _3
#define GET_4(_1, _2, _3, _4, ...) _4
#define GET_5(_1, _2, _3, _4, _5, ...) _5
#define GET_6(_1, _2, _3, _4, _5, _6) _6

#define ARG_1_TYPE(...) GET_1(__VA_ARGS__)
#define ARG_1_NAME(...) GET_2(__VA_ARGS__)
#define ARG_2_TYPE(...) GET_3(__VA_ARGS__)
#define ARG_2_NAME(...) GET_4(__VA_ARGS__)
#define ARG_3_TYPE(...) GET_5(__VA_ARGS__)
#define ARG_3_NAME(...) GET_6(__VA_ARGS__)

#define ARG_1(...) ARG_1_TYPE(__VA_ARGS__) ARG_1_NAME(__VA_ARGS__)
#define ARG_2(...) ARG_2_TYPE(__VA_ARGS__) ARG_2_NAME(__VA_ARGS__)
#define ARG_3(...) ARG_3_TYPE(__VA_ARGS__) ARG_3_NAME(__VA_ARGS__)

#if CAN_IFUNC && __has_attribute(ifunc)

// On Linux there is the special "ifunc" syntax we can use to pick the optimal
// implementation at runtime.

#define RESOLVER(RETTYPE, NAME, ...)                                           \
	DNA_PUBLIC                                                                 \
	void NAME(ARG_1(__VA_ARGS__), ARG_2(__VA_ARGS__), ARG_3(__VA_ARGS__))      \
		__attribute__((ifunc(STRINGIFY(NAME) "_select")));

#define RESOLVER_VOID(RETTYPE, NAME, ...)                                      \
	RESOLVER(RETTYPE, NAME, __VA_ARGS__)

#else

// If ifunc is unavailable (for instance on macOS or hurd) we have to implement
// the functionality ourselves. To this end we define a function pointer that
// points to the optimal implementation. Said pointer is initiallised to a
// "XXX_callonce" function which, when called, picks the best implementation,
// sets the pointer, and finally forwards the arguments to the actual function.
// This overhead on the first function call can be avoided if the system
// supports __attribute__((constructor)). Then the optimal implementation will
// be picked when the program is first loaded.

#define RESOLVER_META(STATEMENT, RETTYPE, NAME, ...)                           \
                                                                               \
	RETTYPE NAME##_callonce(                                                   \
		ARG_1(__VA_ARGS__), ARG_2(__VA_ARGS__), ARG_3(__VA_ARGS__));           \
                                                                               \
	static NAME##_fn *NAME##_fnptr = NAME##_callonce;                          \
                                                                               \
	DNA_LOCAL                                                                  \
	RETTYPE NAME##_callonce(                                                   \
		ARG_1(__VA_ARGS__), ARG_2(__VA_ARGS__), ARG_3(__VA_ARGS__))            \
	{                                                                          \
		NAME##_fnptr = NAME##_select();                                        \
		STATEMENT NAME##_fnptr(                                                \
			ARG_1_NAME(__VA_ARGS__), ARG_2_NAME(__VA_ARGS__),                  \
			ARG_3_NAME(__VA_ARGS__));                                          \
	}                                                                          \
                                                                               \
	DNA_PUBLIC                                                                 \
	RETTYPE NAME(ARG_1(__VA_ARGS__), ARG_2(__VA_ARGS__), ARG_3(__VA_ARGS__))   \
	{                                                                          \
		STATEMENT NAME##_fnptr(                                                \
			ARG_1_NAME(__VA_ARGS__), ARG_2_NAME(__VA_ARGS__),                  \
			ARG_3_NAME(__VA_ARGS__));                                          \
	}                                                                          \
                                                                               \
	DNA_LOCAL                                                                  \
	DNA_CONSTRUCTOR                                                            \
	void NAME##_init(void)                                                     \
	{                                                                          \
		NAME##_fnptr = NAME##_select();                                        \
	}

// In C it isn't allowed to have a return statement with a void expression in a
// void function. Thus, we need an extra overload that removes the return
// statement from the above code. In C++ this wouldn't be a problem.

#define RESOLVER_VOID(RETTYPE, NAME, ...)                                      \
	RESOLVER_META((void), RETTYPE, NAME, __VA_ARGS__)
#define RESOLVER(RETTYPE, NAME, ...)                                           \
	RESOLVER_META(return, RETTYPE, NAME, __VA_ARGS__)

#endif

#ifdef __cplusplus
}
#endif
