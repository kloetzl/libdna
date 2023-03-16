#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <cstring>
#include <dna.h>
#include <iostream>
#include <random>

#ifdef __SSE2__
#include <emmintrin.h>
#include <immintrin.h>
#include <nmmintrin.h>
#endif

static const size_t LENGTH = 1000000;

size_t seed = 1729;
size_t invrate = 100;

#define UNLIKELY(X) __builtin_expect((X), 0)

extern void
gen(char *str, size_t length);

extern void
mutate(char *str, size_t length);

template <class Bench_fn>
void
bench(benchmark::State &state, Bench_fn fn)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);
	char *other = (char *)malloc(LENGTH + 1);
	gen(other, LENGTH);
	mutate(other, LENGTH);

	for (auto _ : state) {
		auto d = fn(forward, forward + LENGTH, other);
		benchmark::DoNotOptimize(d);
	}

	free(other);
	free(forward);
}

extern "C" double
dna4_evodist_jc_generic(const char *begin, const char *end, const char *other);

// static void
// libdna4_evodist_k80(benchmark::State &state)
// {
// 	char *forward = (char *)malloc(LENGTH + 1);
// 	gen(forward, LENGTH);
// 	char *other = (char *)malloc(LENGTH + 1);
// 	gen(other, LENGTH);
// 	mutate(other, LENGTH);

// 	while (state.KeepRunning()) {
// 		auto d = dna4_evodist_k80(forward, forward + LENGTH, other, NULL, NULL);
// 		benchmark::DoNotOptimize(d);
// 	}

// 	free(forward);
// 	free(other);
// }
// BENCHMARK(libdna4_evodist_k80);

size_t
noneq(const char *self, const char *other, size_t length)
{
	size_t ret = 0;
	for (size_t i = 0; i < length; i++) {
		if (self[i] != other[i]) {
			ret++;
		}
	}
	return ret;
}

double
base(const char *begin, const char *end, const char *other)
{
	size_t substitutions = 0;
	size_t i = 0;
	size_t length = end - begin;

	for (; i < length; i++) {
		if (begin[i] != other[i]) {
			substitutions++;
		}
	}

	return substitutions;
}

#ifdef __ARM_NEON

#include <arm_neon.h>

size_t
neon(const char *begin, const char *end, const char *other)
{
	size_t substitutions = 0;
	size_t length = end - begin;
	size_t offset = 0;

	typedef uint8x16_t vec_type;
	size_t vec_size = sizeof(vec_type);
	size_t vec_length = length / vec_size;

	size_t vec_offset = 0;
	for (; vec_offset < vec_length; vec_offset++) {
		vec_type b;
		memcpy(&b, begin + vec_offset * vec_size, vec_size);
		vec_type o;
		memcpy(&o, other + vec_offset * vec_size, vec_size);

		const vec_type eq = vceqq_u8(b, o);
		const vec_type all1 = vdupq_n_u8(1);
		const vec_type check = vandq_u8(eq, all1);
		char popcount = vaddvq_u8(check);

		substitutions += vec_size - popcount;
	}

	offset = vec_offset * vec_size;
	for (; offset < length; offset++) {
		if (UNLIKELY(begin[offset] != other[offset])) {
			substitutions++;
		}
	}

	return substitutions;
}

#endif

#ifdef __SSE2__

double
intrinsics_sse2(const char *begin, const char *end, const char *other)
{
	size_t substitutions = 0;
	size_t offset = 0;
	size_t length = end - begin;

	size_t sse2_offset = 0;
	size_t sse2_length = length / sizeof(__m128i);

	substitutions += sse2_length * sizeof(__m128i);
	size_t equal = 0;
	for (; sse2_offset < sse2_length; sse2_offset++) {
		__m128i b;
		memcpy(&b, begin + sse2_offset * sizeof(__m128i), sizeof(b));
		__m128i o;
		memcpy(&o, other + sse2_offset * sizeof(__m128i), sizeof(o));

		__m128i v1 = _mm_cmpeq_epi8(b, o);

		unsigned int vmask = _mm_movemask_epi8(v1);
		equal += __builtin_popcount(vmask);
	}

	substitutions -= equal;

	offset += sse2_offset * sizeof(__m128i);

	for (; offset < length; offset++) {
		if (begin[offset] != other[offset]) {
			substitutions++;
		}
	}

	return substitutions;
}

double
intrinsics_sse2_two(const char *begin, const char *end, const char *other)
{
	size_t substitutions = 0;
	size_t offset = 0;
	size_t length = end - begin;

	size_t sse2_offset = 0;
	size_t sse2_length = length / sizeof(__m128i);

	size_t equal = 0;
	for (; sse2_offset + 1 < sse2_length; sse2_offset++) {
		__m128i b;
		memcpy(&b, begin + sse2_offset * sizeof(__m128i), sizeof(b));
		__m128i o;
		memcpy(&o, other + sse2_offset * sizeof(__m128i), sizeof(o));

		__m128i v1 = _mm_cmpeq_epi8(b, o);

		unsigned int vmask = _mm_movemask_epi8(v1);
		equal += __builtin_popcount(vmask);

		sse2_offset++;
		// second pass
		memcpy(&b, begin + sse2_offset * sizeof(__m128i), sizeof(b));
		memcpy(&o, other + sse2_offset * sizeof(__m128i), sizeof(o));

		v1 = _mm_cmpeq_epi8(b, o);

		vmask = _mm_movemask_epi8(v1);
		equal += __builtin_popcount(vmask);
	}

	substitutions = sse2_offset * sizeof(__m128i) - equal;

	offset += sse2_offset * sizeof(__m128i);

	for (; offset < length; offset++) {
		if (begin[offset] != other[offset]) {
			substitutions++;
		}
	}

	return substitutions;
}

#endif

#ifdef __AVX2__
double
intrinsics_avx2(const char *begin, const char *end, const char *other)
{
	size_t substitutions = 0;
	size_t offset = 0;
	size_t length = end - begin;

	size_t num_bytes = sizeof(__m256i);
	size_t avx2_offset = 0;
	size_t avx2_length = length / num_bytes;

	size_t equal = 0;
	substitutions += avx2_length * num_bytes;
	for (; avx2_offset < avx2_length; avx2_offset++) {
		__m256i b;
		memcpy(&b, begin + avx2_offset * num_bytes, num_bytes);
		__m256i o;
		memcpy(&o, other + avx2_offset * num_bytes, num_bytes);

		__m256i v1 = _mm256_cmpeq_epi8(b, o);

		unsigned int vmask = _mm256_movemask_epi8(v1);
		equal += __builtin_popcount(vmask);
	}

	substitutions -= equal;
	offset += avx2_offset * num_bytes;

	for (; offset < length; offset++) {
		if (begin[offset] != other[offset]) {
			substitutions++;
		}
	}

	return substitutions;
}

double
intrinsics_avx2_two(const char *begin, const char *end, const char *other)
{
	size_t substitutions = 0;
	size_t offset = 0;
	size_t length = end - begin;

	size_t num_bytes = sizeof(__m256i);
	size_t avx2_offset = 0;
	size_t avx2_length = length / num_bytes;

	size_t equal = 0;
	for (; avx2_offset + 1 < avx2_length; avx2_offset++) {
		__m256i b;
		memcpy(&b, begin + avx2_offset * num_bytes, num_bytes);
		__m256i o;
		memcpy(&o, other + avx2_offset * num_bytes, num_bytes);

		__m256i v1 = _mm256_cmpeq_epi8(b, o);

		unsigned int vmask = _mm256_movemask_epi8(v1);
		equal += __builtin_popcount(vmask);

		avx2_offset++;
		memcpy(&b, begin + avx2_offset * num_bytes, num_bytes);
		memcpy(&o, other + avx2_offset * num_bytes, num_bytes);

		v1 = _mm256_cmpeq_epi8(b, o);
		vmask = _mm256_movemask_epi8(v1);

		equal += __builtin_popcount(vmask);
	}

	substitutions += avx2_offset * num_bytes;
	substitutions -= equal;
	offset += avx2_offset * num_bytes;

	for (; offset < length; offset++) {
		if (begin[offset] != other[offset]) {
			substitutions++;
		}
	}

	return substitutions;
}

#endif

#ifdef __AVX512BW__

double
intrinsics_mask_avx512(const char *begin, const char *end, const char *other)
{
	size_t substitutions = 0;
	size_t offset = 0;
	size_t length = end - begin;

	size_t unaligned = 64 - ((uintptr_t)begin % 64);

	size_t sse2_offset = 0;
	size_t sse2_length = (length - unaligned) / sizeof(__m128i);

	substitutions = noneq(begin, other, unaligned);
	offset += unaligned;

	substitutions += sse2_length * sizeof(__m128i);

	__attribute__((aligned(64))) const char *begin_aligned = begin + unaligned;
	const char *other_aligned = other + unaligned;

	size_t equal = 0;
	for (; sse2_offset < sse2_length; sse2_offset++) {
		__m128i b;
		memcpy(&b, begin_aligned + sse2_offset * sizeof(__m128i), sizeof(b));
		__m128i o;
		memcpy(&o, other_aligned + sse2_offset * sizeof(__m128i), sizeof(o));

		// __m128i v1 = _mm_cmpeq_epi8(b, o);

		unsigned int vmask = _mm_cmpeq_epi8_mask(b, o);
		equal += __builtin_popcount(vmask);
	}

	substitutions -= equal;

	offset += sse2_offset * sizeof(__m128i);

	for (; offset < length; offset++) {
		if (begin[offset] != other[offset]) {
			substitutions++;
		}
	}

	return substitutions;
}

double
intrinsics_mask256_avx512(const char *begin, const char *end, const char *other)
{
	size_t substitutions = 0;
	size_t offset = 0;
	size_t length = end - begin;

	size_t vec_offset = 0;
	size_t vec_length = length / sizeof(__m256i);

	substitutions += vec_length * sizeof(__m256i);
	size_t equal = 0;
	for (; vec_offset < vec_length; vec_offset++) {
		__m256i b;
		memcpy(&b, begin + vec_offset * sizeof(__m256i), sizeof(b));
		__m256i o;
		memcpy(&o, other + vec_offset * sizeof(__m256i), sizeof(o));

		// __m256i v1 = _mm_cmpeq_epi8(b, o);

		unsigned int vmask = _mm256_cmpeq_epi8_mask(b, o);
		equal += __builtin_popcount(vmask);
	}

	substitutions -= equal;

	offset += vec_offset * sizeof(__m256i);

	for (; offset < length; offset++) {
		if (begin[offset] != other[offset]) {
			substitutions++;
		}
	}

	return substitutions;
}

double
intrinsics_mask512_avx512(const char *begin, const char *end, const char *other)
{
	size_t substitutions = 0;
	size_t offset = 0;
	size_t length = end - begin;

	size_t vec_offset = 0;
	size_t vec_length = length / sizeof(__m512i);

	size_t equal = 0;
	for (; vec_offset < vec_length; vec_offset++) {
		__m512i b;
		memcpy(&b, begin + vec_offset * sizeof(__m512i), sizeof(b));
		__m512i o;
		memcpy(&o, other + vec_offset * sizeof(__m512i), sizeof(o));

		auto vmask = _mm512_cmpeq_epi8_mask(b, o);
		equal += __builtin_popcountll(vmask);
	}

	substitutions += vec_offset * sizeof(__m512i);
	substitutions -= equal;

	offset += vec_offset * sizeof(__m512i);

	for (; offset < length; offset++) {
		if (begin[offset] != other[offset]) {
			substitutions++;
		}
	}

	return substitutions;
}

#endif

double
k80_twiddle(
	const char *begin,
	const char *end,
	const char *other,
	size_t *transitions_ptr,
	size_t *transversions_ptr)
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

static void
k80_twiddle(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);
	char *other = (char *)malloc(LENGTH + 1);
	gen(other, LENGTH);
	mutate(other, LENGTH);

	while (state.KeepRunning()) {
		auto d = k80_twiddle(forward, forward + LENGTH, other, NULL, NULL);
		benchmark::DoNotOptimize(d);
	}

	free(forward);
	free(other);
}
BENCHMARK(k80_twiddle);

double
k80_twiddle2(
	char *begin,
	char *end,
	const char *other,
	size_t *transitions_ptr,
	size_t *transversions_ptr)
{
	size_t transitions = 0, transversions = 0;
	size_t i = 0;
	size_t length = end - begin;

	for (; i < length; i++) {
		char c = begin[i];
		char d = other[i];
		char f = c ^ d;
		if (f) {
			transitions++;
			if (f == 6 || f == 0x17) {
				transversions++;
			}
		}
	}

	if (transitions_ptr) *transitions_ptr = transitions;
	if (transversions_ptr) *transversions_ptr = transversions;

	// math
	double P = (double)transitions / length;
	double Q = (double)transversions / length;

	double tmp = 1.0 - 2.0 * P - Q;

	return tmp;
}

static void
k80_twiddle2(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);
	char *other = (char *)malloc(LENGTH + 1);
	gen(other, LENGTH);
	mutate(other, LENGTH);

	while (state.KeepRunning()) {
		auto d = k80_twiddle2(forward, forward + LENGTH, other, NULL, NULL);
		benchmark::DoNotOptimize(d);
	}

	free(forward);
	free(other);
}
BENCHMARK(k80_twiddle2);

BENCHMARK_CAPTURE(bench, dnax_count_mismatches, dnax_count_mismatches);
// BENCHMARK_CAPTURE(bench, dna4_evodist_jc_generic, dna4_evodist_jc_generic);
BENCHMARK_CAPTURE(bench, base, base);

#ifdef __SSE2__
BENCHMARK_CAPTURE(bench, intrinsics_sse2, intrinsics_sse2);
BENCHMARK_CAPTURE(bench, intrinsics_sse2_two, intrinsics_sse2_two);
#endif

#ifdef __AVX2__
BENCHMARK_CAPTURE(bench, intrinsics_avx2, intrinsics_avx2);
BENCHMARK_CAPTURE(bench, intrinsics_avx2_two, intrinsics_avx2_two);
#endif

#ifdef __AVX512BW__
BENCHMARK_CAPTURE(bench, intrinsics_mask_avx512, intrinsics_mask_avx512);
BENCHMARK_CAPTURE(bench, intrinsics_mask256_avx512, intrinsics_mask256_avx512);
BENCHMARK_CAPTURE(bench, intrinsics_mask512_avx512, intrinsics_mask512_avx512);
#endif

#ifdef __ARM_NEON
BENCHMARK_CAPTURE(bench, neon, neon);
#endif

BENCHMARK_MAIN();
