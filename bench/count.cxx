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

/** Fake the compiler into thinking *p is being read. Thus it cannot remove *p
 * as unused. */
void
escape(void *p)
{
	asm volatile("" : : "g"(p) : "memory");
}

static void
libdnax_count(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	size_t table[128];

	while (state.KeepRunning()) {
		dnax_count(table, forward, forward + LENGTH);
		escape(table);
	}

	free(forward);
}
BENCHMARK(libdnax_count);

static void
libdna4_gc_content(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		double d = dna4_gc_content(forward, forward + LENGTH);
		escape(&d);
	}

	double d = dna4_gc_content(forward, forward + LENGTH);
	std::cerr << d << std::endl;

	free(forward);
}
BENCHMARK(libdna4_gc_content);

#define UNLIKELY(X) __builtin_expect((X), 0)

double
popcnt(const char *begin, const char *end)
{
	size_t offset = 0;
	size_t length = end - begin;

	const __m256i avx2C = _mm256_set1_epi8('C');
	const __m256i avx2G = _mm256_set1_epi8('G');

	size_t avx2_offset = 0;
	size_t avx2_length = length / sizeof(__m256i);

	size_t count = 0;

	for (; avx2_offset < avx2_length; avx2_offset++) {
		__m256i b;
		memcpy(&b, begin + avx2_offset * sizeof(__m256i), sizeof(b));

		__m256i v1 = _mm256_cmpeq_epi8(b, avx2C);
		__m256i v2 = _mm256_cmpeq_epi8(b, avx2G);

		unsigned int vmask =
			_mm256_movemask_epi8(v1) | _mm256_movemask_epi8(v2);
		count += __builtin_popcount(vmask);
	}

	offset += avx2_offset * sizeof(__m256i);

	size_t table[128];
	dnax_count(table, begin + offset, end);
	count += table['C'] + table['G'];

	return (double)count / length;
}

static void
popcnt(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		double d = popcnt(forward, forward + LENGTH);
		escape(&d);
	}

	double d = popcnt(forward, forward + LENGTH);
	std::cerr << d << std::endl;

	free(forward);
}
BENCHMARK(popcnt);

BENCHMARK_MAIN();
