#include "Tcommon.h"
#include "catch.hpp"

#include <assert.h>
#include <dna.h>
#include <iostream>
#include <string.h>
#include <string>

namespace dnax
{
auto
pack_4bits(const std::string &str)
{
	auto ret = std::vector<unsigned char>(str.size(), 0);

	auto end = dnax_pack_4bits(dna::begin(str), dna::end(str), ret.data());

	ret.erase(ret.begin() + (end - ret.data()), ret.end());
	return ret;
}

auto
unpack_4bits(const std::vector<unsigned char> &packed)
{
	auto ret = std::string(packed.size() * 2, 0);

	dnax_unpack_4bits(
		packed.data(), packed.data() + packed.size(), dna::begin(ret));

	return ret;
}
} // namespace dnax

using namespace std::string_literals;

TEST_CASE("Basic packing (4bits) checks")
{
	auto foo = dnax::pack_4bits("ACGT");
	REQUIRE(foo.size() == 2);
	auto bar = dnax::unpack_4bits(foo);
	REQUIRE(bar.size() == 4);
	REQUIRE(bar == "ACGT");

	auto all = "ACGTWSMKRYBDHVN-"s;
	auto packed = dnax::pack_4bits(all);
	REQUIRE(dnax::unpack_4bits(packed) == all);

	REQUIRE(dnax::unpack_4bits(dnax::pack_4bits("U")) == "T-");
}
