#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <cstring>
#include <dna.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <iostream>
#include <nmmintrin.h>
#include <random>

static const size_t LENGTH = 1000000;

size_t seed = 1729;
size_t invrate = 71;
size_t gaprate = 103;

extern void
gen(char *str, size_t length);

extern void
mutate(char *str, size_t length);

template <class Bench_fn>
void
old_bench(benchmark::State &state, Bench_fn fn)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);
	char *other = (char *)malloc(LENGTH + 1);
	gen(other, LENGTH);
	mutate(other, LENGTH);
	for (size_t i = 0; i < LENGTH; i+= gaprate) {
		other[i] = '-';
	}

	for (auto _ : state) {
		auto d = fn(forward, forward + LENGTH, other);
		benchmark::DoNotOptimize(d);
	}

	free(other);
	free(forward);
}

template <class Bench_fn>
void
bench(benchmark::State &state, Bench_fn fn)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);
	char *other = (char *)malloc(LENGTH + 1);
	gen(other, LENGTH);
	mutate(other, LENGTH);
	for (size_t i = 0; i < LENGTH; i+= gaprate) {
		other[i] = '-';
	}

	size_t gaps = 0;

	for (auto _ : state) {
		auto d = fn(forward, forward + LENGTH, other, &gaps);
		benchmark::DoNotOptimize(d);
		benchmark::DoNotOptimize(gaps);
	}

	free(other);
	free(forward);
}

size_t
base(const char *begin, const char *end, const char *other, size_t *gaps)
{
	size_t substitutions = 0;
	size_t i = 0;
	size_t total = 0;
	size_t length = end - begin;

	for (; i < length; i++) {
		if (begin[i] == '-' || other[i] == '-') continue;

		if (begin[i] != other[i]) {
			substitutions++;
		}

		total++;
	}

	if (gaps) *gaps = length - total;
	return substitutions;
}

size_t
intrinsics_sse(const char *begin, const char *end, const char *other, size_t *gaps)
{
	size_t local_total = 0;
	size_t substitutions = 0;
	size_t i = 0;
	size_t length = end - begin;

	using vec_type = __m128i;
	const auto vec_size = sizeof(vec_type);

	size_t length_chunked = length - (length % vec_size);

	vec_type all_gap = _mm_set1_epi8('-');

	for (; i < length_chunked; i += vec_size) {
		vec_type chunk1;
		memcpy(&chunk1, begin + i, vec_size);
		vec_type chunk2;
		memcpy(&chunk2, other + i, vec_size);

		vec_type eql = _mm_cmpeq_epi8(chunk1, chunk2);
		// unsigned int neql_mask = (~_mm_movemask_epi8(eql)) &
		// 						 (((unsigned long)1 << vec_size) - 1);

		vec_type gap1 = _mm_cmpeq_epi8(chunk1, all_gap);
		vec_type gap2 = _mm_cmpeq_epi8(chunk2, all_gap);
		vec_type gaps = _mm_or_si128(gap1, gap2);
		eql = _mm_or_si128(eql, gaps);

		unsigned int eqlorgap = _mm_movemask_epi8(eql);
		unsigned int gap_mask = _mm_movemask_epi8(gaps);

		unsigned int neql_mask = (~eqlorgap) & (((unsigned long)1 << vec_size) - 1);
		substitutions += __builtin_popcount(neql_mask);
		local_total += vec_size - __builtin_popcount(gap_mask);
	}

	for (; i < length; i++) {
		if (begin[i] == '-' || other[i] == '-') continue;

		if (begin[i] != other[i]) {
			substitutions++;
		}

		local_total++;
	}

	if (gaps) *gaps = length - local_total;
	return substitutions;
}

size_t
intrinsics_sse_crazy(const char *begin, const char *end, const char *other, size_t *gapptr)
{
	size_t local_total = 0;
	size_t local_gaps = 0;
	size_t substitutions = 0;
	size_t equalorgap = 0;
	size_t i = 0;
	size_t length = end - begin;

	using vec_type = __m128i;
	const auto vec_size = sizeof(vec_type);

	size_t length_chunked = length - (length % vec_size);

	vec_type all_gap = _mm_set1_epi8('-');

	for (; i < length_chunked; i += vec_size) {
		vec_type chunk1;
		memcpy(&chunk1, begin + i, vec_size);
		vec_type chunk2;
		memcpy(&chunk2, other + i, vec_size);

		vec_type eql = _mm_cmpeq_epi8(chunk1, chunk2);

		vec_type chimeric = _mm_min_epu8(chunk1, chunk2); // safe, because '-' < ACGT
		vec_type gaps = _mm_cmpeq_epi8(chimeric, all_gap);

		unsigned int gap_mask = _mm_movemask_epi8(gaps);
		unsigned int eqlorgap = _mm_movemask_epi8(eql) | gap_mask;

		equalorgap += __builtin_popcount(eqlorgap);
		local_gaps += __builtin_popcount(gap_mask);
	}

	substitutions = i - equalorgap;	
	local_total = i - local_gaps;

	for (; i < length; i++) {
		if (begin[i] == '-' || other[i] == '-') continue;

		if (begin[i] != other[i]) {
			substitutions++;
		}

		local_total++;
	}

	if (gapptr) *gapptr = length - local_total;
	return substitutions;
}


#ifdef __AVX2__

size_t
intrinsics_avx2(const char *begin, const char *end, const char *other, size_t *gaps)
{
	size_t local_total = 0;
	size_t substitutions = 0;
	size_t i = 0;
	size_t length = end - begin;

	using vec_type = __m256i;
	const auto vec_size = sizeof(vec_type);

	size_t length_chunked = length - (length % vec_size);

	vec_type all_gap = _mm256_set1_epi8('-');

	for (; i < length_chunked; i += vec_size) {
		vec_type chunk1;
		memcpy(&chunk1, begin + i, vec_size);
		vec_type chunk2;
		memcpy(&chunk2, other + i, vec_size);

		vec_type eql = _mm256_cmpeq_epi8(chunk1, chunk2);

		vec_type gap1 = _mm256_cmpeq_epi8(chunk1, all_gap);
		vec_type gap2 = _mm256_cmpeq_epi8(chunk2, all_gap);
		vec_type gaps = _mm256_or_si256(gap1, gap2);
		eql = _mm256_or_si256(eql, gaps);

		unsigned int eqlorgap = _mm256_movemask_epi8(eql);
		unsigned int gap_mask = _mm256_movemask_epi8(gaps);

		unsigned int neql_mask = (~eqlorgap) & (((unsigned long)1 << vec_size) - 1);
		substitutions += __builtin_popcount(neql_mask);
		local_total += vec_size - __builtin_popcount(gap_mask);
	}

	for (; i < length; i++) {
		if (begin[i] == '-' || other[i] == '-') continue;

		if (begin[i] != other[i]) {
			substitutions++;
		}

		local_total++;
	}

	if (gaps) *gaps = length - local_total;
	return substitutions;
}

size_t
intrinsics_avx2_derp(const char *begin, const char *end, const char *other, size_t *gapptr)
{
	size_t local_total = 0;
	size_t local_gaps = 0;
	size_t substitutions = 0;
	size_t equalorgap = 0;
	size_t i = 0;
	size_t length = end - begin;

	using vec_type = __m256i;
	const auto vec_size = sizeof(vec_type);

	size_t length_chunked = length - (length % vec_size);

	vec_type all_gap = _mm256_set1_epi8('-');

	for (; i < length_chunked; i += vec_size) {
		vec_type chunk1;
		memcpy(&chunk1, begin + i, vec_size);
		vec_type chunk2;
		memcpy(&chunk2, other + i, vec_size);

		vec_type eql = _mm256_cmpeq_epi8(chunk1, chunk2);

		vec_type gap1 = _mm256_cmpeq_epi8(chunk1, all_gap);
		vec_type gap2 = _mm256_cmpeq_epi8(chunk2, all_gap);
		vec_type gaps = _mm256_or_si256(gap1, gap2);
		// eql = _mm256_or_si256(eql, gaps);

		unsigned int gap_mask = _mm256_movemask_epi8(gaps);
		unsigned int eqlorgap = _mm256_movemask_epi8(eql) | gap_mask;

		equalorgap += __builtin_popcount(eqlorgap);
		local_gaps += __builtin_popcount(gap_mask);
	}

	substitutions = i - equalorgap;	
	local_total = i - local_gaps;

	for (; i < length; i++) {
		if (begin[i] == '-' || other[i] == '-') continue;

		if (begin[i] != other[i]) {
			substitutions++;
		}

		local_total++;
	}

	if (gapptr) *gapptr = length - local_total;
	return substitutions;
}

size_t
intrinsics_avx2_crazy(const char *begin, const char *end, const char *other, size_t *gapptr)
{
	size_t local_total = 0;
	size_t local_gaps = 0;
	size_t substitutions = 0;
	size_t equalorgap = 0;
	size_t i = 0;
	size_t length = end - begin;

	using vec_type = __m256i;
	const auto vec_size = sizeof(vec_type);

	size_t length_chunked = length - (length % vec_size);

	vec_type all_gap = _mm256_set1_epi8('-');

	for (; i < length_chunked; i += vec_size) {
		vec_type chunk1;
		memcpy(&chunk1, begin + i, vec_size);
		vec_type chunk2;
		memcpy(&chunk2, other + i, vec_size);

		vec_type eql = _mm256_cmpeq_epi8(chunk1, chunk2);

		vec_type chimeric = _mm256_min_epu8(chunk1, chunk2); // safe, because '-' < ACGT
		vec_type gaps = _mm256_cmpeq_epi8(chimeric, all_gap);

		unsigned int gap_mask = _mm256_movemask_epi8(gaps);
		unsigned int eqlorgap = _mm256_movemask_epi8(eql) | gap_mask;

		equalorgap += __builtin_popcount(eqlorgap);
		local_gaps += __builtin_popcount(gap_mask);
	}

	substitutions = i - equalorgap;	
	local_total = i - local_gaps;

	for (; i < length; i++) {
		if (begin[i] == '-' || other[i] == '-') continue;

		if (begin[i] != other[i]) {
			substitutions++;
		}

		local_total++;
	}

	if (gapptr) *gapptr = length - local_total;
	return substitutions;
}

#endif

#ifdef __AVX512BW__

size_t
intrinsics_avx512_256_crazy(const char *begin, const char *end, const char *other, size_t *gapptr)
{
	size_t local_total = 0;
	size_t local_gaps = 0;
	size_t substitutions = 0;
	size_t equalorgap = 0;
	size_t i = 0;
	size_t length = end - begin;

	using vec_type = __m256i;
	const auto vec_size = sizeof(vec_type);

	size_t length_chunked = length - (length % vec_size);

	vec_type all_gap = _mm256_set1_epi8('-');

	for (; i < length_chunked; i += vec_size) {
		vec_type chunk1;
		memcpy(&chunk1, begin + i, vec_size);
		vec_type chunk2;
		memcpy(&chunk2, other + i, vec_size);

		unsigned int gap_mask = _mm256_cmpeq_epi8_mask(chunk1, chunk2);

		vec_type chimeric = _mm256_min_epu8(chunk1, chunk2); // safe, because '-' < ACGT
		unsigned int eqlorgap = _mm256_cmpeq_epi8_mask(chimeric, all_gap) | gap_mask;

		equalorgap += __builtin_popcount(eqlorgap);
		local_gaps += __builtin_popcount(gap_mask);
	}

	substitutions = i - equalorgap;	
	local_total = i - local_gaps;

	for (; i < length; i++) {
		if (begin[i] == '-' || other[i] == '-') continue;

		if (begin[i] != other[i]) {
			substitutions++;
		}

		local_total++;
	}

	if (gapptr) *gapptr = length - local_total;
	return substitutions;
}

size_t
intrinsics_avx512_crazy(const char *begin, const char *end, const char *other, size_t *gapptr)
{
	size_t local_total = 0;
	size_t local_gaps = 0;
	size_t substitutions = 0;
	size_t equalorgap = 0;
	size_t i = 0;
	size_t length = end - begin;

	using vec_type = __m512i;
	const auto vec_size = sizeof(vec_type);

	size_t length_chunked = length - (length % vec_size);

	vec_type all_gap = _mm512_set1_epi8('-');

	for (; i < length_chunked; i += vec_size) {
		vec_type chunk1;
		memcpy(&chunk1, begin + i, vec_size);
		vec_type chunk2;
		memcpy(&chunk2, other + i, vec_size);

		unsigned int gap_mask = _mm512_cmpeq_epi8_mask(chunk1, chunk2);

		vec_type chimeric = _mm512_min_epu8(chunk1, chunk2); // safe, because '-' < ACGT
		unsigned int eqlorgap = _mm512_cmpeq_epi8_mask(chimeric, all_gap) | gap_mask;

		equalorgap += __builtin_popcount(eqlorgap);
		local_gaps += __builtin_popcount(gap_mask);
	}

	substitutions = i - equalorgap;	
	local_total = i - local_gaps;

	for (; i < length; i++) {
		if (begin[i] == '-' || other[i] == '-') continue;

		if (begin[i] != other[i]) {
			substitutions++;
		}

		local_total++;
	}

	if (gapptr) *gapptr = length - local_total;
	return substitutions;
}
#endif

BENCHMARK_CAPTURE(old_bench, dna4_count_mismatches, dna4_count_mismatches);
BENCHMARK_CAPTURE(bench, base, base);
BENCHMARK_CAPTURE(bench, intrinsics_sse, intrinsics_sse);
BENCHMARK_CAPTURE(bench, intrinsics_sse_crazy, intrinsics_sse_crazy);
BENCHMARK_CAPTURE(bench, intrinsics_avx2, intrinsics_avx2);
BENCHMARK_CAPTURE(bench, intrinsics_avx2_derp, intrinsics_avx2_derp);
BENCHMARK_CAPTURE(bench, intrinsics_avx2_crazy, intrinsics_avx2_crazy);

#ifdef __AVX512BW__
BENCHMARK_CAPTURE(bench, intrinsics_avx512_256_crazy, intrinsics_avx512_256_crazy);
BENCHMARK_CAPTURE(bench, intrinsics_avx512_crazy, intrinsics_avx512_crazy);
#endif

BENCHMARK_MAIN();
