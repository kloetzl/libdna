#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <dna.h>
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

	for (size_t i = 0; i < length; i++) {
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

#define BENCHME(functionname)                                                  \
                                                                               \
	static void functionname(benchmark::State &state)                          \
	{                                                                          \
		char *forward = (char *)malloc(LENGTH + 1);                            \
		char *reverse = (char *)malloc(LENGTH + 1);                            \
		gen(forward, LENGTH);                                                  \
                                                                               \
		while (state.KeepRunning()) {                                          \
			functionname(forward, forward + LENGTH, reverse);                  \
			escape(reverse);                                                   \
		}                                                                      \
                                                                               \
		free(forward);                                                         \
		free(reverse);                                                         \
	}                                                                          \
	BENCHMARK(functionname);

extern "C" {
extern char *
dna4_revcomp_ssse3(const char *begin, const char *end, char * dest);
	
}


BENCHME(dna4_revcomp);
BENCHME(dna4_revcomp_ssse3);
// BENCHME(libdnax);

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

BENCHME(revcomp_switch);

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

BENCHME(revcomp_table4);

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

BENCHME(twiddle);

static void
dnax_revcomp(benchmark::State &state)
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
BENCHMARK(dnax_revcomp);


static void
libdnax_replace(benchmark::State &state)
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
