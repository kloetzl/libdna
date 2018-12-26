#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <cstring>
#include <dna.h>
#include <iostream>
#include <random>

static const size_t LENGTH = 1000000;

static const size_t seed = 1729;
static const size_t invrate = 61;

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

BENCHMARK_CAPTURE(bench, dnax_mismatch, dnax_mismatch);
BENCHMARK_CAPTURE(bench, chunked, chunked);
BENCHMARK_CAPTURE(bench, simple, simple);

BENCHMARK_MAIN();
