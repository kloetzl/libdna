#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <cstring>
#include <dna.h>
#include <random>

#ifdef __SSE2__
#include <emmintrin.h>
#include <immintrin.h>
#include <smmintrin.h>
#endif

static const size_t LENGTH = 1000003;
static const size_t MISALIGN_REV = 1;

size_t seed = 1729;
size_t invrate;

extern void
gen(char *str, size_t length);

template <class Pack_fn>
void
bench(benchmark::State &state, Pack_fn fn)
{
	char *forward = (char *)malloc(LENGTH + 1);
	char *reverse = (char *)malloc(LENGTH + 1 + MISALIGN_REV);
	gen(forward, LENGTH);

	for (auto _ : state) {
		fn(forward, forward + LENGTH, reverse + MISALIGN_REV);
		benchmark::DoNotOptimize(reverse);
	}

	benchmark::DoNotOptimize(reverse);

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

	const unsigned char *uforward =
		reinterpret_cast<const unsigned char *>(forward);
	for (size_t k = 0; k < len; k++) {
		reverse[len - k - 1] = table[uforward[k]];
	}

	return reverse;
}

#ifdef __SSE4_2__
char *
shuffle(const char *begin, const char *end, char *__restrict dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	size_t length = end - begin;
	typedef __m128i vec_type;

	size_t vec_bytes = sizeof(vec_type);
	size_t vec_length = length / vec_bytes;

	vec_type revmask =
		_mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	vec_type nibblecode = _mm_setr_epi8(
		'0', 'T', '2', 'G', 'A', '5', '6', 'C', '8', '9', 'a', 'b', 'c', 'd',
		'e', 'f');

	size_t vec_offset = 0;
	for (; vec_offset < vec_length; vec_offset++) {
		vec_type chunk;
		memcpy(&chunk, end - (vec_offset + 1) * vec_bytes, vec_bytes);
		// fprintf(stderr, "%16.16s\n", &chunk);

		vec_type reversed = _mm_shuffle_epi8(chunk, revmask);
		// fprintf(stderr, "%16.16s\n", &reversed);

		vec_type mapped = _mm_shuffle_epi8(nibblecode, reversed);
		// fprintf(stderr, "%16.16s\n\n", &mapped);

		size_t offset = vec_offset * vec_bytes;
		memcpy(dest + offset, &mapped, vec_bytes);
	}

	size_t offset = vec_offset * vec_bytes;
	for (size_t i = 0; i < length - offset; i++) {
		char c = begin[i];

		dest[length - 1 - i] = c ^= c & 2 ? 4 : 21;
	}

	return dest + length;
}
#endif

#ifdef __AVX2__
char *
shuffle_avx2(const char *begin, const char *end, char *__restrict dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	size_t length = end - begin;
	typedef __m256i vec_type;

	size_t vec_bytes = sizeof(vec_type);
	size_t vec_length = length / vec_bytes;

	vec_type revmask = _mm256_set_epi8(
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5,
		6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	vec_type nibblecode = _mm256_setr_epi8(
		'0', 'T', '2', 'G', 'A', '5', '6', 'C', '8', '9', 'a', 'b', 'c', 'd',
		'e', 'f', '0', 'T', '2', 'G', 'A', '5', '6', 'C', '8', '9', 'a', 'b',
		'c', 'd', 'e', 'f');

	size_t vec_offset = 0;
	for (; vec_offset < vec_length; vec_offset++) {
		vec_type chunk;
		memcpy(&chunk, end - (vec_offset + 1) * vec_bytes, vec_bytes);
		// fprintf(stderr, "%32.32s\n", &chunk);

		vec_type halfreversed = _mm256_shuffle_epi8(chunk, revmask);
		vec_type reversed =
			_mm256_permute2x128_si256(halfreversed, halfreversed, 1);
		// fprintf(stderr, "%32.32s\n", &reversed);

		vec_type mapped = _mm256_shuffle_epi8(nibblecode, reversed);
		// fprintf(stderr, "%32.32s\n\n", &mapped);

		size_t offset = vec_offset * vec_bytes;
		memcpy(dest + offset, &mapped, vec_bytes);
	}

	size_t offset = vec_offset * vec_bytes;
	for (size_t i = 0; i < length - offset; i++) {
		char c = begin[length - 1 - i];

		dest[offset + i] = c ^= c & 2 ? 4 : 21;
	}

	return dest + length;
}
#endif

// __attribute__((target_clones("avx2", "sse2", "default")))
static char *
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

#ifdef __ARM_NEON

#include <arm_neon.h>

void
twiddle_16_neon(const char *from, char *__restrict to)
{
	typedef uint8x16_t vec_type;

	size_t vec_bytes = sizeof(vec_type);

	const vec_type all0 = vdupq_n_u8(0);
	const vec_type all2 = vdupq_n_u8(2);
	const vec_type all4 = vdupq_n_u8(4);
	const vec_type all21 = vdupq_n_u8(21);
	const static unsigned char revdata[16] = {15, 14, 13, 12, 11, 10, 9, 8,
											  7,  6,  5,  4,  3,  2,  1, 0};
	const vec_type revmask = vld1q_u8(revdata);

	vec_type chunk;
	memcpy(&chunk, from, vec_bytes);

	const vec_type reversed = vqtbl1q_u8(chunk, revmask);
	const vec_type check = vandq_u8(reversed, all2);
	const vec_type is_zero = vceqq_u8(check, all0);
	const vec_type blended_mask = vbslq_u8(is_zero, all21, all4);
	const vec_type xored = veorq_u8(blended_mask, reversed);

	memcpy(to, &xored, vec_bytes);
}

char *
twiddle_neon(const char *begin, const char *end, char *__restrict dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	size_t length = end - begin;
	typedef uint8x16_t vec_type;

	size_t stride = 1;
	size_t vec_bytes = 16; // sizeof(vec_type);
	size_t vec_length = length / vec_bytes;
	vec_length -= vec_length % stride;
	assert(length >= vec_bytes);

	const vec_type all0 = vdupq_n_u8(0);
	const vec_type all2 = vdupq_n_u8(2);
	const vec_type all4 = vdupq_n_u8(4);
	const vec_type all21 = vdupq_n_u8(21);
	const static unsigned char revdata[16] = {15, 14, 13, 12, 11, 10, 9, 8,
											  7,  6,  5,  4,  3,  2,  1, 0};
	const vec_type revmask = vld1q_u8(revdata);

	size_t vec_offset = 0;
	for (; vec_offset < vec_length; vec_offset += stride) {

		const char *from = end - (vec_offset + 1) * vec_bytes;
		char *to = dest + vec_offset * vec_bytes;

		vec_type chunk;
		memcpy(&chunk, from, vec_bytes);

		const vec_type reversed = vqtbl1q_u8(chunk, revmask);
		const vec_type check = vandq_u8(reversed, all2);
		const vec_type is_zero = vceqq_u8(check, all0);
		const vec_type blended_mask = vbslq_u8(is_zero, all21, all4);
		const vec_type xored = veorq_u8(blended_mask, reversed);

		memcpy(to, &xored, vec_bytes);
		// twiddle_16_neon(from - vec_bytes, to + vec_bytes);
	}

	// problematic for size < vec_bytes!
	for (size_t i = 0; i < length - vec_offset * vec_bytes; i += vec_bytes) {
		twiddle_16_neon(begin + i, dest + length - vec_bytes - i);
	}

	return dest + length;
}

inline void
shuffle_16_neon(const char *from, char *__restrict to)
{
	typedef uint8x16_t vec_type;

	size_t vec_bytes = sizeof(vec_type);

	const unsigned char revdata[16] = {15, 14, 13, 12, 11, 10, 9, 8,
									   7,  6,  5,  4,  3,  2,  1, 0};
	const vec_type revmask = vld1q_u8(revdata);
	const static unsigned char nibbledata[16] = {'0', 'T', '2', 'G', 'A', '5',
												 '6', 'C', '8', '9', 'a', 'b',
												 'c', 'd', 'e', 'f'};
	const vec_type nibblecode = vld1q_u8(nibbledata);

	vec_type chunk;
	memcpy(&chunk, from, vec_bytes);

	const vec_type reversed = vqtbl1q_u8(chunk, revmask);
	// TODO: clear higher bits?
	const vec_type xored = vqtbl1q_u8(nibblecode, reversed);

	memcpy(to, &xored, vec_bytes);
}

char *
shuffle_neon(const char *begin, const char *end, char *__restrict dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	size_t length = end - begin;
	typedef uint8x16_t vec_type;

	size_t stride = 1;
	size_t vec_bytes = 16; // sizeof(vec_type);
	size_t vec_length = length / vec_bytes;
	vec_length -= vec_length % stride;
	assert(length >= vec_bytes);

	const unsigned char revdata[16] = {15, 14, 13, 12, 11, 10, 9, 8,
									   7,  6,  5,  4,  3,  2,  1, 0};
	const vec_type revmask = vld1q_u8(revdata);
	const static unsigned char nibbledata[16] = {'0', 'T', '2', 'G', 'A', '5',
												 '6', 'C', '8', '9', 'a', 'b',
												 'c', 'd', 'e', 'f'};
	const vec_type nibblecode = vld1q_u8(nibbledata);
	size_t vec_offset = 0;
	for (; vec_offset < vec_length; vec_offset += stride) {

		const char *from = end - (vec_offset + 1) * vec_bytes;
		char *to = dest + vec_offset * vec_bytes;

		//      shuffle_16_neon(from - vec_bytes, to + vec_bytes);
		vec_type chunk;
		memcpy(&chunk, from, vec_bytes);

		const vec_type reversed = vqtbl1q_u8(chunk, revmask);
		// TODO: clear higher bits?
		const vec_type xored = vqtbl1q_u8(nibblecode, reversed);

		memcpy(to, &xored, vec_bytes);
	}

	// problematic for size < vec_bytes!
	for (size_t i = 0; i < length - vec_offset * vec_bytes; i += vec_bytes) {
		twiddle_16_neon(begin + i, dest + length - vec_bytes - i);
	}

	return dest + length;
}
#endif

#ifdef __SSE4_2__
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
		vec_type chunk;
		memcpy(&chunk, end - (vec_offset + 1) * vec_bytes, vec_bytes);
		// fprintf(stderr, "%16.16s\n", &chunk);

		vec_type reversed = _mm_shuffle_epi8(chunk, revmask);
		// fprintf(stderr, "%16.16s\n", &reversed);

		vec_type check = _mm_and_si128(reversed, all2);
		vec_type is_zero = _mm_cmpeq_epi8(check, all0);
		vec_type blended_mask = _mm_blendv_epi8(all4, all21, is_zero);
		vec_type xored = _mm_xor_si128(blended_mask, reversed);
		// fprintf(stderr, "%16.16s\n", &xored);

		size_t offset = vec_offset * vec_bytes;
		memcpy(dest + offset, &xored, vec_bytes);
	}

	// size_t offset = vec_offset * vec_bytes;
	for (size_t i = 0; i < length - vec_offset * vec_bytes; i++) {
		char c = begin[i];

		dest[length - 1 - i] = c ^= c & 2 ? 4 : 21;
	}

	return dest + length;
}

void
twiddle_16_sse42(const char *from, char *__restrict to)
{
	typedef __m128i vec_type;

	size_t vec_bytes = sizeof(vec_type);

	const vec_type all0 = _mm_set1_epi8(0);
	const vec_type all2 = _mm_set1_epi8(2);
	const vec_type all4 = _mm_set1_epi8(4);
	const vec_type all21 = _mm_set1_epi8(21);
	const vec_type revmask =
		_mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

	vec_type chunk;
	memcpy(&chunk, from, vec_bytes);

	const vec_type reversed = _mm_shuffle_epi8(chunk, revmask);
	const vec_type check = _mm_and_si128(reversed, all2);
	const vec_type is_zero = _mm_cmpeq_epi8(check, all0);
	const vec_type blended_mask = _mm_blendv_epi8(all4, all21, is_zero);
	const vec_type xored = _mm_xor_si128(blended_mask, reversed);

	memcpy(to, &xored, vec_bytes);
}

void
shuffle_16_sse42(const char *from, char *__restrict to)
{
	typedef __m128i vec_type;

	size_t vec_bytes = sizeof(vec_type);

	const vec_type revmask =
		_mm_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	const vec_type nibblecode = _mm_setr_epi8(
		'0', 'T', '2', 'G', 'A', '5', '6', 'C', '8', '9', 'a', 'b', 'c', 'd',
		'e', 'f');

	vec_type chunk;
	memcpy(&chunk, from, vec_bytes);

	const vec_type reversed = _mm_shuffle_epi8(chunk, revmask);
	const vec_type mapped = _mm_shuffle_epi8(nibblecode, reversed);

	memcpy(to, &mapped, vec_bytes);
}

char *
twiddle_double_sse42(const char *begin, const char *end, char *__restrict dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	size_t length = end - begin;
	typedef __m128i vec_type;

	size_t stride = 3;
	size_t vec_bytes = sizeof(vec_type);
	size_t vec_length = length / vec_bytes;
	vec_length -= vec_length % stride;
	assert(length >= vec_bytes);

	size_t vec_offset = 0;
	for (; vec_offset < vec_length; vec_offset += stride) {

		const char *from = end - (vec_offset + 1) * vec_bytes;
		char *to = dest + vec_offset * vec_bytes;

		twiddle_16_sse42(from, to);
		twiddle_16_sse42(from - vec_bytes, to + vec_bytes);
		twiddle_16_sse42(from - vec_bytes * 2, to + vec_bytes * 2);
		// twiddle_16_sse42(from - vec_bytes * 3, to + vec_bytes * 3);
	}

	// problematic for size < vec_bytes!
	for (size_t i = 0; i < length - vec_offset * vec_bytes; i += vec_bytes) {
		twiddle_16_sse42(begin + i, dest + length - vec_bytes - i);
	}

	return dest + length;
}

char *
shuffle_double_sse42(const char *begin, const char *end, char *__restrict dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	size_t length = end - begin;
	typedef __m128i vec_type;

	size_t stride = 2;
	size_t vec_bytes = sizeof(vec_type);
	size_t vec_length = length / vec_bytes;
	vec_length -= vec_length % stride;
	assert(length >= vec_bytes);

	size_t vec_offset = 0;
	for (; vec_offset < vec_length; vec_offset += stride) {

		const char *from = end - (vec_offset + 1) * vec_bytes;
		char *to = dest + vec_offset * vec_bytes;

		shuffle_16_sse42(from, to);
		shuffle_16_sse42(from - vec_bytes, to + vec_bytes);
		// shuffle_16_sse42(from - vec_bytes * 2, to + vec_bytes * 2);
		// shuffle_16_sse42(from - vec_bytes * 3, to + vec_bytes * 3);
	}

	// problematic for size < vec_bytes!
	for (size_t i = 0; i < length - vec_offset * vec_bytes; i += vec_bytes) {
		shuffle_16_sse42(begin + i, dest + length - vec_bytes - i);
	}

	return dest + length;
}

char *
mixed_double_sse42(const char *begin, const char *end, char *__restrict dest)
{
	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	size_t length = end - begin;
	typedef __m128i vec_type;

	size_t stride = 4;
	size_t vec_bytes = sizeof(vec_type);
	size_t vec_length = length / vec_bytes;
	vec_length -= vec_length % stride;
	assert(length >= vec_bytes);

	size_t vec_offset = 0;
	for (; vec_offset < vec_length; vec_offset += stride) {

		const char *from = end - (vec_offset + 1) * vec_bytes;
		char *to = dest + vec_offset * vec_bytes;

		shuffle_16_sse42(from, to);
		twiddle_16_sse42(from - vec_bytes, to + vec_bytes);
		shuffle_16_sse42(from - vec_bytes * 2, to + vec_bytes * 2);
		twiddle_16_sse42(from - vec_bytes * 3, to + vec_bytes * 3);
	}

	// problematic for size < vec_bytes!
	for (size_t i = 0; i < length - vec_offset * vec_bytes; i += vec_bytes) {
		shuffle_16_sse42(begin + i, dest + length - vec_bytes - i);
	}

	return dest + length;
}
#endif

#ifdef __AVX2__
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
		vec_type chunk;
		memcpy(&chunk, end - (vec_offset + 1) * vec_bytes, vec_bytes);
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

		size_t offset = vec_offset * vec_bytes;
		memcpy(dest + offset, &xored, vec_bytes);
	}

	for (size_t i = 0; i < length - vec_offset * vec_bytes; i++) {
		char c = begin[i];

		dest[length - 1 - i] = c ^= c & 2 ? 4 : 21;
	}

	return dest + length;
}

// -mprefer
char *
twiddle_avx2_wide_register(
	const char *begin, const char *end, char *__restrict dest)
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
#endif

#ifdef __AVX512VBMI__
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
	char *reverse = (char *)malloc(LENGTH + 1 + MISALIGN_REV);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		dnax_revcomp(
			dnax_revcomp_table, forward, forward + LENGTH,
			reverse + MISALIGN_REV);
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
	char *reverse = (char *)malloc(LENGTH + 1 + MISALIGN_REV);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		dnax_replace(
			dnax_revcomp_table, forward, forward + LENGTH,
			reverse + MISALIGN_REV);
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
		dest[i] = begin[length - 1 - i] < 4 ? 3 - begin[length - 1 - i] : 4;
	}
}

static void
bwa_bench(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	char *reverse = (char *)malloc(LENGTH + 1 + MISALIGN_REV);
	gen(forward, LENGTH);
	bwa_encode(forward, forward + LENGTH);

	for (auto _ : state) {
		bwa_revcomp(forward, forward + LENGTH, reverse + MISALIGN_REV);
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
#ifdef __SSE4_2__
BENCHMARK_CAPTURE(bench, twiddle_sse42, twiddle_sse42);
BENCHMARK_CAPTURE(bench, twiddle_double_sse42, twiddle_double_sse42);
BENCHMARK_CAPTURE(bench, shuffle_double_sse42, shuffle_double_sse42);
BENCHMARK_CAPTURE(bench, mixed_double_sse42, mixed_double_sse42);
BENCHMARK_CAPTURE(bench, shuffle, shuffle);
#endif
#ifdef __AVX2__
BENCHMARK_CAPTURE(bench, twiddle_avx2, twiddle_avx2);
BENCHMARK_CAPTURE(bench, shuffle_avx2, shuffle_avx2);
#endif
#ifdef __ARM_NEON
BENCHMARK_CAPTURE(bench, twiddle_neon, twiddle_neon);
BENCHMARK_CAPTURE(bench, shuffle_neon, shuffle_neon);
#endif

BENCHMARK_MAIN();
