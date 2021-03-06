#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <cstring>
#include <dna.h>
#include <iostream>

#ifdef __SSE2__
#include <emmintrin.h>
#include <immintrin.h>
#include <nmmintrin.h>
#endif

static const size_t LENGTH = 1000000;

size_t seed = 1729;
size_t invrate;

extern void
gen(char *str, size_t length);

static void
libdnax_count(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	size_t table[128];

	for (auto _ : state) {
		dnax_count(table, forward, forward + LENGTH);
		benchmark::DoNotOptimize(table);
	}

	free(forward);
}
BENCHMARK(libdnax_count);

static void
libdna4_gc_content(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	for (auto _ : state) {
		double d = dna4_gc_content(forward, forward + LENGTH);
		benchmark::DoNotOptimize(d);
	}

	free(forward);
}
BENCHMARK(libdna4_gc_content);

#define UNLIKELY(X) __builtin_expect((X), 0)

#ifdef __AVX2__
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

	return (double)count / length;
}

static void
popcnt(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	for (auto _ : state) {
		double d = popcnt(forward, forward + LENGTH);
		benchmark::DoNotOptimize(d);
	}

	free(forward);
}
BENCHMARK(popcnt);
#endif

BENCHMARK_MAIN();
