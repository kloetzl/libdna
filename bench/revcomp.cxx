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

#ifdef __AVX512VBMI__
char *
twiddle_sse42(const char *begin, const char *end, char *__restrict dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	size_t length = end - begin;
	typedef __m128i vec_type;

	size_t vec_bytes = sizeof(vec_type);
	size_t vec_length = length / vec_bytes;

	vec_type all0 = _mm_set1_epi8(0);
	vec_type all2 = _mm_set1_epi8(2);
	vec_type all4 = _mm_set1_epi8(4);
	vec_type all21 = _mm_set1_epi8(21);
	vec_type revmask =
		_mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

	size_t vec_offset = 0;
	for (; vec_offset < vec_length; vec_offset++) {
		size_t offset = vec_offset * vec_bytes;
		// fprintf(stderr, "%zu\n", offset);
		vec_type chunk;
		memcpy(&chunk, begin + offset, vec_bytes);
		// fprintf(stderr, "%16.16s\n", &chunk);

		vec_type reversed = _mm_shuffle_epi8(chunk, revmask);
		// fprintf(stderr, "%16.16s\n", &reversed);

		vec_type check = _mm_and_si128(reversed, all2);
		vec_type is_zero = _mm_cmpeq_epi8(check, all0);
		vec_type blended_mask = _mm_blendv_epi8(all4, all21, is_zero);
		vec_type xored = _mm_xor_si128(blended_mask, reversed);
		// fprintf(stderr, "%16.16s\n", &xored);

		memcpy(dest + length - (vec_offset + 1) * vec_bytes, &xored, vec_bytes);
	}

	size_t offset = vec_offset * vec_bytes;
	for (size_t i = offset; i < length; i++) {
		char c = begin[i];

		dest[length - 1 - i] = c ^= c & 2 ? 4 : 21;
	}

	return dest + length;
}

char *
twiddle_avx2(const char *begin, const char *end, char *__restrict dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	size_t length = end - begin;
	typedef __m256i vec_type;

	size_t vec_bytes = sizeof(vec_type);
	size_t vec_length = length / vec_bytes;

	vec_type all0 = _mm256_set1_epi8(0);
	vec_type all2 = _mm256_set1_epi8(2);
	vec_type all4 = _mm256_set1_epi8(4);
	vec_type all21 = _mm256_set1_epi8(21);
	vec_type revmask = _mm256_set_epi8(
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5,
		6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

	size_t vec_offset = 0;
	for (; vec_offset < vec_length; vec_offset++) {
		size_t offset = vec_offset * vec_bytes;
		// fprintf(stderr, "%zu\n", offset);
		vec_type chunk;
		memcpy(&chunk, begin + offset, vec_bytes);
		// fprintf(stderr, "%16.16s\n", &chunk);

		vec_type halfreversed = _mm256_shuffle_epi8(chunk, revmask);
		vec_type reversed =
			_mm256_permute2x128_si256(halfreversed, halfreversed, 1);
		// fprintf(stderr, "%16.16s\n", &reversed);

		vec_type check = _mm256_and_si256(reversed, all2);
		vec_type is_zero = _mm256_cmpeq_epi8(check, all0);
		vec_type blended_mask = _mm256_blendv_epi8(all4, all21, is_zero);
		vec_type xored = _mm256_xor_si256(blended_mask, reversed);
		// fprintf(stderr, "%16.16s\n", &xored);

		memcpy(dest + length - (vec_offset + 1) * vec_bytes, &xored, vec_bytes);
	}

	size_t offset = vec_offset * vec_bytes;
	for (size_t i = offset; i < length; i++) {
		char c = begin[i];

		dest[length - 1 - i] = c ^= c & 2 ? 4 : 21;
	}

	return dest + length;
}

char *
twiddle_avx2_permute(const char *begin, const char *end, char *__restrict dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	size_t length = end - begin;
	typedef __m256i vec_type;

	size_t vec_bytes = sizeof(vec_type);
	size_t vec_length = length / vec_bytes;

	vec_type all0 = _mm256_set1_epi8(0);
	vec_type all2 = _mm256_set1_epi8(2);
	vec_type all4 = _mm256_set1_epi8(4);
	vec_type all21 = _mm256_set1_epi8(21);
	vec_type revmask = _mm256_set_epi8(
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5,
		6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

	size_t vec_offset = 0;
	for (; vec_offset < vec_length; vec_offset++) {
		size_t offset = vec_offset * vec_bytes;
		// fprintf(stderr, "%zu\n", offset);
		vec_type chunk;
		memcpy(&chunk, begin + offset, vec_bytes);
		fprintf(stderr, "%16.16s\n", &chunk);

		// vec_type halfreversed = _mm256_shuffle_epi8(chunk, revmask);
		vec_type reversed = _mm256_permutexvar_epi8(chunk, 1);
		fprintf(stderr, "%16.16s\n", &reversed);

		vec_type check = _mm256_and_si256(reversed, all2);
		vec_type is_zero = _mm256_cmpeq_epi8(check, all0);
		vec_type blended_mask = _mm256_blendv_epi8(all4, all21, is_zero);
		vec_type xored = _mm256_xor_si256(blended_mask, reversed);
		// fprintf(stderr, "%16.16s\n", &xored);

		memcpy(dest + length - (vec_offset + 1) * vec_bytes, &xored, vec_bytes);
	}

	size_t offset = vec_offset * vec_bytes;
	for (size_t i = offset; i < length; i++) {
		char c = begin[i];

		dest[length - 1 - i] = c ^= c & 2 ? 4 : 21;
	}

	return dest + length;
}
#endif

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

BENCHMARK_CAPTURE(bench, dna4_revcomp, dna4_revcomp);
BENCHMARK_CAPTURE(bench, revcomp_switch, revcomp_switch);
BENCHMARK_CAPTURE(bench, revcomp_table4, revcomp_table4);
BENCHMARK_CAPTURE(bench, twiddle, twiddle);
BENCHMARK_CAPTURE(bench, subtract, subtract);

#ifdef __AVX512VBMI__
BENCHMARK_CAPTURE(bench, twiddle_sse42, twiddle_sse42);
BENCHMARK_CAPTURE(bench, twiddle_avx2, twiddle_avx2);
#endif

BENCHMARK_MAIN();
