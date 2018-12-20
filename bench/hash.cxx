#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <dna.h>
#include <random>
#include <cstring>
#include <emmintrin.h>
#include <tmmintrin.h>

static const size_t LENGTH = 100000;
static const size_t K = 16;
static const size_t seed = 1729;

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

void
escape(void *p)
{
	asm volatile("" : : "g"(p) : "memory");
}

static uint64_t
hash_simple(const char *kmer, uint64_t k)
{
	uint64_t return_value = 0;

	while (k--) {
		char c = *kmer++;
		uint64_t val = 0;
		switch (c) {
			case 'A': val = 0; break;
			case 'C': val = 1; break;
			case 'G': val = 2; break;
			case 'T': val = 3; break;
		}

		return_value <<= 2;
		return_value |= val;
	}

	return return_value;
}

static size_t
hash_twiddle(const char *kmer, size_t k)
{
	size_t return_value = 0;

	while (k--) {
		char c = *kmer++;
		c &= 6;
		c ^= c >> 1;

		return_value <<= 2;
		return_value |= c >> 1;
	}

	return return_value;
}

static size_t
hash_twiddle_length(const char *kmer, size_t K)
{
	size_t return_value = 0;
	size_t k = 0;

	while (k < K) {
		char c = kmer[k++];
		c &= 6;
		c ^= c >> 1;

		return_value <<= 2;
		return_value |= c >> 1;
	}

	return return_value;
}

static size_t
hash_twiddle_length_unordered(const char *kmer, size_t K)
{
	size_t return_value = 0;
	size_t k = 0;

	while (k < K) {
		char c = kmer[k++];
		c &= 6;

		return_value <<= 2;
		return_value |= c >> 1;
	}

	return return_value;
}

static void printadj(const char *addr, size_t nbytes)
{
	static char buf[129] = {0};
	for (size_t i = 0; i < nbytes; i += 2) {
		sprintf(buf + (i/2) * 5, " %c %c ", addr[i], addr[i + 1]);
	}

	fprintf(stderr, "%s\n", buf);
}


static void printhex(const char *addr, size_t nbytes)
{
	static char buf[129] = {0};
	for (size_t i = 0; i < nbytes; i++) {
		sprintf(buf + i * 3, "%02hhx ", (unsigned int)addr[i]);
	}

	fprintf(stderr, "%s", buf);
}

static void printhexshort(const char *addr, size_t nbytes)
{
	static char buf[129] = {0};
	for (size_t i = 0; i < nbytes; i += 2) {
		short z;
		// memcpy(&z, addr + i, sizeof(short));
		memcpy(&z, addr + i + 1, 1);
		memcpy(((char*)&z) + 1, addr + i, 1);
		sprintf(buf + (i/2) * 5, "%04hx ", z);
	}

	fprintf(stderr, "%s", buf);
}



static void printshort(const char *addr, size_t nbytes)
{
	static char buf[129] = {0};
	for (size_t i = 0; i < nbytes; i += 2) {
		short z;
		memcpy(&z, addr + i, sizeof(short));
		sprintf(buf + (i/2) * 5, "%04hu ", z);
	}

	fprintf(stderr, "%s", buf);
}

static size_t
hash_twiddle_simd(const char *kmer, size_t K)
{
	size_t return_value = 0;
	// printadj(kmer, K);

	__m128i a;
	memcpy(&a, kmer, K);
	__m128i all6 = _mm_set1_epi8(6);
	__m128i b = _mm_and_si128(a, all6);

	// printhexshort((char*)&b, sizeof(b)); fprintf(stderr, "\n");

	__m128i shift1 = _mm_set1_epi64x (1);
	__m128i aligned = _mm_sra_epi16(b, shift1);
	// printhexshort((char*)&aligned, sizeof(aligned)); fprintf(stderr, ">\n");

	__m128i multmask =  _mm_set_epi16(64, 16, 4, 1, 64, 16, 4, 1);
	// printhex((char*)&multmask, sizeof(multmask)); fprintf(stderr, "\n");
	__m128i shifted = _mm_mullo_epi16(aligned, multmask);
	// printhexshort((char*)&shifted, sizeof(shifted)); fprintf(stderr, "*\n");

	__m128i allzero = _mm_set1_epi8(0);
	// __m128i sum = _mm_sad_epu8(shifted, allzero); // sum first 8 bytes
	__m128i sum = _mm_hadd_epi16(shifted, allzero); // sum two 16b into one
	// printhexshort((char*)&sum, sizeof(sum)); fprintf(stderr, "+\n");
	__m128i sum2 = _mm_hadd_epi16(sum, allzero); // sum two 16b into one
	// printhexshort((char*)&sum2, sizeof(sum2)); fprintf(stderr, "+\n");

	memcpy(&return_value, &sum2, sizeof(return_value));

	// fprintf(stderr, "%lx\n\n", return_value);
	return return_value;
}


static size_t
hash_table(const char *kmer, size_t K)
{
	static char table[127];
	table['A'] = 0;
	table['C'] = 1;
	table['G'] = 2;
	table['T'] = 3;

	size_t return_value = 0;
	size_t k = 0;

	while (k < K) {
		return_value <<= 2;
		return_value |= table[kmer[k++]];
	}

	return return_value;
}

static void
hash_simple(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t hash = hash_simple(kmer, K);
			escape(&hash);
		}
	}

	free(forward);
}
BENCHMARK(hash_simple);

static void
hash_twiddle(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t hash = hash_twiddle(kmer, K);
			escape(&hash);
		}
	}

	free(forward);
}
BENCHMARK(hash_twiddle);

static void
hash_twiddle_length(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t hash = hash_twiddle_length(kmer, K);
			escape(&hash);
		}
	}

	free(forward);
}
BENCHMARK(hash_twiddle_length);

static void
hash_twiddle_length_unordered(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t hash = hash_twiddle_length_unordered(kmer, K);
			escape(&hash);
		}
	}

	free(forward);
}
BENCHMARK(hash_twiddle_length_unordered);


static void
hash_twiddle_simd(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	// size_t K = 8;

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t hash = hash_twiddle_simd(kmer, K);
			escape(&hash);
		}
	}

	free(forward);
}
BENCHMARK(hash_twiddle_simd);

static void
hash_table(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t hash = hash_table(kmer, K);
			escape(&hash);
		}
	}

	free(forward);
}
BENCHMARK(hash_table);

uint64_t
derp(const char *begin, size_t k)
{
	uint64_t res = 0;
	for (size_t i = 0; i < k; i++) {
		res <<= 2;
		char c = (begin[i] & 6) >> 1;
		res |= c ^ (c >> 1);
	}
	return res;
}

static void
derp(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t hash = derp(kmer, K);
			escape(&hash);
		}
	}

	free(forward);
}
BENCHMARK(derp);

static void
libdna4_hash(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t hash = dna4_hash(kmer, K);
			escape(&hash);
		}
	}

	free(forward);
}
BENCHMARK(libdna4_hash);

static void
libdnax_hash(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t hash = dnax_hash(dnax_hash_table, kmer, K);
			escape(&hash);
		}
	}

	free(forward);
}
BENCHMARK(libdnax_hash);

BENCHMARK_MAIN();
