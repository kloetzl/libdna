#include <benchmark/benchmark.h>
#include <cstdlib>
#include <dna>
#include <random>
#include <seqan3/alphabet/nucleotide/dna4.hpp>
#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/range/views/all.hpp> // include all of SeqAn's views
#include <seqan3/range/views/char_to.hpp>
#include <seqan3/std/ranges> // include all of the standard library's views

static const size_t LENGTH = 1000000;

size_t seed = 1729;
size_t invrate;

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

	benchmark::DoNotOptimize(reverse);

	free(reverse);
	free(forward);
}

static void
dnax_revcomp(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	char *reverse = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	for (auto _ : state) {
		dnax_revcomp(dnax_revcomp_table, forward, forward + LENGTH, reverse);
		benchmark::DoNotOptimize(reverse);
	}

	free(forward);
	free(reverse);
}
BENCHMARK(dnax_revcomp);

static void
bwa_encode(char *begin, char *end)
{
	static /*constexpr*/ char table[127];
	memset(table, 4, sizeof(table));
	table['A'] = 0;
	table['C'] = 1;
	table['G'] = 2;
	table['T'] = 3;
	dnax_replace(table, begin, end, begin);
}

static void
bwa_revcomp(const char *begin, const char *end, char *dest)
{
	size_t i = 0, length = end - begin;
	for (; i < length; i++) {
		dest[length - 1 - i] = begin[i] < 4 ? 3 - begin[i] : 4;
	}
}

static void
bwa_bench(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	char *reverse = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);
	bwa_encode(forward, forward + LENGTH);

	for (auto _ : state) {
		bwa_revcomp(forward, forward + LENGTH, reverse);
		benchmark::DoNotOptimize(reverse);
	}

	free(forward);
	free(reverse);
}
BENCHMARK(bwa_bench);

void
bench_seqan3_dna4(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	auto s = std::string{forward};
	auto s_as_dna = s | seqan3::views::char_to<seqan3::dna4>;

	for (auto _ : state) {
		auto revcomp =
			s_as_dna | std::views::reverse | seqan3::views::complement;

		// force evaluation of view
		for (auto foo : revcomp)
			benchmark::DoNotOptimize(foo);
	}

	free(forward);
}

BENCHMARK_CAPTURE(bench, dna4_revcomp, dna4_revcomp);
BENCHMARK(bench_seqan3_dna4);
BENCHMARK_MAIN();
