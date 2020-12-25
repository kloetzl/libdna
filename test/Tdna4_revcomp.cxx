#include "Tcommon.h"
#include "dna_internal.h"

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
	const char str[] = "ACGTACGTACGT";
	char buf[13];
	char *end = dna4_revcomp(str, str + sizeof(str) - 1, buf);
	*end = (unsigned char)'\0';

	REQUIRE(strncmp(str, buf, 12) == 0);

	auto forward = repeat("ACGT", 10);

	auto buffer = new char[forward.size() + 1];
	memset(buffer, 0, forward.size() + 1);

	auto end_ptr = dna4_revcomp(dna::begin(forward), dna::end(forward), buffer);
	REQUIRE(static_cast<size_t>(end_ptr - buffer) == forward.size());
	REQUIRE(forward == buffer);

	delete[] buffer;
}

TEST_CASE("Mono characters")
{
	auto forward = std::string(100, 'A');
	auto reverse = std::string(100, 0);

	dna4_revcomp(dna::begin(forward), dna::end(forward), dna::begin(reverse));
	REQUIRE(std::string(100, 'T') == reverse);

	forward = std::string(100, 'T');
	reverse = std::string(100, 0);

	dna4_revcomp(dna::begin(forward), dna::end(forward), dna::begin(reverse));
	REQUIRE(std::string(100, 'A') == reverse);

	forward = std::string(100, 'C');
	reverse = std::string(100, 0);

	dna4_revcomp(dna::begin(forward), dna::end(forward), dna::begin(reverse));
	REQUIRE(std::string(100, 'G') == reverse);

	forward = std::string(100, 'G');
	reverse = std::string(100, 0);

	dna4_revcomp(dna::begin(forward), dna::end(forward), dna::begin(reverse));
	REQUIRE(std::string(100, 'C') == reverse);
}

namespace dna4
{
using function_type = char *(const char *, const char *, char *);

template <function_type fn>
std::string
rc(const std::string &forward)
{
	auto reverse = std::string(forward.size(), 0);
	auto end = fn(dna::begin(forward), dna::end(forward), dna::begin(reverse));
	reverse.erase(end - dna::begin(reverse));
	return reverse;
}

#ifdef EXPOSE_INTERNALS

const auto revcomp_generic = rc<dna4_revcomp_generic>;
const auto revcomp_sse42 = rc<dna4_revcomp_sse42>;
const auto revcomp_avx2 = rc<dna4_revcomp_avx2>;

#endif

} // namespace dna4

using namespace std::string_literals;

TEST_CASE("Edge cases")
{
	REQUIRE(dna4::revcomp("") == ""s);
	REQUIRE(dna4::revcomp("T") == "A"s);
	REQUIRE(dna4::revcomp("G") == "C"s);
	REQUIRE(dna4::revcomp("C") == "G"s);
	REQUIRE(dna4::revcomp("A") == "T"s);
	// lower case and other characters are undefined.

	auto str = "TACGATCGATCGAAAGCTAGTTCGCCCCGAGATA"s;
	auto rcstr = dna4::revcomp(str);
	REQUIRE(rcstr == "TATCTCGGGGCGAACTAGCTTTCGATCGATCGTA");
	REQUIRE(dna4::revcomp(rcstr) == str);
}

TEST_CASE("Different lengths")
{
	for (int k = 0; k < 100; k++) {
		REQUIRE(dna4::revcomp(repeat("A", k)) == repeat("T", k));
		REQUIRE(dna4::revcomp(repeat("C", k)) == repeat("G", k));
		REQUIRE(dna4::revcomp(repeat("G", k)) == repeat("C", k));
		REQUIRE(dna4::revcomp(repeat("T", k)) == repeat("A", k));
		REQUIRE(dna4::revcomp(repeat("ACGT", k)) == repeat("ACGT", k));
	}
}

#ifdef EXPOSE_INTERNALS

TEST_CASE("Various Generic")
{
	auto str = "TACGATCGATCGAAAGCTAGTTCGCCCCGAGATA"s;
	auto rcstr = dna4::revcomp_generic(str);
	REQUIRE(rcstr == "TATCTCGGGGCGAACTAGCTTTCGATCGATCGTA");
	REQUIRE(dna4::revcomp_generic(rcstr) == str);

	for (int k = 0; k < 100; k++) {
		REQUIRE(dna4::revcomp_generic(repeat("A", k)) == repeat("T", k));
		REQUIRE(dna4::revcomp_generic(repeat("C", k)) == repeat("G", k));
		REQUIRE(dna4::revcomp_generic(repeat("G", k)) == repeat("C", k));
		REQUIRE(dna4::revcomp_generic(repeat("T", k)) == repeat("A", k));
		REQUIRE(dna4::revcomp_generic(repeat("ACGT", k)) == repeat("ACGT", k));
	}
}

#ifdef __SSE4_2__
TEST_CASE("Various SSE42")
{
	auto str = "TACGATCGATCGAAAGCTAGTTCGCCCCGAGATA"s;
	auto rcstr = dna4::revcomp_sse42(str);
	REQUIRE(rcstr == "TATCTCGGGGCGAACTAGCTTTCGATCGATCGTA");
	REQUIRE(dna4::revcomp_sse42(rcstr) == str);

	for (int k = 0; k < 100; k++) {
		REQUIRE(dna4::revcomp_sse42(repeat("A", k)) == repeat("T", k));
		REQUIRE(dna4::revcomp_sse42(repeat("C", k)) == repeat("G", k));
		REQUIRE(dna4::revcomp_sse42(repeat("G", k)) == repeat("C", k));
		REQUIRE(dna4::revcomp_sse42(repeat("T", k)) == repeat("A", k));
		REQUIRE(dna4::revcomp_sse42(repeat("ACGT", k)) == repeat("ACGT", k));
	}
}
#endif

#ifdef __AVX2__
TEST_CASE("Various AVX2")
{
	auto str = "TACGATCGATCGAAAGCTAGTTCGCCCCGAGATA"s;
	auto rcstr = dna4::revcomp_avx2(str);
	REQUIRE(rcstr == "TATCTCGGGGCGAACTAGCTTTCGATCGATCGTA");
	REQUIRE(dna4::revcomp_avx2(rcstr) == str);

	for (int k = 0; k < 100; k++) {
		REQUIRE(dna4::revcomp_avx2(repeat("A", k)) == repeat("T", k));
		REQUIRE(dna4::revcomp_avx2(repeat("C", k)) == repeat("G", k));
		REQUIRE(dna4::revcomp_avx2(repeat("G", k)) == repeat("C", k));
		REQUIRE(dna4::revcomp_avx2(repeat("T", k)) == repeat("A", k));
		REQUIRE(dna4::revcomp_avx2(repeat("ACGT", k)) == repeat("ACGT", k));
	}
}
#endif

TEST_CASE("Dispatch")
{
	// The test should be compiled and executed with -march=native.
#ifdef __AVX2__
	REQUIRE(&dna4_revcomp == &dna4_revcomp_avx2);
#else
#ifdef __SSE4_2__
	REQUIRE(&dna4_revcomp == &dna4_revcomp_sse42);
#else
	REQUIRE(&dna4_revcomp == &dna4_revcomp_generic);
#endif
#endif
}

#endif
