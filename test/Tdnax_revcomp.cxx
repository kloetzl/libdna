#include "catch.hpp"

#include <assert.h>
#include <dna.h>
#include <string.h>
#include <string>

auto
begin_data(std::string &str)
{
	return str.data();
}

auto
end_data(std::string &str)
{
	return str.data() + str.size();
}

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

	auto end_ptr = dna4_revcomp(begin_data(forward), end_data(forward), buffer);
	REQUIRE(end_ptr - buffer == forward.size());
	REQUIRE(forward == buffer);

	end_ptr = dnax_revcomp(
		dnax_revcomp_table, begin_data(forward), end_data(forward), buffer);

	REQUIRE(end_ptr - buffer == forward.size());
	REQUIRE(forward == buffer);
}
