#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <dna.h>
#include <random>

static const size_t LENGTH = 1000000;


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

/** Fake the compiler into thinking *p is being read. Thus it cannot remove *p
 * as unused. */
void escape(void *p)
{
	asm volatile("" : : "g"(p) : "memory");
}


static void libdna4(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	char *reverse = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		dna4_revcomp(forward, forward + LENGTH, reverse);
		escape(reverse);
	}

	free(forward);
	free(reverse);
}
BENCHMARK(libdna4);

static void libdnax(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	char *reverse = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		dnax_revcomp(dnax_revcomp_table, forward, forward + LENGTH, reverse);
		escape(reverse);
	}

	free(forward);
	free(reverse);
}
BENCHMARK(libdnax);

static char *revcomp_table4(const char *forward, const char *end, char *reverse)
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

static void revcomp_table4(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	char *reverse = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		revcomp_table4(forward, forward + LENGTH, reverse);
		escape(reverse);
	}

	free(forward);
	free(reverse);
}
BENCHMARK(revcomp_table4);

static void libdnax_replace(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	char *reverse = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		dnax_replace(dnax_revcomp_table, forward, forward + LENGTH, reverse);
		escape(reverse);
	}

	free(forward);
	free(reverse);
}
BENCHMARK(libdnax_replace);


BENCHMARK_MAIN();
