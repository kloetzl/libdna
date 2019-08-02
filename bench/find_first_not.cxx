#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <cstring>
#include <dna.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <random>
#include <smmintrin.h>

static const size_t LENGTH = 1000000;

size_t seed = 1729;
size_t invrate;
size_t mutrate = 101;
size_t k = 32;

extern void
gen(char *str, size_t length);

template <class Pack_fn>
void
bench(benchmark::State &state, Pack_fn fn)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	for (size_t i = 0; i < LENGTH; i += mutrate) {
		forward[i] = '_';
	}

	for (auto _ : state) {
		for (auto marker = forward; marker < forward + LENGTH; marker++) {
			marker = fn(marker, forward + LENGTH);
			benchmark::DoNotOptimize(marker);
		}
	}

	free(forward);
}

static char *
table_based(const char *begin, const char *end)
{
	return dnax_find_first_not_of(dnax_to_dna4_table, begin, end);
}

/*

A: 0100 0001
C: 0100 0011
G: 0100 0111
T: 0101 0100

 */
static char *
shuffle_complex(const char *begin, const char *end)
{
	typedef __m128i vec_type;
	size_t vec_size = sizeof(vec_type);
	size_t capped_length = (end - begin) & ~(vec_size - 1);

#define MAYBE_A (1 << 0)
#define MAYBE_C (1 << 1)
#define MAYBE_G (1 << 2)
#define MAYBE_T (1 << 3)

	vec_type mask_low_nibble = _mm_setr_epi8(
		0, MAYBE_A, 0, MAYBE_C, MAYBE_T, 0, 0, MAYBE_G, 0, 0, 0, 0, 0, 0, 0, 0);
	vec_type mask_high_nibble = _mm_setr_epi8(
		0, 0, 0, 0, MAYBE_A | MAYBE_C | MAYBE_G, MAYBE_T, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0);
	for (size_t i = 0; i < capped_length; i += vec_size) {
		vec_type chunk;
		memcpy(&chunk, begin + i, vec_size);
		// fprintf(stderr, "%16.16s\n", &chunk);

		vec_type v1 = _mm_shuffle_epi8(
			mask_low_nibble, chunk); // mask_low_nibble[chunk]; // SHUFFLE
		// TODO: and 7
		vec_type v2 = _mm_shuffle_epi8(
			mask_high_nibble,
			_mm_and_si128(
				_mm_srai_epi32(chunk, 4),
				_mm_set1_epi8(7))); // mask_high_nibble[chunk >> 4]; // SHUFFLE

		// vec_type debug_or_mask = _mm_set1_epi8('0');
		// vec_type debug_v1 = _mm_or_si128(v1, debug_or_mask);
		// vec_type debug_v2 = _mm_or_si128(v2, debug_or_mask);

		// fprintf(stderr, "%16.16s\n", &debug_v1);
		// fprintf(stderr, "%16.16s\n", &debug_v2);

		vec_type v3 = _mm_and_si128(v1, v2); // AND bits

		// vec_type debug_v3 = _mm_or_si128(v3, debug_or_mask);
		// fprintf(stderr, "%16.16s\n", &debug_v3);

		vec_type v4 = _mm_cmpeq_epi8(v3, _mm_setzero_si128()); // COMPARE WITH 0

		// vec_type debug_v4 = _mm_or_si128(v4, debug_or_mask);
		// fprintf(stderr, "%16.16s\n", &debug_v4);

		int mask = _mm_movemask_epi8(v4);
		if (mask) {
			int offset = __builtin_ctz(mask);
			// fprintf(stderr, "%d\n", offset);
			return (char *)begin + i + offset;
		}

		// fprintf(stderr, "\n");
	}

	// finalize
	return table_based(begin + capped_length, end);
}

static char *
shuffle_fast(const char *begin, const char *end)
{
	typedef __m128i vec_type;
	size_t vec_size = sizeof(vec_type);
	size_t capped_length = (end - begin) & ~(vec_size - 1);

	vec_type mask_low_nibble =
		_mm_setr_epi8(0, 'A', 0, 'C', 'T', 0, 0, 'G', 0, 0, 0, 0, 0, 0, 0, 0);
	for (size_t i = 0; i < capped_length; i += vec_size) {
		vec_type chunk;
		memcpy(&chunk, begin + i, vec_size);

		vec_type v1 = _mm_shuffle_epi8(mask_low_nibble, chunk);
		vec_type v4 = _mm_cmpeq_epi8(v1, chunk);

		int mask = ~_mm_movemask_epi8(v4) & ((1 << vec_size) - 1);
		if (mask) {
			return (char *)begin + i + __builtin_ctz(mask);
		}
	}

	// finalize
	return table_based(begin + capped_length, end);
}

BENCHMARK_CAPTURE(bench, table_based, table_based);
BENCHMARK_CAPTURE(bench, shuffle_complex, shuffle_complex);
BENCHMARK_CAPTURE(bench, shuffle_fast, shuffle_fast);

BENCHMARK_MAIN();
