#include "Tcommon.h"

#include <catch2/catch.hpp>
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

TEST_CASE("Example from dnax_find_first_mismatch manpage")
{
	const char str1[] = "ACGGWaA";
	const char str2[] = "ACGGWAAT";

	const char *ptr = dnax_find_first_mismatch(str1, str1 + 7, str2);
	REQUIRE(ptr - str1 == 5);
	printf("common prefix: %.*s\n", (int)(ptr - str1), str1);
}

double
gc_content(const char *begin, const char *end)
{
	size_t table[256] = {0};
	dnax_count(table, begin, end);
	double gc = table['G'] + table['C'] + table['g'] + table['c'];
	return gc / (end - begin);
}

TEST_CASE("Example from dnax_count manpage")
{
	size_t table[256] = {0};
	const char str[] = "AACGTACCT";

	dnax_count(table, str, str + 9);
	REQUIRE(table['A'] == 3);
	REQUIRE(table['C'] == 3);
	REQUIRE(table['G'] == 1);
	REQUIRE(table['T'] == 2);

	REQUIRE(gc_content(str, str + 9) == 4.0 / 9);
}

TEST_CASE("Example from dnax_replace manpage")
{
	char in[] = "AaCGT";
	char out[6] = {0};

	char table[256] = {};
	memset(table, -1, 256);
	table['A'] = 'a';
	table['C'] = 'c';
	table['G'] = 'g';
	table['T'] = 't';

	char *end = dnax_replace(table, in, in + 5, out);
	*end = '\0';

	REQUIRE(std::string(out) == "acgt");
}

TEST_CASE("Example from dnax_extract_dna4 manpage")
{
	char in[] = "AaC!GT";
	char out[6] = {0};

	char *end = dnax_extract_dna4(in, in + 6, out);
	*end = '\0';

	REQUIRE(std::string(out) == "AACGT");
}

TEST_CASE("Example from dnax_translate manpage")
{
	char in[] = "AUG!CTN";
	char out[3] = {0};

	char *end = dnax_translate(in, in + 7, out);
	*end = '\0';

	REQUIRE(std::string(out) == "ML");
}

TEST_CASE("Example from dnax_find_first_of manpage")
{
	const char str[] = "ACGT-AA";
	char table[256] = {0};
	std::memset(table, -1, 256);
	table['-'] = 2;

	const char *gap = dnax_find_first_of(table, str, str + 7);
	REQUIRE(gap - str == 4);
}

TEST_CASE("Example from dnax_find_first_not_of manpage")
{
	const char str[] = "ACGT-AA";
	char table[256] = {0};
	std::memset(table, 0, 256);
	table['-'] = -1;

	const char *gap = dnax_find_first_not_of(table, str, str + 7);
	REQUIRE(gap - str == 4);
}
