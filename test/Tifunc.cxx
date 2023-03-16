#include "Tcommon.h"
#include "config.h"
#include "dna_internal.h"

#include <catch2/catch.hpp>

#if EXPOSE_INTERNALS && CAN_IFUNC

// Only on linux with ELF ifuncs does the address of the functions change.
// The test should be compiled and executed with -march=native.

TEST_CASE("dnax_count_mismatches")
{
#ifdef __x86_64

	printf("%p dnax_count_mismatches\n", (void *)&dnax_count_mismatches);
	printf(
		"%p dnax_count_mismatches_avx512\n",
		(void *)&dnax_count_mismatches_avx512);
	printf(
		"%p dnax_count_mismatches_avx2\n", (void *)&dnax_count_mismatches_avx2);
	printf(
		"%p dnax_count_mismatches_sse2\n", (void *)&dnax_count_mismatches_sse2);
	printf(
		"%p dnax_count_mismatches_generic\n",
		(void *)&dnax_count_mismatches_generic);

#if defined(__AVX512BW__) && defined(__AVX512VL__)
	REQUIRE(&dnax_count_mismatches == &dnax_count_mismatches_avx512);
#elif defined(__AVX2__)
	REQUIRE(&dnax_count_mismatches == &dnax_count_mismatches_avx2);
#elif defined(__SSE4_2__)
	REQUIRE(&dnax_count_mismatches == &dnax_count_mismatches_sse2);
#else
	REQUIRE(&dnax_count_mismatches == &dnax_count_mismatches_generic);
#endif
#endif

#ifdef __ARM_NEON
	// there is no separate *_neon function
	REQUIRE(&dnax_count_mismatches != &dnax_count_mismatches_generic);
#endif

#if !defined(__x86_64) && !defined(__ARM_NEON)
	REQUIRE(&dnax_count_mismatches == &dnax_count_mismatches_generic);
#endif
}

TEST_CASE("dna4_revcomp")
{
#ifdef __x86_64

	printf("%p dna4_revcomp\n", (void *)&dna4_revcomp);
	printf("%p dna4_revcomp_avx2\n", (void *)&dna4_revcomp_avx2);
	printf("%p dna4_revcomp_sse42\n", (void *)&dna4_revcomp_sse42);
	printf("%p dna4_revcomp_generic\n", (void *)&dna4_revcomp_generic);

#ifdef __AVX2__
	REQUIRE(&dna4_revcomp == &dna4_revcomp_avx2);
#else
#ifdef __SSE4_2__
	REQUIRE(&dna4_revcomp == &dna4_revcomp_sse42);
#else
	REQUIRE(&dna4_revcomp == &dna4_revcomp_generic);
#endif
#endif
#endif

#ifdef __ARM_NEON
	// there is no separate *_neon function
	REQUIRE(&dna4_revcomp != &dna4_revcomp_generic);
#endif

#if !defined(__x86_64) && !defined(__ARM_NEON)
	REQUIRE(&dna4_revcomp == &dna4_revcomp_generic);
#endif
}

TEST_CASE("dna4_fill_random")
{
#ifdef __x86_64

	printf("%p dna4_fill_random\n", (void *)&dna4_fill_random);
	printf("%p dna4_fill_random_avx2\n", (void *)&dna4_fill_random_avx2);
	printf("%p dna4_fill_random_sse42\n", (void *)&dna4_fill_random_sse42);
	printf("%p dna4_fill_random_generic\n", (void *)&dna4_fill_random_generic);

#if defined(__AVX2__)
	REQUIRE(&dna4_fill_random == &dna4_fill_random_avx2);
#elif defined(__SSE4_2__)
	REQUIRE(&dna4_fill_random == &dna4_fill_random_sse42);
#else
	REQUIRE(&dna4_fill_random == &dna4_fill_random_generic);
#endif
#endif

#if !defined(__x86_64)
	REQUIRE(&dna4_fill_random == &dna4_fill_random_generic);
#endif
}

TEST_CASE("dna4_count_mismatches_rc")
{
#ifdef __x86_64

	printf("%p dna4_count_mismatches_rc\n", (void *)&dna4_count_mismatches_rc);
	printf(
		"%p dna4_count_mismatches_rc_avx2\n",
		(void *)&dna4_count_mismatches_rc_avx2);
	printf(
		"%p dna4_count_mismatches_rc_sse42\n",
		(void *)&dna4_count_mismatches_rc_sse42);
	printf(
		"%p dna4_count_mismatches_rc_generic\n",
		(void *)&dna4_count_mismatches_rc_generic);

#if defined(__AVX2__)
	REQUIRE(&dna4_count_mismatches_rc == &dna4_count_mismatches_rc_avx2);
#elif defined(__SSE4_2__)
	REQUIRE(&dna4_count_mismatches_rc == &dna4_count_mismatches_rc_sse42);
#else
	REQUIRE(&dna4_count_mismatches_rc == &dna4_count_mismatches_rc_generic);
#endif
#endif

#if !defined(__x86_64)
	REQUIRE(&dna4_count_mismatches_rc == &dna4_count_mismatches_rc_generic);
#endif
}

#endif
