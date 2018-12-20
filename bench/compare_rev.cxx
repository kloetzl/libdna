#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <cstring>
#include <dna.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <iostream>
#include <nmmintrin.h>
#include <random>

static const size_t LENGTH = 1000000;

static const size_t seed = 1729;
static const size_t invrate = 100;

void
gen(char *str, size_t length)
{
	static const char *ACGT = "ACGT";

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

/** Fake the compiler into thinking *p is being read. Thus it cannot remove *p
 * as unused. */
void
escape(void *p)
{
	asm volatile("" : : "g"(p) : "memory");
}

static void
dna4_evodist_jc(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);
	char *other = (char *)malloc(LENGTH + 1);
	gen(other, LENGTH);
	mutate(other, LENGTH);

	size_t subst = 0;

	while (state.KeepRunning()) {
		auto d = dna4_evodist_jc(forward, forward + LENGTH, other, &subst);
		escape(&d);
	}

	// std::cout << subst << std::endl;

	free(forward);
	free(other);
}
BENCHMARK(dna4_evodist_jc);

constexpr bool
is_complement(char c, char d)
{
	auto xorr = c ^ d;
	return (xorr & 6) == 4;
}

double
base_rev(
	const char *begin,
	const char *end,
	const char *other,
	size_t *substitutions_ptr)
{
	size_t substitutions = 0;
	size_t i = 0;
	size_t length = end - begin;

	for (; i < length; i++) {
		if (!is_complement(begin[i], other[length - 1 - i])) {
			substitutions++;
		}
	}

	if (substitutions_ptr) *substitutions_ptr = substitutions;

	// math
	double rate = (double)substitutions / length;

	return rate;
}

static void
base_rev(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);
	char *other = (char *)malloc(LENGTH + 1);
	gen(other, LENGTH);
	mutate(other, LENGTH);
	std::reverse(other, other + LENGTH);

	size_t subst = 0;

	while (state.KeepRunning()) {
		auto d = base_rev(forward, forward + LENGTH, other, &subst);
		escape(&d);
	}

	std::cout << subst << std::endl;

	free(forward);
	free(other);
}
BENCHMARK(base_rev);

double
intr(
	const char *begin,
	const char *end,
	const char *other,
	size_t *substitutions_ptr)
{
	size_t substitutions = 0;
	size_t offset = 0;
	size_t length = end - begin;

	size_t avx2_offset = 0;
	size_t avx2_length = length / sizeof(__m128i);

	substitutions += sizeof(__m128i) * avx2_length;
	for (; avx2_offset < avx2_length; avx2_offset++) {
		__m128i b;
		memcpy(&b, begin + avx2_offset * sizeof(__m128i), sizeof(b));
		__m128i o;
		size_t pos = length - (avx2_offset + 1) * sizeof(__m128i);
		memcpy(&o, other + pos, sizeof(o));

		__m128i mask =
			_mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
		__m128i reversed = _mm_shuffle_epi8(o, mask);

		__m128i v1 = _mm_xor_si128(b, reversed);
		__m128i mask6 = _mm_set1_epi8(6);
		__m128i v2 = _mm_and_si128(v1, mask6);
		__m128i mask4 = _mm_set1_epi8(4);
		__m128i v3 = _mm_cmpeq_epi8(v2, mask4);

		unsigned int vmask = _mm_movemask_epi8(v3);
		// substitutions += sizeof(__m128i) - __builtin_popcount(vmask);
		substitutions -= __builtin_popcount(vmask);
	}

	offset += avx2_offset * sizeof(__m128i);

	for (; offset < length; offset++) {
		if (!is_complement(begin[offset], other[length - 1 - offset])) {
			substitutions++;
		}
	}

	if (substitutions_ptr) *substitutions_ptr = substitutions;

	// math
	double rate = (double)substitutions / length;

	return rate;
}

static void
intr(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);
	char *other = (char *)malloc(LENGTH + 1);
	gen(other, LENGTH);
	mutate(other, LENGTH);
	std::reverse(other, other + LENGTH);

	size_t subst = 0;

	while (state.KeepRunning()) {
		auto d = intr(forward, forward + LENGTH, other, &subst);
		escape(&d);
	}

	std::cout << subst << std::endl;

	free(forward);
	free(other);
}
BENCHMARK(intr);

BENCHMARK_MAIN();
