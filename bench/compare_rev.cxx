#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <cstring>
#include <dna.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <iostream>
#include <nmmintrin.h>

static const size_t LENGTH = 1000000;

size_t seed = 1729;
size_t invrate = 100;

extern void
gen(char *str, size_t length);

extern void
mutate(char *str, size_t length);

template <class Bench_fn>
void
bench(benchmark::State &state, Bench_fn fn)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);
	char *other = (char *)malloc(LENGTH + 1);
	gen(other, LENGTH);
	mutate(other, LENGTH);
	std::reverse(other, other + LENGTH);

	for (auto _ : state) {
		auto d = fn(forward, forward + LENGTH, other);
		benchmark::DoNotOptimize(d);
	}

	free(forward);
	free(other);
}

constexpr bool
is_complement(char c, char d)
{
	auto xorr = c ^ d;
	return (xorr & 6) == 4;
}

double
base_rev(const char *begin, const char *end, const char *other)
{
	size_t substitutions = 0;
	size_t i = 0;
	size_t length = end - begin;

	for (; i < length; i++) {
		if (!is_complement(begin[i], other[length - 1 - i])) {
			substitutions++;
		}
	}

	return substitutions;
}

double
intr(const char *begin, const char *end, const char *other)
{
	size_t substitutions = 0;
	size_t offset = 0;
	size_t length = end - begin;

	size_t vec_offset = 0;
	size_t vec_length = length / sizeof(__m128i);

	substitutions += sizeof(__m128i) * vec_length;
	for (; vec_offset < vec_length; vec_offset++) {
		__m128i b;
		memcpy(&b, begin + vec_offset * sizeof(__m128i), sizeof(b));
		__m128i o;
		size_t pos = length - (vec_offset + 1) * sizeof(__m128i);
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

	offset += vec_offset * sizeof(__m128i);

	for (; offset < length; offset++) {
		if (!is_complement(begin[offset], other[length - 1 - offset])) {
			substitutions++;
		}
	}

	return substitutions;
}

size_t
revcomp_then_count_mismatches(
	const char *begin, const char *end, const char *other)
{
	char *buffer = (char *)malloc(LENGTH + 1);
	dna4_revcomp(begin, end, buffer);
	char *buffer_end = buffer + LENGTH;

	auto d = dna4_count_mismatches(buffer, buffer_end, other);

	free(buffer);
	return d;
}

BENCHMARK_CAPTURE(bench, dna4_count_mismatches, dna4_count_mismatches);
BENCHMARK_CAPTURE(bench, intr, intr);
BENCHMARK_CAPTURE(bench, base_rev, base_rev);
BENCHMARK_CAPTURE(
	bench, revcomp_then_count_mismatches, revcomp_then_count_mismatches);

BENCHMARK_MAIN();
