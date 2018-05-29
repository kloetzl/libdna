#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <dna.h>
#include <random>

static const size_t LENGTH = 100000;
static const size_t K = 32;
static const size_t seed = 1729;

void gen(char *str, size_t length)
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

void escape(void *p)
{
	asm volatile("" : : "g"(p) : "memory");
}



static size_t hash_simple(const char *kmer, size_t k)
{
	size_t return_value = 0;

	while (k--) {
		char c = *kmer++;
		size_t val = 0;
		switch (c) {
			case 'A': val = 0; break;
			case 'C': val = 1; break;
			case 'G': val = 2; break;
			case 'T': val = 3; break;
		}

		return_value <<= 2;
		return_value |= val;
	}

	return return_value;
}

static size_t hash_twiddle(const char *kmer, size_t k)
{
	size_t return_value = 0;

	while (k--) {
		char c = *kmer++;
		c &= 6;
		c ^= c >> 1;

		return_value <<= 2;
		return_value |= c >> 1;
	}

	return return_value;
}

static size_t hash_twiddle_length(const char *kmer, size_t K)
{
	size_t return_value = 0;
	size_t k = 0;

	while (k < K) {
		char c = kmer[k++];
		c &= 6;
		c ^= c >> 1;

		return_value <<= 2;
		return_value |= c >> 1;
	}

	return return_value;
}

static size_t hash_twiddle_length_unordered(const char *kmer, size_t K)
{
	size_t return_value = 0;
	size_t k = 0;

	while (k < K) {
		char c = kmer[k++];
		c &= 6;

		return_value <<= 2;
		return_value |= c >> 1;
	}

	return return_value;
}

static size_t hash_table(const char *kmer, size_t K)
{
	static char table[127];
	table['A'] = 0;
	table['C'] = 1;
	table['G'] = 2;
	table['T'] = 3;

	size_t return_value = 0;
	size_t k = 0;

	while (k < K) {
		return_value <<= 2;
		return_value |= table[kmer[k++]];
	}

	return return_value;
}


static void hash_simple(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t hash = hash_simple(kmer, K);
			escape(&hash);
		}
	}

	free(forward);
}
BENCHMARK(hash_simple);

static void hash_twiddle(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t hash = hash_twiddle(kmer, K);
			escape(&hash);
		}
	}

	free(forward);
}
BENCHMARK(hash_twiddle);

static void hash_twiddle_length(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t hash = hash_twiddle_length(kmer, K);
			escape(&hash);
		}
	}

	free(forward);
}
BENCHMARK(hash_twiddle_length);

static void hash_twiddle_length_unordered(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t hash = hash_twiddle_length_unordered(kmer, K);
			escape(&hash);
		}
	}

	free(forward);
}
BENCHMARK(hash_twiddle_length_unordered);

static void hash_table(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t hash = hash_table(kmer, K);
			escape(&hash);
		}
	}

	free(forward);
}
BENCHMARK(hash_table);

static void libdna4_hash(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t hash = dna4_hash(kmer, K);
			escape(&hash);
		}
	}

	free(forward);
}
BENCHMARK(libdna4_hash);


static void libdnax_hash(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t hash = dnax_hash(dnax_hash_table, kmer, K);
			escape(&hash);
		}
	}

	free(forward);
}
BENCHMARK(libdnax_hash);



BENCHMARK_MAIN();
