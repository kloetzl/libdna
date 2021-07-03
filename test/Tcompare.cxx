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
