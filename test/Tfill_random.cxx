#include "Tcommon.h"

#include <catch2/catch.hpp>
#include <cstring>
#include <dna.h>
#include <iostream>
#include <string>
#include <unordered_map>

namespace std
{
// Technically, this is UB because char is not a user-defined type.
template <typename T, typename U> struct hash<std::pair<T, U>> {
	auto operator()(const std::pair<T, U> &p) const noexcept
	{
		const auto [t, u] = p;
		return static_cast<size_t>(t) << 8 + u;
	}
};
} // namespace std

static const auto two_mers = std::vector<std::pair<char, char>>{
	{'A', 'A'}, {'A', 'C'}, {'A', 'G'}, {'A', 'T'}, {'C', 'A'}, {'C', 'C'},
	{'C', 'G'}, {'C', 'T'}, {'G', 'A'}, {'G', 'C'}, {'G', 'G'}, {'G', 'T'},
	{'T', 'A'}, {'T', 'C'}, {'T', 'G'}, {'T', 'T'}};

class chi2
{
  public:
	enum class randomness { fine, suspect, reject };

	static std::string stringify(randomness value)
	{
		if (value == chi2::randomness::fine) return "fine";
		if (value == chi2::randomness::suspect) return "suspect";
		if (value == chi2::randomness::reject) return "reject";
		return "unkown";
	}

	template <typename T, typename I>
	static randomness equi(I begin, I end, const std::vector<T> &categories)
	{
		if (categories.size() < 2) return randomness::reject;

		std::unordered_map<T, size_t> counts;
		for (; begin < end; ++begin) {
			counts[*begin]++;
		}

		double Y = 0.0;
		double N = 0.0;
		for (auto [key, count] : counts) {
			Y += count * count;
			N += count;
		}

		double V = Y * categories.size() / N - N;

		auto thresholds = std::vector<std::tuple<double, double, double>>{
			{0.00016, 3.841, 6.635}, {0.02010, 5.991, 9.210},
			{0.1148, 7.815, 11.34}, // 3
			{0.0, 0.0, 0.0},         {0.0, 0.0, 0.0},
			{0.0, 0.0, 0.0},         {0.0, 0.0, 0.0},
			{0.0, 0.0, 0.0},         {0.0, 0.0, 0.0},
			{0.0, 0.0, 0.0},         {0.0, 0.0, 0.0},
			{0.0, 0.0, 0.0},         {0.0, 0.0, 0.0},
			{0.0, 0.0, 0.0},         {5.229, 25.00, 30.58}};

		if (thresholds.size() + 2 < categories.size())
			return randomness::reject;

		auto k = categories.size() - 2;
		auto [toogood, weak, bad] = thresholds[k];
		if (V < toogood) return randomness::suspect;
		if (V > bad) return randomness::reject;
		if (V > weak) return randomness::suspect;
		return randomness::fine;
	}
};

TEST_CASE("Some simple checks")
{
	char *tiny = (char *)malloc(6);
	char *small = (char *)malloc(26);
	char *large = (char *)malloc(10001);

	const size_t seed = 23;
	dna4_fill_random(tiny, tiny + 5, seed);
	dna4_fill_random(small, small + 25, seed);
	dna4_fill_random(large, large + 10000, seed);
	tiny[5] = small[25] = large[10000] = '\0';
	std::cerr << tiny << std::endl;
	std::cerr << small << std::endl;

	REQUIRE(memcmp(tiny, small, 5) == 0);
	REQUIRE(memcmp(tiny, large, 5) == 0);
	REQUIRE(memcmp(small, large, 25) == 0);

	REQUIRE(dna4::random(5, seed) == tiny);
	REQUIRE(dna4::random(25, seed) == small);
	REQUIRE(dna4::random(10000, seed) == large);

	free(tiny);
	free(small);
	free(large);
}

TEST_CASE("χ2 Simple")
{
	constexpr size_t N = 10000;
	char *buffer = (char *)malloc(N + 1);
	dna4_fill_random(buffer, buffer + N, 1);
	size_t counts[256] = {0};
	dnax_count(counts, buffer, buffer + N);

	auto total =
		std::accumulate(std::begin(counts), std::end(counts), (size_t)0);
	REQUIRE(total == N);

	auto ACGT = counts['A'] + counts['C'] + counts['G'] + counts['T'];
	REQUIRE(ACGT == N);

	std::cerr << counts['A'] << std::endl;
	std::cerr << counts['C'] << std::endl;
	std::cerr << counts['G'] << std::endl;
	std::cerr << counts['T'] << std::endl;

	double Y = (double)counts['A'] * counts['A'] + counts['C'] * counts['C'] +
			   counts['G'] * counts['G'] + counts['T'] * counts['T'];
	std::cerr << Y << std::endl;
	auto V = Y * 4 / N - N;
	std::cerr << V << std::endl;

	auto val =
		chi2::equi(buffer, buffer + N, std::vector<char>{'A', 'C', 'G', 'T'});
	std::cerr << chi2::stringify(val) << std::endl;

	free(buffer);
}

TEST_CASE("χ2 Multiple")
{
	constexpr size_t N = 10000;
	auto buffer1 = std::string(N, '\0');
	auto buffer2 = std::string(N, '\0');
	auto buffer3 = std::string(N, '\0');
	auto buffer4 = std::string(N, '\0');
	auto buffer5 = std::string(N, '\0');

	uint32_t seed = 23;
	dna4_fill_random(dna::begin(buffer1), dna::end(buffer1), seed++);
	dna4_fill_random(dna::begin(buffer2), dna::end(buffer2), seed++);
	dna4_fill_random(dna::begin(buffer3), dna::end(buffer3), seed++);
	dna4_fill_random(dna::begin(buffer4), dna::end(buffer4), seed++);
	dna4_fill_random(dna::begin(buffer5), dna::end(buffer5), seed++);

	auto acgt = std::vector<char>{'A', 'C', 'G', 'T'};
	auto randomness1 = chi2::equi(buffer1.begin(), buffer1.end(), acgt);
	auto randomness2 = chi2::equi(buffer2.begin(), buffer2.end(), acgt);
	auto randomness3 = chi2::equi(buffer3.begin(), buffer3.end(), acgt);
	auto randomness4 = chi2::equi(buffer4.begin(), buffer4.end(), acgt);
	auto randomness5 = chi2::equi(buffer5.begin(), buffer5.end(), acgt);

	REQUIRE(randomness1 != chi2::randomness::reject);
	REQUIRE(randomness2 != chi2::randomness::reject);
	REQUIRE(randomness3 != chi2::randomness::reject);
	REQUIRE(randomness4 != chi2::randomness::reject);
	REQUIRE(randomness5 != chi2::randomness::reject);

	int suspicious = 0;
	if (randomness1 == chi2::randomness::suspect) suspicious++;
	if (randomness2 == chi2::randomness::suspect) suspicious++;
	if (randomness3 == chi2::randomness::suspect) suspicious++;
	if (randomness4 == chi2::randomness::suspect) suspicious++;
	if (randomness5 == chi2::randomness::suspect) suspicious++;

	REQUIRE(suspicious < 3);
}

TEST_CASE("χ2 2mer")
{
	constexpr size_t N = 10000;
	constexpr int runs = 5;

	uint32_t seed = 23;
	int suspicious = 0;

	for (int i = 0; i < runs; i++) {
		auto buffer = std::string(N, '\0');
		dna4_fill_random(dna::begin(buffer), dna::end(buffer), seed++);

		auto kmers = std::vector<std::pair<char, char>>{};
		std::transform(
			buffer.begin(), buffer.end() - 1, buffer.begin() + 1,
			std::back_inserter(kmers),
			[](char a, char b) { return std::make_pair(a, b); });

		auto randomness = chi2::equi(kmers.begin(), kmers.end(), two_mers);
		REQUIRE(randomness != chi2::randomness::reject);
		if (randomness == chi2::randomness::suspect) suspicious++;
	}

	REQUIRE(suspicious < runs / 2);
}

TEST_CASE("χ2 2mer 4apart")
{
	constexpr size_t N = 10000;
	constexpr int runs = 5;

	uint32_t seed = 1593;
	int suspicious = 0;

	for (int i = 0; i < runs; i++) {
		auto buffer = std::string(N, '\0');
		dna4_fill_random(dna::begin(buffer), dna::end(buffer), seed++);

		auto kmers = std::vector<std::pair<char, char>>{};
		std::transform(
			buffer.begin(), buffer.end() - 4, buffer.begin() + 4,
			std::back_inserter(kmers),
			[](char a, char b) { return std::make_pair(a, b); });

		auto randomness = chi2::equi(kmers.begin(), kmers.end(), two_mers);
		REQUIRE(randomness != chi2::randomness::reject);
		if (randomness == chi2::randomness::suspect) suspicious++;
	}

	REQUIRE(suspicious < runs / 2);
}

TEST_CASE("χ2 2mer 16apart")
{
	constexpr size_t N = 10000;
	constexpr int runs = 5;

	uint32_t seed = 1593;
	int suspicious = 0;

	for (int i = 0; i < runs; i++) {
		auto buffer = std::string(N, '\0');
		dna4_fill_random(dna::begin(buffer), dna::end(buffer), seed++);

		auto kmers = std::vector<std::pair<char, char>>{};
		std::transform(
			buffer.begin(), buffer.end() - 16, buffer.begin() + 16,
			std::back_inserter(kmers),
			[](char a, char b) { return std::make_pair(a, b); });

		auto randomness = chi2::equi(kmers.begin(), kmers.end(), two_mers);
		REQUIRE(randomness != chi2::randomness::reject);
		if (randomness == chi2::randomness::suspect) suspicious++;
	}

	REQUIRE(suspicious < runs / 2);
}

TEST_CASE("χ2 seed 2mer")
{
	constexpr size_t N = 10000;
	constexpr int runs = 5;

	uint32_t seed = 42;
	int suspicious = 0;

	for (int i = 0; i < runs; i++) {
		auto buffer1 = std::string(N, '\0');
		auto buffer2 = std::string(N, '\0');

		dna4_fill_random(dna::begin(buffer1), dna::end(buffer1), seed++);
		dna4_fill_random(dna::begin(buffer2), dna::end(buffer2), seed);

		printf("%80.80s\n", buffer1.c_str());

		auto kmers = std::vector<std::pair<char, char>>{};
		std::transform(
			buffer1.begin(), buffer1.end(), buffer2.begin(),
			std::back_inserter(kmers),
			[](char a, char b) { return std::make_pair(a, b); });

		auto randomness = chi2::equi(kmers.begin(), kmers.end(), two_mers);
		REQUIRE(randomness != chi2::randomness::reject);
		if (randomness == chi2::randomness::suspect) suspicious++;
	}

	REQUIRE(suspicious < runs / 2);
}
