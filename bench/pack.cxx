#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <dna.h>
#include <random>
#include <cstring>
#include <emmintrin.h>
#include <tmmintrin.h>
#include <immintrin.h>

static const size_t LENGTH = 100000;
static const size_t K = 20;
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
pack_simple(const char *kmer, uint64_t k)
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
pack_twiddle(const char *kmer, size_t k)
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
pack_twiddle_length(const char *kmer, size_t K)
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
pack_twiddle_length_unordered(const char *kmer, size_t K)
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

	fprintf(stderr, "%s\n", buf);
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

	fprintf(stderr, "%s\n", buf);
}



static void printshort(const char *addr, size_t nbytes)
{
	static char buf[129] = {0};
	for (size_t i = 0; i < nbytes; i += 2) {
		short z;
		memcpy(&z, addr + i, sizeof(short));
		sprintf(buf + (i/2) * 5, "%04hu ", z);
	}

	fprintf(stderr, "%s\n", buf);
}

static size_t
pack_simd_unordered(const char *kmer, size_t K)
{
	size_t return_value = 0;
	int debug = 0;

	for (size_t k = 0; k < K; k += sizeof(__m128i)) {
		size_t rest = K - sizeof(__m128i) > k ? sizeof(__m128i) : K - k;
		if(debug) {printadj(kmer + k, rest);}

		__m128i a;
		memset(&a, 0, sizeof(a));
		memcpy(&a, kmer + k, rest);
		__m128i all6 = _mm_set1_epi8(6);
		__m128i b = _mm_and_si128(a, all6);

		if (debug) printhexshort((char*)&b, sizeof(b));

		__m128i shift1 = _mm_set1_epi64x (1);
		__m128i aligned = _mm_sra_epi16(b, shift1);
		if (debug) printhexshort((char*)&aligned, sizeof(aligned));

		__m128i multmask =  _mm_set_epi16(64, 16, 4, 1, 64, 16, 4, 1);
		__m128i shifted = _mm_mullo_epi16(aligned, multmask);
		if (debug) printhexshort((char*)&shifted, sizeof(shifted));

		__m128i allzero = _mm_set1_epi8(0);
		// __m128i sum = _mm_sad_epu8(shifted, allzero); // sum first 8 bytes
		__m128i sum = _mm_hadd_epi16(shifted, allzero); // sum two 16b into one
		if (debug) printhexshort((char*)&sum, sizeof(sum));
		__m128i sum2 = _mm_hadd_epi16(sum, allzero); // sum two 16b into one
		if (debug) printhexshort((char*)&sum2, sizeof(sum2));

		return_value <<= sizeof(__m128i) * 2;
		size_t temp = 0;
		memcpy(&temp, &sum2, sizeof(return_value));
		return_value |= temp;
		if(debug) {fprintf(stderr, "%lx\n", temp);}
	}

	if(debug) {fprintf(stderr, "%lx\n\n", return_value);}
	return return_value;
}

static size_t
pack_simd_unordered2(const char *kmer, size_t K)
{
	size_t return_value = 0;
	int debug = 0;

	for (size_t k = 0; k < K; k += sizeof(__m128i)) {
		size_t rest = K - sizeof(__m128i) > k ? sizeof(__m128i) : K - k;
		if(debug) {printadj(kmer + k, rest);}

		__m128i a;
		memset(&a, 0, sizeof(a));
		memcpy(&a, kmer + k, rest);
		__m128i all6 = _mm_set1_epi8(6);
		__m128i b = _mm_and_si128(a, all6);

		if(debug) printhexshort((char*)&b, sizeof(b));

		__m128i shift1 = _mm_set1_epi64x (1);
		__m128i aligned = _mm_sra_epi16(b, shift1);
		if(debug) printhexshort((char*)&aligned, sizeof(aligned));

		__m128i multmask =  _mm_set_epi16(64, 16, 4, 1, 64, 16, 4, 1);
		__m128i sum = _mm_madd_epi16(aligned, multmask);
		if(debug) printhexshort((char*)&sum, sizeof(sum));

		__m128i allzero = _mm_set1_epi8(0);
		if(debug) printhexshort((char*)&sum, sizeof(sum));
		__m128i sum2 = _mm_hadd_epi16(sum, allzero); // sum two 16b into one
		if(debug) printhexshort((char*)&sum2, sizeof(sum2));

		return_value <<= sizeof(__m128i) * 2;
		size_t temp = 0;
		memcpy(&temp, &sum2, sizeof(return_value));
		return_value |= temp;
		if(debug) {fprintf(stderr, "%lx\n", temp);}
	}

	if(debug) {fprintf(stderr, "%lx\n\n", return_value);}
	return return_value;
}


static size_t
pack_simd_avx2(const char *kmer, size_t K)
{
	size_t return_value = 0;
	int debug = 0;

	if (debug) {printadj(kmer, K);}

	__m256i a;
	memset(&a, 0, sizeof(a));
	if (K > 32) K = 32;
	memcpy(&a, kmer, K);

	__m256i all6 = _mm256_set1_epi8(6);
	__m256i b = _mm256_and_si256(a, all6);

	__m128i shift1 = _mm_set1_epi64x(1);
	__m256i aligned = _mm256_sra_epi16(b, shift1);
	if (debug) printhexshort((char*)&aligned, sizeof(aligned));

	__m256i shiftmask = _mm256_set_epi32(6, 4, 2, 0, 6, 4, 2, 0);
	__m256i shifted = _mm256_sllv_epi32(aligned, shiftmask);
	if (debug) printhexshort((char*)&shifted, sizeof(shifted));

	__m256i allzero = _mm256_set1_epi8(0);
	__m256i sum = _mm256_hadd_epi32(shifted, allzero);
	if (debug) printhexshort((char*)&sum, sizeof(sum));
	__m256i sum2 = _mm256_hadd_epi32(sum, allzero);
	if (debug) printhexshort((char*)&sum2, sizeof(sum2));

	memcpy(&return_value, &sum2, sizeof(return_value));

	if (debug) {fprintf(stderr, "%lx\n\n", return_value);}
	return return_value;
}


static size_t
pack_table(const char *kmer, size_t K)
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
pack_simple(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t pack = pack_simple(kmer, K);
			escape(&pack);
		}
	}

	free(forward);
}
BENCHMARK(pack_simple);

static void
pack_twiddle(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t pack = pack_twiddle(kmer, K);
			escape(&pack);
		}
	}

	free(forward);
}
BENCHMARK(pack_twiddle);

static void
pack_twiddle_length(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t pack = pack_twiddle_length(kmer, K);
			escape(&pack);
		}
	}

	free(forward);
}
BENCHMARK(pack_twiddle_length);

static void
pack_twiddle_length_unordered(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t pack = pack_twiddle_length_unordered(kmer, K);
			escape(&pack);
		}
	}

	free(forward);
}
BENCHMARK(pack_twiddle_length_unordered);


static void
pack_simd_unordered(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t pack = pack_simd_unordered(kmer, K);
			escape(&pack);
		}
	}

	free(forward);
}
BENCHMARK(pack_simd_unordered);

static void
pack_simd_unordered2(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t pack = pack_simd_unordered2(kmer, K);
			escape(&pack);
		}
	}

	free(forward);
}
BENCHMARK(pack_simd_unordered2);

static void
pack_simd_avx2(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t pack = pack_simd_avx2(kmer, K);
			escape(&pack);
		}
	}

	free(forward);
}
BENCHMARK(pack_simd_avx2);


static void
pack_table(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t pack = pack_table(kmer, K);
			escape(&pack);
		}
	}

	free(forward);
}
BENCHMARK(pack_table);

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
			size_t pack = derp(kmer, K);
			escape(&pack);
		}
	}

	free(forward);
}
BENCHMARK(derp);

static void
libdna4_pack(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t pack = dna4_pack(kmer, K);
			escape(&pack);
		}
	}

	free(forward);
}
BENCHMARK(libdna4_pack);

static void
libdnax_pack(benchmark::State &state)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	while (state.KeepRunning()) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t pack = dnax_pack(dnax_pack_table, kmer, K);
			escape(&pack);
		}
	}

	free(forward);
}
BENCHMARK(libdnax_pack);

BENCHMARK_MAIN();
