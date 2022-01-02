#include <benchmark/benchmark.h>
#include <cstring>
#include <dna.h>
#include <random>

#ifdef __SSE2__
#include <emmintrin.h>
#include <immintrin.h>
#include <smmintrin.h>
#endif

static const size_t LENGTH = 1000003;

size_t seed = 1729;
size_t invrate;

extern void
gen(char *str, size_t length);

static uint32_t NOISE1 =
	0xb5297a4d; // 0b0110'1000'1110'0011'0001'1101'1010'0100
static uint32_t NOISE2 =
	0x68e31da4; // 0b1011'0101'0010'1001'0111'1010'0100'1101
static uint32_t NOISE3 =
	0x1b56c4e9; // 0b0001'1011'0101'0110'1100'0100'1110'1001

static inline uint32_t
squirrel3(uint32_t n, uint32_t seed)
{
	n *= NOISE1;
	n += seed;
	n ^= n >> 8;
	n += NOISE2;
	n ^= n << 13;
	n *= NOISE3;
	n ^= n >> 17;
	return n;
}

template <class gen_fn>
void
bench(benchmark::State &state, gen_fn fn)
{
	char *forward = (char *)malloc(LENGTH + 1);

	for (auto _ : state) {
		fn(forward, forward + LENGTH, seed);
		benchmark::DoNotOptimize(forward);
	}

	benchmark::DoNotOptimize(forward);

	free(forward);
}

static void
rng(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);

	const char ACGT[4] = {'A', 'C', 'G', 'T'};
	auto base_dist = std::uniform_int_distribution<int>{0, 3};

	auto fill = [&](char *begin, char *end, size_t seed) {
		auto base_rand = std::default_random_engine{
			static_cast<std::default_random_engine::result_type>(seed)};
		auto base_acgt = [&] { return ACGT[base_dist(base_rand)]; };

		for (; begin < end; begin++) {
			*begin = base_acgt();
		}
	};

	for (auto _ : state) {
		fill(forward, forward + LENGTH, seed);
		benchmark::DoNotOptimize(forward);
	}

	free(forward);
}

void
squirrel3_generic(char *begin, char *end, size_t seed)
{
	size_t length = end - begin;
	char ACGT[4] = {'A', 'C', 'G', 'T'};

	size_t i = 0;
	for (; i < (length & ~3); i += 4) {
		uint32_t noise = squirrel3(i, seed);

		char buffer[4]; // TODO: is this the correct byte order?
		buffer[0] = ACGT[(noise >> 24) & 3];
		buffer[1] = ACGT[(noise >> 16) & 3];
		buffer[2] = ACGT[(noise >> 8) & 3];
		buffer[3] = ACGT[(noise >> 0) & 3];
		memcpy(begin + i, buffer, sizeof(buffer));
	}

	size_t rest = length - i;
	if (rest) {
		uint32_t noise = squirrel3(i, seed);

		char buffer[4];
		buffer[0] = ACGT[(noise >> 24) & 3];
		buffer[1] = ACGT[(noise >> 16) & 3];
		buffer[2] = ACGT[(noise >> 8) & 3];
		buffer[3] = ACGT[(noise >> 0) & 3];
		memcpy(begin + i, buffer, rest);
	}
}

#ifdef __SSE4_2__

typedef __m128i vec_type;

static inline vec_type
chunk_squirrel3(uint32_t n, uint32_t seed)
{

	vec_type offset = _mm_setr_epi32(0, 1, 2, 3);
	vec_type chunk_n = _mm_set1_epi32(n);
	vec_type chunk_seed = _mm_set1_epi32(seed);
	vec_type chunk_noise1 = _mm_set1_epi32(NOISE1);
	vec_type chunk_noise2 = _mm_set1_epi32(NOISE2);
	vec_type chunk_noise3 = _mm_set1_epi32(NOISE3);

	vec_type chunk = _mm_add_epi32(chunk_n, offset);
	chunk = _mm_mullo_epi32(chunk, chunk_noise1);
	chunk = _mm_add_epi32(chunk, chunk_seed);
	chunk = _mm_xor_si128(chunk, _mm_srli_epi32(chunk, 8));
	chunk = _mm_add_epi32(chunk, chunk_noise2);
	chunk = _mm_xor_si128(chunk, _mm_slli_epi32(chunk, 13));
	chunk = _mm_mullo_epi32(chunk, chunk_noise3);
	chunk = _mm_xor_si128(chunk, _mm_srli_epi32(chunk, 17));

	return chunk;
}

static void
squirrel3_sse42(char *begin, char *end, size_t seed)
{
	vec_type nibblecode = _mm_setr_epi8(
		'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T', 'A', 'C',
		'G', 'T');

	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	size_t length = end - begin;

	size_t vec_bytes = sizeof(vec_type);
	size_t vec_length = length / vec_bytes;

	size_t vec_offset = 0;
	for (; vec_offset < vec_length; vec_offset++) {

		char *to = begin + vec_offset * vec_bytes;
		vec_type chunk = chunk_squirrel3(vec_offset * vec_bytes, seed);
		vec_type mapped = _mm_shuffle_epi8(nibblecode, chunk);
		memcpy(to, &mapped, vec_bytes);
	}

	size_t rest = length - vec_offset * vec_bytes;
	if (rest) {
		char *to = begin + vec_offset * vec_bytes;
		vec_type chunk = chunk_squirrel3(vec_offset * vec_bytes, seed);
		vec_type mapped = _mm_shuffle_epi8(nibblecode, chunk);
		memcpy(to, &mapped, rest);
	}
}
#endif

#ifdef __AVX2__

static inline __m256i
large_chunk_squirrel3(uint32_t n, uint32_t seed)
{
	typedef __m256i vec_type;

	vec_type offset = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
	vec_type chunk_n = _mm256_set1_epi32(n);
	vec_type chunk_seed = _mm256_set1_epi32(seed);
	vec_type chunk_noise1 = _mm256_set1_epi32(NOISE1);
	vec_type chunk_noise2 = _mm256_set1_epi32(NOISE2);
	vec_type chunk_noise3 = _mm256_set1_epi32(NOISE3);

	vec_type chunk = _mm256_add_epi32(chunk_n, offset);
	chunk = _mm256_mullo_epi32(chunk, chunk_noise1);
	chunk = _mm256_add_epi32(chunk, chunk_seed);
	chunk = _mm256_xor_si256(chunk, _mm256_srli_epi32(chunk, 8));
	chunk = _mm256_add_epi32(chunk, chunk_noise2);
	chunk = _mm256_xor_si256(chunk, _mm256_slli_epi32(chunk, 13));
	chunk = _mm256_mullo_epi32(chunk, chunk_noise3);
	chunk = _mm256_xor_si256(chunk, _mm256_srli_epi32(chunk, 17));

	return chunk;
}

static void
squirrel3_avx2(char *begin, char *end, size_t seed)
{
	typedef __m256i vec_type;

	vec_type nibblecode = _mm256_setr_epi8(
		'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T', 'A', 'C',
		'G', 'T', 'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T',
		'A', 'C', 'G', 'T');

	assert(begin != NULL);
	assert(end != NULL);
	assert(dest != NULL);
	assert(begin <= end);

	size_t length = end - begin;

	size_t vec_bytes = sizeof(vec_type);
	size_t vec_length = length / vec_bytes;

	size_t vec_offset = 0;
	for (; vec_offset < vec_length; vec_offset++) {

		char *to = begin + vec_offset * vec_bytes;
		vec_type chunk = large_chunk_squirrel3(vec_offset * vec_bytes, seed);
		vec_type mapped = _mm256_shuffle_epi8(nibblecode, chunk);
		memcpy(to, &mapped, vec_bytes);
	}

	size_t rest = length - vec_offset * vec_bytes;
	if (rest) {
		char *to = begin + vec_offset * vec_bytes;
		vec_type chunk = large_chunk_squirrel3(vec_offset * vec_bytes, seed);
		vec_type mapped = _mm256_shuffle_epi8(nibblecode, chunk);
		memcpy(to, &mapped, rest);
	}
}
#endif

BENCHMARK(rng);
BENCHMARK_CAPTURE(bench, dna4_fill_random, dna4_fill_random);
BENCHMARK_CAPTURE(bench, squirrel3_generic, squirrel3_generic);

#ifdef __SSE4_2__
BENCHMARK_CAPTURE(bench, squirrel3_sse42, squirrel3_sse42);
#endif

#ifdef __AVX2__
BENCHMARK_CAPTURE(bench, squirrel3_avx2, squirrel3_avx2);
#endif

BENCHMARK_MAIN();
