#include "Tcommon.h"

#include <assert.h>
#include <catch2/catch.hpp>
#include <dna.h>
#include <iostream>
#include <string.h>
#include <string>

using namespace std::string_literals;

TEST_CASE("Basic packing (2bits) checks")
{
	auto foo = dna4::pack_2bits(4, "ACGT");
	auto bar = dna4::unpack_2bits(4, foo);
	REQUIRE(bar == "ACGT");

	REQUIRE(dna4::pack_2bits(1, "A") == 0);
	REQUIRE(dna4::pack_2bits(1, "C") == 1);
	REQUIRE(dna4::pack_2bits(1, "G") == 2);
	REQUIRE(dna4::pack_2bits(1, "T") == 3);
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
