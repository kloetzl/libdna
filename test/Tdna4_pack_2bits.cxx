#include "Tcommon.h"
#include "catch.hpp"

#include <assert.h>
#include <dna.h>
#include <iostream>
#include <string.h>
#include <string>

namespace dna4
{
auto
pack_2bits(const std::string &str, size_t k)
{
	return dna4_pack_2bits(dna::begin(str), k);
}

auto
unpack_2bits(uint64_t packed, size_t k)
{
	auto ret = std::string(k, 0);

	dna4_unpack_2bits(dna::begin(ret), k, packed);

	return ret;
}
} // namespace dna4

using namespace std::string_literals;

TEST_CASE("Basic packing (2bits) checks")
{
	auto foo = dna4::pack_2bits("ACGT", 4);
	auto bar = dna4::unpack_2bits(foo, 4);
	REQUIRE(bar == "ACGT");

	REQUIRE(dna4::pack_2bits("A", 1) == 0);
	REQUIRE(dna4::pack_2bits("C", 1) == 1);
	REQUIRE(dna4::pack_2bits("G", 1) == 2);
	REQUIRE(dna4::pack_2bits("T", 1) == 3);
}

TEST_CASE("Manpage examples")
{
	const char str[] = "ACGTACGTTTCC";
	uint64_t packed = dna4_pack_2bits(str, 12);

	REQUIRE(packed == 0x1b1bf5);
	printf("%" PRIu64 "\n", packed);

	packed = 0x1b1bf5;
	char buffer[13] = {0};
	dna4_unpack_2bits(buffer, 12, packed);

	printf("%s\n", buffer);
	REQUIRE(strcmp(buffer, "ACGTACGTTTCC") == 0);
}
