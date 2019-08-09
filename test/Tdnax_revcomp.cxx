#include "Tcommon.h"
#include "catch.hpp"

#include <assert.h>
#include <dna.h>
#include <string.h>
#include <string>

auto
repeat(std::string in, int count)
{
	auto ret = std::string{};
	ret.reserve(in.size() * count);

	for (int i = 0; i < count; i++) {
		ret += in;
	}

	return ret;
}

TEST_CASE("Basic revcomp checks")
{
	auto forward = repeat("ACGT", 10);

	auto buffer = new char[forward.size() + 1];
	memset(buffer, 0, forward.size() + 1);

	auto end_ptr = dna4_revcomp(dna::begin(forward), dna::end(forward), buffer);
	REQUIRE(end_ptr - buffer == forward.size());
	REQUIRE(forward == buffer);

	end_ptr = dnax_revcomp(
		dnax_revcomp_table, dna::begin(forward), dna::end(forward), buffer);

	REQUIRE(end_ptr - buffer == forward.size());
	REQUIRE(forward == buffer);

	delete[] buffer;
}
