#include <dna.h>
#include <random>

extern size_t seed;
extern size_t invrate;

void
gen(char *str, size_t length)
{
	dna4_fill_random(str, str + length, seed);
	*str = '\0';
}

void
mutate(char *str, size_t length)
{
	static const auto NO_A = "CGT";
	static const auto NO_C = "AGT";
	static const auto NO_G = "ACT";
	static const auto NO_T = "ACG";

	auto mut_rand = std::default_random_engine{
		static_cast<std::default_random_engine::result_type>(seed)};
	auto mut_dist = std::uniform_int_distribution<int>{0, 2};
	auto mut_acgt = [&](char c) {
		auto r = mut_dist(mut_rand);
		if (c == 'A') return NO_A[r];
		if (c == 'C') return NO_C[r];
		if (c == 'G') return NO_G[r];
		return NO_T[r];
	};

	for (size_t i = 0; i < length; i += invrate) {
		auto c = str[i];
		str[i] = mut_acgt(c);
	}
}
