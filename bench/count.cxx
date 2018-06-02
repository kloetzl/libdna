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


static void libdnax_count(benchmark::State &state)
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

static void libdna4_gc_content(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		double d = dna4_gc_content(forward, forward + LENGTH);
		escape(&d);
	}

	free(forward);
}
BENCHMARK(libdna4_gc_content);



BENCHMARK_MAIN();

