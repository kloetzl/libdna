#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <dna.h>
#include <random>
#include <cstring>

static const size_t LENGTH = 1000000;


static const size_t seed = 1729;
static const size_t invrate = 10;

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

void mutate(char *str, size_t length)
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

/** Fake the compiler into thinking *p is being read. Thus it cannot remove *p
 * as unused. */
void escape(void *p)
{
	asm volatile("" : : "g"(p) : "memory");
}


static void libdna4_evodist_jc(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);
	char *other = (char *)malloc(LENGTH + 1);
	gen(other, LENGTH);
	mutate(other, LENGTH);

	while (state.KeepRunning()) {
		auto d = dna4_evodist_jc(forward, forward + LENGTH, other, NULL);
		escape(&d);
	}

	free(forward);
	free(other);
}
BENCHMARK(libdna4_evodist_jc);

static void libdna4_evodist_k80(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);
	char *other = (char *)malloc(LENGTH + 1);
	gen(other, LENGTH);
	mutate(other, LENGTH);

	while (state.KeepRunning()) {
		auto d = dna4_evodist_k80(forward, forward + LENGTH, other, NULL, NULL);
		escape(&d);
	}

	free(forward);
	free(other);
}
BENCHMARK(libdna4_evodist_k80);


double base(const char *begin, const char *end, const char *other,
				size_t *substitutions_ptr)
{
	size_t substitutions = 0;
	size_t i = 0;
	size_t length = end - begin;

	for (; i < length; i++) {
		if (begin[i] != other[i]) {
			substitutions++;
		}
	}

	if (substitutions_ptr) *substitutions_ptr = substitutions;

	// math
	double rate = (double)substitutions / length;

	return rate;
}

static void base(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);
	char *other = (char *)malloc(LENGTH + 1);
	gen(other, LENGTH);
	mutate(other, LENGTH);

	while (state.KeepRunning()) {
		auto d = base(forward, forward + LENGTH, other, NULL);
		escape(&d);
	}

	free(forward);
	free(other);
}
BENCHMARK(base);


double k80_twiddle(const char *begin, const char *end, const char *other,
						size_t *transitions_ptr, size_t *transversions_ptr)
{
	size_t transitions = 0, transversions = 0;
	size_t i = 0;
	size_t length = end - begin;

	for (; i < length; i++) {
		char c = begin[i] + 1;
		char d = other[i] + 1;
		if (__builtin_expect(c != d, 0)) {
			transitions++;
			if (__builtin_expect((c ^ d) & 4, 0)) {
				transversions++;
			}
		}
		// if (begin[i] != other[i]) {
		// 	if ((begin[i] == 'G' || begin[i] == 'C') ^
		// 		(other[i] == 'G' || other[i] == 'C')) {
		// 		transversions++;
		// 	} else {
		// 		transitions++;
		// 	}
		// }
	}

	if (transitions_ptr) *transitions_ptr = transitions;
	if (transversions_ptr) *transversions_ptr = transversions;

	// math
	double P = (double)transitions / length;
	double Q = (double)transversions / length;

	double tmp = 1.0 - 2.0 * P - Q;

	return tmp;
}

static void k80_twiddle(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);
	char *other = (char *)malloc(LENGTH + 1);
	gen(other, LENGTH);
	mutate(other, LENGTH);

	while (state.KeepRunning()) {
		auto d = k80_twiddle(forward, forward + LENGTH, other, NULL, NULL);
		escape(&d);
	}

	free(forward);
	free(other);
}
BENCHMARK(k80_twiddle);


BENCHMARK_MAIN();

