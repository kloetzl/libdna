#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <cstring>
#include <dna.hpp>
#include <random>

#ifdef __SSE2__
#include <emmintrin.h>
#include <immintrin.h>
#include <tmmintrin.h>
#endif

static const size_t LENGTH = 100000;
static const size_t K = 31;
size_t seed = 61;
size_t invrate;

extern void
gen(char *str, size_t length);

template <class Pack_fn>
void
bench_template(benchmark::State &state, Pack_fn fn)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);

	for (auto _ : state) {
		for (char *kmer = forward; kmer < forward + LENGTH - K; kmer++) {
			size_t pack = fn(kmer, K);
			benchmark::DoNotOptimize(pack);
		}
	}

	free(forward);
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
printadj(const char *addr, size_t nbytes)
{
	static char buf[129] = {0};
	for (size_t i = 0; i < nbytes; i += 2) {
		sprintf(buf + (i / 2) * 5, " %c %c ", addr[i], addr[i + 1]);
	}

	fprintf(stderr, "%s\n", buf);
}

static void
printhex(const char *addr, size_t nbytes)
{
	static char buf[129] = {0};
	for (size_t i = 0; i < nbytes; i++) {
		sprintf(buf + i * 3, "%02hhx ", (unsigned int)addr[i]);
	}

	fprintf(stderr, "%s\n", buf);
}

static void
printhexshort(const char *addr, size_t nbytes)
{
	static char buf[129] = {0};
	for (size_t i = 0; i < nbytes; i += 2) {
		short z;
		// memcpy(&z, addr + i, sizeof(short));
		memcpy(&z, addr + i + 1, 1);
		memcpy(((char *)&z) + 1, addr + i, 1);
		sprintf(buf + (i / 2) * 5, "%04hx ", z);
	}

	fprintf(stderr, "%s\n", buf);
}

static void
printshort(const char *addr, size_t nbytes)
{
	static char buf[129] = {0};
	for (size_t i = 0; i < nbytes; i += 2) {
		short z;
		memcpy(&z, addr + i, sizeof(short));
		sprintf(buf + (i / 2) * 5, "%04hu ", z);
	}

	fprintf(stderr, "%s\n", buf);
}

static uint64_t
mult_v1(const char *begin, size_t k)
{
	assert(begin != NULL);
	assert(k <= 32);

	uint64_t res = 0;

	char dat[32];
	memset(dat, '_', 32);
	memcpy(dat, begin, k);
	uint32_t ints[8] = {0};

	// read bytes independent of endianess
	for (int i = 0; i < 8; i++) {
		uint8_t *off = (uint8_t *)dat + i * 4;
		// ints[i] = (off[3]<<0) | (off[2]<<8) | (off[1]<<16) | (off[0]<<24);
		ints[i] =
			(off[3] << 24) | (off[2] << 16) | (off[1] << 8) | (off[0] << 0);
	}

	for (int i = 0; i < 8; i++) {
		// printadj(dat + i * 4, 4);
		ints[i] &= 0x6060606;
		ints[i] >>= 1;
		ints[i] ^= ints[i] >> 1;
		ints[i] &= 0x3030303;
		// printhex((char*)&ints[i], 4);
		// ints[i] *= 0x1044010;
		ints[i] *= 0x40100401;

		// printhex((char*)&ints[i], 4);

		uint8_t high_byte = ints[i] >> (3 * 8);
		res |= (uint64_t)high_byte << ((7 - i) * 8);
	}

	// printf("%lx\n", res);

	// uint64_t high_bit = res & 0xaaaaaaaaaaaaaaaalu;
	// high_bit >>= 1;
	// res ^= high_bit;
	// printf("%lx\n", res);

	res >>= (32 - k) * 2;
	// printf("%lx\n", res);

	return res;
}

static uint64_t
mult_v1_1(const char *begin, size_t k)
{
	assert(begin != NULL);
	assert(k <= 32);

	uint64_t res = 0;

	char dat[32];
	memset(dat, '_', 32);
	memcpy(dat, begin, k);
	uint32_t ints[8] = {0};

	// read bytes independent of endianess
	for (int i = 0; i < 8; i++) {
		uint8_t *off = (uint8_t *)dat + i * 4;
		// ints[i] = (off[3]<<0) | (off[2]<<8) | (off[1]<<16) | (off[0]<<24);
		ints[i] =
			(off[3] << 24) | (off[2] << 16) | (off[1] << 8) | (off[0] << 0);
	}

	for (int i = 0; i < 8; i++) {
		// printadj(dat + i * 4, 4);
		ints[i] &= 0x6060606;
		ints[i] >>= 1;
		// printhex((char*)&ints[i], 4);
		ints[i] *= 0x40100401;

		// printhex((char*)&ints[i], 4);

		uint8_t high_byte = ints[i] >> (3 * 8);
		res |= (uint64_t)high_byte << ((7 - i) * 8);
	}

	// printf("%lx\n", res);

	uint64_t high_bit = res & 0xaaaaaaaaaaaaaaaalu;
	high_bit >>= 1;
	res ^= high_bit;
	// printf("%lx\n", res);

	res >>= (32 - k) * 2;
	// printf("%lx\n", res);

	return res;
}

static uint64_t
mult_v2(const char *begin, size_t k)
{
	assert(begin != NULL);
	assert(k <= 32);

	uint64_t res = 0;

	char dat[32];
	memset(dat, '\0', 32);
	memcpy(dat, begin, k);
	uint32_t ints[8] = {0};

	// read bytes independently of endianess
	for (int i = 0; i < 8; i++) {
		uint8_t *off = (uint8_t *)dat + i * 4;
		ints[i] =
			(off[3] << 0) | (off[2] << 8) | (off[1] << 16) | (off[0] << 24);
	}

	for (int i = 0; i < 8; i++) {
		ints[i] &= 0x6060606;
		ints[i] *= 0x820820;

		uint8_t high_byte = ints[i] >> (3 * 8);
		res |= (uint64_t)high_byte << ((7 - i) * 8);
	}

	uint64_t high_bit = res & 0xaaaaaaaaaaaaaaaalu;
	high_bit >>= 1;
	res ^= high_bit;

	res >>= (32 - k) * 2;

	return res;
}

static uint64_t
mult_v3(const char *begin, size_t k)
{
	assert(begin != NULL);
	assert(k <= 32);

	uint64_t res = 0;

	char dat[32];
	memset(dat, '\0', 32);
	memcpy(dat, begin, k);
	uint32_t ints[8] = {0};

	// read bytes independently of endianess
	for (int i = 0; i < 8; i++) {
		uint8_t *off = (uint8_t *)dat + i * 4;
		ints[i] =
			(off[3] << 24) | (off[2] << 16) | (off[1] << 8) | (off[0] << 0);
	}

	for (int i = 0; i < 8; i++) {
		ints[i] &= 0x6060606;
		ints[i] *= 0x820820;

		uint8_t high_byte = ints[i] >> (3 * 8);
		res |= (uint64_t)high_byte << ((7 - i) * 8);
	}

	uint64_t high_bit = res & 0xaaaaaaaaaaaaaaaalu;
	high_bit >>= 1;
	res ^= high_bit;

	res >>= (32 - k) * 2;

	return res;
}

#ifdef __SSE2__
static uint64_t
simd_mult(const char *begin, size_t k)
{
	assert(begin != NULL);
	assert(k <= 32);

	uint64_t res = 0;

	char dat[32];
	memset(dat, '\0', 32);
	memcpy(dat, begin, k);

	__m256i nibblecode = _mm256_setr_epi8(
		'0', 0, '2', 1, 3, '5', '6', 2, '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
		'0', 0, '2', 1, 3, '5', '6', 2, '8', '9', 'a', 'b', 'c', 'd', 'e', 'f');

	__m256i chunk;
	memcpy(&chunk, &dat, sizeof(chunk));
	chunk = _mm256_shuffle_epi8(nibblecode, chunk);

	// GCC likes to replace the following multiplication with shifts and adds.
	chunk = _mm256_mullo_epi32(chunk, _mm256_set1_epi32(0x820820));
	__m256i mask = _mm256_set_epi32(0, 4, 8, 16, 0, 4, 8, 16);
	chunk = _mm256_shuffle_epi8(chunk, mask);

	memcpy(&res, (char *)&chunk + 4, 8);

	res >>= (32 - k) * 2;

	return res;
}
#endif

BENCHMARK_CAPTURE(bench_template, dna4_pack_2bits, dna4_pack_2bits);
// BENCHMARK_CAPTURE(bench_template, dnax_pack_4bits, dnax_pack_4bits);
BENCHMARK_CAPTURE(bench_template, pack_simple, pack_simple);
BENCHMARK_CAPTURE(bench_template, pack_table, pack_table);
BENCHMARK_CAPTURE(bench_template, mult_v1, mult_v1);
BENCHMARK_CAPTURE(bench_template, mult_v1_1, mult_v1_1);
BENCHMARK_CAPTURE(bench_template, mult_v2, mult_v2);
BENCHMARK_CAPTURE(bench_template, mult_v3, mult_v3);

#ifdef __SSE2__
BENCHMARK_CAPTURE(bench_template, simd_mult, simd_mult);
#endif

BENCHMARK_MAIN();
