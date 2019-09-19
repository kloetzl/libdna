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

	auto count = dnax_pack_4bits(dna::begin(str), dna::end(str), ret.data());
	auto bytes = (count + 1) >> 1;
	ret.erase(ret.begin() + bytes, ret.end());
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

	REQUIRE(dnax::pack_4bits("A").size() == 1);
}

TEST_CASE("Manpage examples")
{
	const char str[] = "ACGT-W";
	unsigned char buffer[3] = {0};

	size_t nibbles = dnax_pack_4bits(str, str + 6, buffer);
	size_t bytes = (nibbles + 1) >> 1;

	REQUIRE(bytes == 3);

	{
		const char str[] = "ACGT-W";
		unsigned char packed[3] = {0};
		char buffer[7] = {0};

		dnax_pack_4bits(str, str + 6, packed);
		dnax_unpack_4bits(packed, packed + 3, buffer);

		REQUIRE(strcmp(str, buffer) == 0);
	}
}
