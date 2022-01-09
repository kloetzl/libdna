#include "Tcommon.h"
#include "dna_internal.h"

#include <assert.h>
#include <catch2/catch.hpp>
#include <dna.h>
#include <iostream>
#include <string.h>
#include <string>

using namespace std::string_literals;

TEST_CASE("Just some simple checks")
{
	auto upper = std::string("ACGT");
	auto lower = std::string("acgt");
	auto rna = std::string("ACGU");

	auto spaced = std::string("A C-G       T");

	REQUIRE(dnax::extract_dna4(upper) == "ACGT");
	REQUIRE(dnax::extract_dna4(lower) == "ACGT");
	REQUIRE(dnax::extract_dna4(rna) == "ACGT");
	REQUIRE(dnax::extract_dna4(spaced) == "ACGT");
}

TEST_CASE("Long checks")
{
	auto upper = repeat("ACGT", 5);
	auto lower = repeat("acgt", 5);
	auto rna = repeat("ACGU", 5);

	auto spaced = repeat("A C-G       T", 5);
	auto acgt5 = repeat("ACGT", 5);

	REQUIRE(dnax::extract_dna4(upper) == repeat("ACGT", 5));
	REQUIRE(dnax::extract_dna4(lower) == repeat("ACGT", 5));
	REQUIRE(dnax::extract_dna4(rna) == repeat("ACGT", 5));
	REQUIRE(dnax::extract_dna4(spaced) == repeat("ACGT", 5));
}

TEST_CASE("All positions")
{
	auto before = repeat("A", 123);

	for (size_t i = 0; i < 123; i++) {
		REQUIRE(dnax::extract_dna4(before) == repeat("A", 123 - i));
		before[i] = 'X';
	}

	REQUIRE(dnax::extract_dna4(before) == "");
}

TEST_CASE("All chars")
{
	std::string all = std::string(256, 0);
	std::iota(all.begin(), all.end(), 0);

	auto shortened = dnax::extract_dna4(all);

	REQUIRE(shortened == "ACGTTACGTT");
}
