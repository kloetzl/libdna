#include "Tcommon.h"

#include <assert.h>
#include <catch2/catch.hpp>
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
	REQUIRE(static_cast<size_t>(end_ptr - buffer) == forward.size());
	REQUIRE(forward == buffer);

	end_ptr = dnax_revcomp(
		dnax_revcomp_table, dna::begin(forward), dna::end(forward), buffer);

	REQUIRE(static_cast<size_t>(end_ptr - buffer) == forward.size());
	REQUIRE(forward == buffer);

	delete[] buffer;
}

TEST_CASE("Mono characters")
{
	auto forward = std::string(100, 'A');
	auto reverse = std::string(100, 0);

	dnax_revcomp(
		dnax_revcomp_table, dna::begin(forward), dna::end(forward),
		dna::begin(reverse));
	REQUIRE(std::string(100, 'T') == reverse);

	forward = std::string(100, 'T');
	reverse = std::string(100, 0);

	dnax_revcomp(
		dnax_revcomp_table, dna::begin(forward), dna::end(forward),
		dna::begin(reverse));
	REQUIRE(std::string(100, 'A') == reverse);

	forward = std::string(100, 'C');
	reverse = std::string(100, 0);

	dnax_revcomp(
		dnax_revcomp_table, dna::begin(forward), dna::end(forward),
		dna::begin(reverse));
	REQUIRE(std::string(100, 'G') == reverse);

	forward = std::string(100, 'G');
	reverse = std::string(100, 0);

	dnax_revcomp(
		dnax_revcomp_table, dna::begin(forward), dna::end(forward),
		dna::begin(reverse));
	REQUIRE(std::string(100, 'C') == reverse);
}

namespace dnax
{
auto
revcomp(const std::string &forward)
{
	auto reverse = std::string(forward.size(), 0);
	auto end = dnax_revcomp(
		dnax_revcomp_table, dna::begin(forward), dna::end(forward),
		dna::begin(reverse));
	reverse.erase(end - dna::begin(reverse));
	return reverse;
}
} // namespace dnax

using namespace std::string_literals;

TEST_CASE("Edge cases")
{
	REQUIRE(dnax::revcomp("") == ""s);
	REQUIRE(dnax::revcomp("T") == "A"s);
	REQUIRE(dnax::revcomp("G") == "C"s);
	REQUIRE(dnax::revcomp("C") == "G"s);
	REQUIRE(dnax::revcomp("A") == "T"s);
	REQUIRE(dnax::revcomp("t") == "a"s);
	REQUIRE(dnax::revcomp("g") == "c"s);
	REQUIRE(dnax::revcomp("c") == "g"s);
	REQUIRE(dnax::revcomp("a") == "t"s);
	REQUIRE(dnax::revcomp("_") == ""s);
}
