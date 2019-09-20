#include "Tcommon.h"
#include "catch.hpp"

#include <cstring>
#include <dna.h>
#include <iostream>
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

TEST_CASE("Example from dna4_count_mismatches manpage")
{
	const char seq1[] = "ACGTACGTACGT";
	const char seq2[] = "ACGTTCGTACGA";
	size_t snps = dna4_count_mismatches(seq1, seq1 + sizeof(seq1) - 1, seq2);

	REQUIRE(snps == 2);
}

TEST_CASE("Example from dna4_revcomp manpage")
{
	const char str[] = "ACGTACGTACGT";
	char buffer[13];
	char *end = dna4_revcomp(str, str + sizeof(str) - 1, buffer);
	*end = '\0';

	assert(strncmp(str, buffer, 12) == 0);
}

TEST_CASE("Example from dnax_revcomp manpage")
{
	const char str[] = "ACGTacgtACGT!";
	char buffer[13];
	char *end =
		dnax_revcomp(dnax_revcomp_table, str, str + sizeof(str) - 1, buffer);
	*end = '\0';

	assert(strncmp(str, buffer, 12) == 0);
}

TEST_CASE("Example from dnax_find_mismatch manpage")
{
	const char str1[] = "ACGGWaA";
	const char str2[] = "ACGGWAAT";

	const char *ptr = dnax_find_mismatch(str1, str1 + 7, str2);
	REQUIRE(ptr - str1 == 5);
	printf("common prefix: %.*s\n", (int)(ptr - str1), str1);
}
