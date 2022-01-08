#include "Tcommon.h"
#include "dna_internal.h"

#include <assert.h>
#include <catch2/catch.hpp>
#include <dna.h>
#include <string.h>
#include <string>

using namespace std::string_literals;

TEST_CASE("Some simple checks")
{
	const auto subject = "AACGTACGT"s;
	const auto query = "AACGTACCT"s;

	size_t mismatches = dna4_count_mismatches(
		dna::begin(subject), dna::end(subject), dna::begin(query));
	REQUIRE(mismatches == 1);

	auto rsubject = std::string(subject); // copy for length reasons
	dna4_revcomp(dna::begin(subject), dna::end(subject), dna::begin(rsubject));

	auto rquery = std::string(query); // copy
	dnax_revcomp(
		dnax_revcomp_table, dna::begin(query), dna::end(query),
		dna::begin(rquery));

	size_t rcmismatches = dna4_count_mismatches(
		dna::begin(rsubject), dna::end(rsubject), dna::begin(rquery));
	REQUIRE(rcmismatches == 1);
}

TEST_CASE("Long strings")
{
	const auto subject = "AACGTACGT"s;
	const auto query = "AACGTACCT"s;

	const int multiplier = 100;
	const auto longsubject = repeat(subject, multiplier);
	const auto longquery = repeat(query, multiplier);

	size_t mismatches = dna4::count_mismatches(longsubject, longquery);
	REQUIRE(mismatches == (1 * multiplier));

	mismatches = dna4_count_mismatches(
		dna::begin(longsubject), dna::end(longsubject), dna::begin(longquery));
	REQUIRE(mismatches == (1 * multiplier));
}

#ifdef EXPOSE_INTERNALS
// The following tests should only be executed when host and target machine are
// the same. Ie. not cross-compilation. Also it expects --march=native if
// available.

using function_type = size_t(const char *, const char *, const char *);

template <function_type fn> struct functor {
	size_t operator()(std::string_view s1, std::string_view s2) const
	{
		return fn(dna::begin(s1), dna::end(s1), dna::begin(s2));
	}
};

using count_mismatches_functor_generic = functor<dna4_count_mismatches_generic>;

#ifdef __x86_64
using count_mismatches_functor_sse2 = functor<dna4_count_mismatches_sse2>;
using count_mismatches_functor_avx2 = functor<dna4_count_mismatches_avx2>;
using count_mismatches_functor_avx512 = functor<dna4_count_mismatches_avx512>;
#endif

#ifdef __ARM_NEON
using count_mismatches_functor_neon = functor<dna4_count_mismatches_neon>;
#endif

using MyTypes = std::tuple<
#if defined(__AVX512BW__) && defined(__AVX5125VL__)
	count_mismatches_functor_avx512,
#endif
#ifdef __AVX2__
	count_mismatches_functor_avx2,
#endif
#ifdef __SSE4_2__
	count_mismatches_functor_sse2,
#endif
#ifdef __ARM_NEON
	count_mismatches_functor_neon,
#endif
	count_mismatches_functor_generic>;

TEMPLATE_LIST_TEST_CASE(
	"Check ISA specific implementations", "[template][list]", MyTypes)
{
	auto count_mismatches = TestType();

	const auto subject = "AACGTACGT"s;
	const auto query = "AACGTACCT"s;

	const auto multipliers = {1, 2, 3, 4, 10, 20, 100};
	for (size_t multiplier : multipliers) {
		const auto longsubject = repeat(subject, multiplier);
		const auto longquery = repeat(query, multiplier);
		REQUIRE(count_mismatches(longsubject, longquery) == multiplier);
	}
}

#endif
