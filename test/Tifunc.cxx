#include "Tcommon.h"
#include "dna_internal.h"

#include <catch2/catch.hpp>

#if EXPOSE_INTERNALS && CAN_IFUNC

// Only on linux with ELF ifuncs does the address of the functions change.
// The test should be compiled and executed with -march=native.

TEST_CASE("dna4_count_mismatches")
{
#ifdef __x86_64
#if defined(__AVX512BW__) && defined(__AVX5125VL)
	REQUIRE(&dna4_count_mismatches == &dna4_count_mismatches_avx512);
#elif defined(__AVX2__)
	REQUIRE(&dna4_count_mismatches == &dna4_count_mismatches_avx2);
#elif defined(__SSE4_2__)
	REQUIRE(&dna4_count_mismatches == &dna4_count_mismatches_sse2);
#else
	REQUIRE(&dna4_count_mismatches == &dna4_count_mismatches_generic);
#endif
#endif

#ifdef __ARM_NEON
	REQUIRE(&dna4_count_mismatches == &dna4_count_mismatches_neon);
#endif

#if !defined(__x86_64) && !defined(__ARM_NEON)
	REQUIRE(&dna4_count_mismatches == &dna4_count_mismatches_generic);
#endif
}

TEST_CASE("dna4_revcomp")
{
#ifdef __x86_64
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
	REQUIRE(&dna4_revcomp == &dna4_revcomp_neon);
#endif

#if !defined(__x86_64) && !defined(__ARM_NEON)
	REQUIRE(&dna4_revcomp == &dna4_revcomp_generic);
#endif
}

#endif
