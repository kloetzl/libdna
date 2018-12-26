#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <cstring>
#include <dna.h>
#include <random>

static const size_t LENGTH = 1000000;

size_t seed = 1729;
size_t invrate;

extern void
gen(char *str, size_t length);

/** Fake the compiler into thinking *p is being read. Thus it cannot remove *p
 * as unused. */
void
escape(void *p)
{
	asm volatile("" : : "g"(p) : "memory");
}

static void
libdnax_replace(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		dnax_replace(dnax_to_dna4_table, forward, forward + LENGTH, forward);
		escape(forward);
	}

	free(forward);
}
BENCHMARK(libdnax_replace);

__attribute__((target_clones("avx", "sse2", "default"))) char *
xto4(const char *begin, const char *end, char *dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);
	// dest == begin is allowed

	for (; begin < end; begin++) {
		char c = *begin & 0x5f;
		// char c = toupper(*begin);
		if (c == 'A' || c == 'C' || c == 'G' || c == 'T') {
			*dest++ = c;
		}
	}

	return dest;
}

static void
xto4(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		xto4(forward, forward + LENGTH, forward);
		escape(forward);
	}

	free(forward);
}
BENCHMARK(xto4);

static void
memcpy(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	char *dest = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		std::memcpy(dest, forward, LENGTH);
		escape(dest);
	}

	free(forward);
}
BENCHMARK(memcpy);

BENCHMARK_MAIN();
