#include <algorithm>
#include <benchmark/benchmark.h>
#include <string>

constexpr int LENGTH = 1'000'000;

static void
empty(benchmark::State &state)
{
	for (auto _ : state) {
		auto s = std::string(LENGTH, '\0');
		benchmark::DoNotOptimize(s);
	}
}

static void
empty_bang(benchmark::State &state)
{
	for (auto _ : state) {
		auto s = std::string(LENGTH, '!');
		benchmark::DoNotOptimize(s);
	}
}

static void
copy(benchmark::State &state)
{
	auto t = std::string(LENGTH, 0);
	for (auto _ : state) {
		auto s = t; // copy
		benchmark::DoNotOptimize(s);
	}
}

BENCHMARK(empty);
BENCHMARK(empty_bang);
BENCHMARK(copy);

BENCHMARK_MAIN();
