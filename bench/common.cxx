#include <random>

extern size_t seed;
extern size_t invrate;

void
gen(char *str, size_t length)
{
	static const char *ACGT = "ACGT";
	fprintf(stderr, "%zu\n", seed);
	auto base_rand = std::default_random_engine{seed};
	auto base_dist = std::uniform_int_distribution<int>{0, 3};
	auto base_acgt = [&] { return ACGT[base_dist(base_rand)]; };

	while (length--) {
		*str++ = base_acgt();
	}

	*str = '\0';
}

void
mutate(char *str, size_t length)
{
	static const auto NO_A = "CGT";
	static const auto NO_C = "AGT";
	static const auto NO_G = "ACT";
	static const auto NO_T = "ACG";

	auto mut_rand = std::default_random_engine{seed};
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
