#include "Tcommon.h"

#include <catch2/catch.hpp>
#include <cstring>
#include <dna.h>
#include <iostream>
#include <string>
#include <unordered_map>

TEST_CASE("Some simple checks")
{
	for (uint64_t i = 0; i < 64; i++) {
		uint64_t data = 1lu << i;
		uint64_t hash = dna_ihash(data);
		uint64_t inverse = dna_ihash_invert(hash);
		REQUIRE(data == inverse);
	}

	REQUIRE(dna_ihash(0) != dna_ihash(1));
	REQUIRE(dna_ihash(1) != dna_ihash(2));
	REQUIRE(dna_ihash(2) != dna_ihash(3));
	REQUIRE(dna_ihash(256) != dna_ihash(1));
	REQUIRE(dna_ihash(1024) != dna_ihash(1));
	REQUIRE(dna_ihash(1lu << 63) != dna_ihash(1));
}

TEST_CASE("χ2 Bit Distribution")
{
	constexpr size_t N = 10000;
	std::vector<uint64_t> hashes(N);
	std::iota(hashes.begin(), hashes.end(), (uint64_t)0);
	std::transform(hashes.begin(), hashes.end(), hashes.begin(), dna_ihash);

	int suspicious = 0;
	std::vector<uint64_t> bits(N);
	for (int i = 0; i < 64; i++) {
		std::transform(
			hashes.begin(), hashes.end(), bits.begin(),
			[i](auto ihash) { return ihash & (1lu << i); });

		auto val = chi2::equi(
			bits.begin(), bits.end(), std::vector<uint64_t>{0, 1lu << i});

		REQUIRE(val != chi2::randomness::reject);
		if (val == chi2::randomness::suspect) suspicious++;
	}
	REQUIRE(suspicious < 3);
}

TEST_CASE("χ2 Consecutive Bits")
{
	constexpr size_t N = 10000;
	std::vector<uint64_t> hashes(N);
	std::iota(hashes.begin(), hashes.end(), (uint64_t)0);
	std::transform(hashes.begin(), hashes.end(), hashes.begin(), dna_ihash);

	int suspicious = 0;
	std::vector<uint64_t> bits(N);
	for (int i = 0; i < 63; i++) {
		std::transform(
			hashes.begin(), hashes.end(), bits.begin(),
			[i](auto ihash) { return ihash & (3lu << i); });

		auto val = chi2::equi(
			bits.begin(), bits.end(),
			std::vector<uint64_t>{0, 1lu << i, 2lu << i, 3lu << i});

		REQUIRE(val != chi2::randomness::reject);
		if (val == chi2::randomness::suspect) suspicious++;
	}
	REQUIRE(suspicious < 3);
}
