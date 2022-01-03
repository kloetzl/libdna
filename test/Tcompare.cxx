#include "Tcommon.h"
#include "dna_internal.h"

#include <assert.h>
#include <catch2/catch.hpp>
#include <dna.h>
#include <string.h>
#include <string>

namespace dna4
{
using function_type = size_t(const char *, const char *, const char *);

template <function_type fn>
size_t
cm(std::string_view s1, std::string_view s2)
{
	return fn(dna::begin(s1), dna::end(s1), dna::begin(s2));
}

#ifdef EXPOSE_INTERNALS

const auto count_mismatches_generic = cm<dna4_count_mismatches_generic>;
const auto count_mismatches_sse2 = cm<dna4_count_mismatches_sse2>;
const auto count_mismatches_avx2 = cm<dna4_count_mismatches_avx2>;
const auto count_mismatches_avx512 = cm<dna4_count_mismatches_avx512>;

#endif

} // namespace dna4

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

TEST_CASE("Various Generic")
{
	const auto subject = "AACGTACGT"s;
	const auto query = "AACGTACCT"s;

	const auto multipliers = {1, 2, 3, 4, 10, 20, 100};
	for (size_t multiplier : multipliers) {
		const auto longsubject = repeat(subject, multiplier);
		const auto longquery = repeat(query, multiplier);
		REQUIRE(
			dna4::count_mismatches_generic(longsubject, longquery) ==
			multiplier);
	}
}

#ifdef __SSE4_2__
TEST_CASE("Various SSE42")
{
	const auto subject = "AACGTACGT"s;
	const auto query = "AACGTACCT"s;

	const auto multipliers = {1, 2, 3, 4, 10, 20, 100};
	for (size_t multiplier : multipliers) {
		const auto longsubject = repeat(subject, multiplier);
		const auto longquery = repeat(query, multiplier);
		REQUIRE(
			dna4::count_mismatches_sse2(longsubject, longquery) == multiplier);
	}
}
#endif

#ifdef __AVX2__
TEST_CASE("Various AVX2")
{
	const auto subject = "AACGTACGT"s;
	const auto query = "AACGTACCT"s;

	const auto multipliers = {1, 2, 3, 4, 10, 20, 100};
	for (size_t multiplier : multipliers) {
		const auto longsubject = repeat(subject, multiplier);
		const auto longquery = repeat(query, multiplier);
		REQUIRE(
			dna4::count_mismatches_avx2(longsubject, longquery) == multiplier);
	}
}
#endif

TEST_CASE("Dispatch")
{
	// The test should be compiled and executed with -march=native.
#ifdef __AVX2__
	REQUIRE(&dna4_count_mismatches == &dna4_count_mismatches_avx2);
#else
#ifdef __SSE4_2__
	REQUIRE(&dna4_count_mismatches == &dna4_count_mismatches_sse2);
#else
	REQUIRE(&dna4_count_mismatches == &dna4_count_mismatches_generic);
#endif
#endif
}

#endif