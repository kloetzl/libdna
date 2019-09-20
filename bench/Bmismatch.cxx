#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <cstring>
#include <dna.h>
#include <iostream>

static const size_t LENGTH = 1000000;

size_t seed = 1729;
size_t invrate = 61;

extern void
gen(char *str, size_t length);

extern void
mutate(char *str, size_t length);

template <typename BenchFn>
static void
bench(benchmark::State &state, BenchFn fn)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);
	char *other = (char *)malloc(LENGTH + 1);
	gen(other, LENGTH);
	mutate(other, LENGTH);

	for (auto _ : state) {
		const char *ptr = forward;
		while (ptr < forward + LENGTH) {
			ptr = fn(ptr, forward + LENGTH, other + (ptr - forward)) + 1;
			benchmark::DoNotOptimize(ptr);
		}
		benchmark::DoNotOptimize(ptr);
	}

	free(forward);
	free(other);
}

static char *
simple(const char *begin, const char *end, const char *other)
{
	size_t length = end - begin;
	size_t i = 0;

	for (; i < length; i++) {
		if (begin[i] != other[i]) break;
	}

	return (char *)begin + i;
}

static char *
chunked(const char *begin, const char *end, const char *other)
{
	size_t length = end - begin;
	size_t i = 0;

	char chunk1[8] = {0};
	char chunk2[8] = {0};

	size_t chunked_length = length - (length % sizeof(chunk1));
	for (i = 0; i < chunked_length; i += sizeof(chunk1)) {
		memcpy(chunk1, &begin[i], sizeof(chunk1));
		memcpy(chunk2, &other[i], sizeof(chunk1));

		int check = memcmp(&chunk1, &chunk2, sizeof(chunk1));
		if (check) {
			break;
		}
	}

	for (; i < length; i++) {
		if (begin[i] != other[i]) break;
	}

	return (char *)begin + i;
}

static char *
direct_cmp(const char *begin, const char *end, const char *other)
{
	size_t length = end - begin;
	size_t i = 0;

	static const size_t chunk_size = 8;

	size_t chunked_length = length - (length % chunk_size);
	for (; i < chunked_length; i += chunk_size) {
		int check = memcmp(begin + i, other + i, chunk_size);
		if (check) {
			break;
		}
	}

	for (; i < length; i++) {
		if (begin[i] != other[i]) break;
	}

	return (char *)begin + i;
}

BENCHMARK_CAPTURE(bench, chunked, chunked);
BENCHMARK_CAPTURE(bench, direct_cmp, direct_cmp);
BENCHMARK_CAPTURE(bench, dnax_find_mismatch, dnax_find_mismatch);
BENCHMARK_CAPTURE(bench, simple, simple);

BENCHMARK_MAIN();
