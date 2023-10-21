#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <cstring>
#include <dna.hpp>
#include <random>

#ifdef __SSE2__
#include <emmintrin.h>
#include <immintrin.h>
#include <tmmintrin.h>
#endif

static const size_t LENGTH = 100000;
static const size_t K = 20;
size_t seed = 61;
size_t invrate;

extern void
gen(char *str, size_t length);

template <class Pack_fn>
void
bench_template(benchmark::State &state, Pack_fn fn)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	for (auto _ : state) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t pack = fn(kmer, K);
			benchmark::DoNotOptimize(pack);
		}
	}

	free(forward);
}

static uint64_t
pack_simple(const char *kmer, uint64_t k)
{
	uint64_t return_value = 0;

	while (k--) {
		char c = *kmer++;
		uint64_t val = 0;
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

static size_t
pack_table(const char *kmer, size_t K)
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

static void
printadj(const char *addr, size_t nbytes)
{
	static char buf[129] = {0};
	for (size_t i = 0; i < nbytes; i += 2) {
		sprintf(buf + (i / 2) * 5, " %c %c ", addr[i], addr[i + 1]);
	}

	fprintf(stderr, "%s\n", buf);
}

static void
printhex(const char *addr, size_t nbytes)
{
	static char buf[129] = {0};
	for (size_t i = 0; i < nbytes; i++) {
		sprintf(buf + i * 3, "%02hhx ", (unsigned int)addr[i]);
	}

	fprintf(stderr, "%s\n", buf);
}

static void
printhexshort(const char *addr, size_t nbytes)
{
	static char buf[129] = {0};
	for (size_t i = 0; i < nbytes; i += 2) {
		short z;
		// memcpy(&z, addr + i, sizeof(short));
		memcpy(&z, addr + i + 1, 1);
		memcpy(((char *)&z) + 1, addr + i, 1);
		sprintf(buf + (i / 2) * 5, "%04hx ", z);
	}

	fprintf(stderr, "%s\n", buf);
}

static void
printshort(const char *addr, size_t nbytes)
{
	static char buf[129] = {0};
	for (size_t i = 0; i < nbytes; i += 2) {
		short z;
		memcpy(&z, addr + i, sizeof(short));
		sprintf(buf + (i / 2) * 5, "%04hu ", z);
	}

	fprintf(stderr, "%s\n", buf);
}

#ifdef __SSE2__
#endif

BENCHMARK_CAPTURE(bench_template, dna4_pack_2bits, dna4_pack_2bits);
// BENCHMARK_CAPTURE(bench_template, dnax_pack_4bits, dnax_pack_4bits);
BENCHMARK_CAPTURE(bench_template, pack_simple, pack_simple);
BENCHMARK_CAPTURE(bench_template, pack_table, pack_table);

#ifdef __SSE2__
#endif

BENCHMARK_MAIN();
