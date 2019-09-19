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
}
