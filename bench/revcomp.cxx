#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <dna.h>
#include <random>

static const size_t LENGTH = 1000000;

size_t seed = 1729;
size_t invrate;

extern void
gen(char *str, size_t length);

template <class Pack_fn>
void
bench(benchmark::State &state, Pack_fn fn)
{
	char *forward = (char *)malloc(LENGTH + 1);
	char *reverse = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	for (auto _ : state) {
		fn(forward, forward + LENGTH, reverse);
		benchmark::DoNotOptimize(reverse);
	}

	free(reverse);
	free(forward);
}

extern "C" {
extern char *
dna4_revcomp_ssse3(const char *begin, const char *end, char *dest);
}

static char *
revcomp_switch(const char *forward, const char *end, char *reverse)
{
	size_t len = end - forward;

	reverse[len] = '\0';

	for (size_t k = 0; k < len; k++) {
		char c = forward[k], d;
		switch (c) {
			case 'A': d = 'T'; break;
			case 'C': d = 'G'; break;
			case 'G': d = 'C'; break;
			case 'T': d = 'A'; break;
		}
		reverse[len - k - 1] = d;
	}

	return reverse;
}

static char *
revcomp_table4(const char *forward, const char *end, char *reverse)
{
	static /*constexpr*/ char table[127];
	table['A'] = 'T';
	table['T'] = 'A';
	table['G'] = 'C';
	table['C'] = 'G';

	size_t len = end - forward;

	reverse[len] = '\0';

	for (size_t k = 0; k < len; k++) {
		reverse[len - k - 1] = table[forward[k]];
	}

	return reverse;
}

static __attribute__((target_clones("avx2", "sse2", "default"))) char *
twiddle(const char *begin, const char *end, char *__restrict dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	size_t length = end - begin;
	for (size_t i = 0; i < length; i++) {
		char c = begin[length - 1 - i];

		dest[i] = c ^= c & 2 ? 4 : 21;
	}

	return dest + length;
}

char *
subtract(const char *begin, const char *end, char *__restrict dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	size_t length = end - begin;
	for (size_t i = 0; i < length; i++) {
		char c = begin[length - 1 - i];
		int sum = c & 2 ? 'C' + 'G' : 'A' + 'T';

		dest[i] = sum - c;
	}

	return dest + length;
}

static void
dnax_revcomp(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	char *reverse = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		dnax_revcomp(dnax_revcomp_table, forward, forward + LENGTH, reverse);
		benchmark::DoNotOptimize(reverse);
	}

	free(forward);
	free(reverse);
}
BENCHMARK(dnax_revcomp);

static void
dnax_replace(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	char *reverse = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		dnax_replace(dnax_revcomp_table, forward, forward + LENGTH, reverse);
		benchmark::DoNotOptimize(reverse);
	}

	free(forward);
	free(reverse);
}
BENCHMARK(dnax_replace);

BENCHMARK_CAPTURE(bench, dna4_revcomp, dna4_revcomp);
BENCHMARK_CAPTURE(bench, dna4_revcomp_ssse3, dna4_revcomp_ssse3);
BENCHMARK_CAPTURE(bench, revcomp_switch, revcomp_switch);
BENCHMARK_CAPTURE(bench, revcomp_table4, revcomp_table4);
BENCHMARK_CAPTURE(bench, twiddle, twiddle);
BENCHMARK_CAPTURE(bench, subtract, subtract);

BENCHMARK_MAIN();
