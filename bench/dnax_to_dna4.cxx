#include "despace_tables.h"

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

extern void
gen(char *str, size_t length);

template <class Pack_fn>
void
bench(benchmark::State &state, Pack_fn fn)
{
	char *forward = (char *)malloc(LENGTH + 1);
	gen(forward, LENGTH);
	char *dest = (char *)malloc(LENGTH + 1);

	for (size_t i = 0; i < LENGTH; i += mutrate) {
		forward[i] = '_';
	}

	for (auto _ : state) {
		auto marker = fn(forward, forward + LENGTH, dest);
		benchmark::DoNotOptimize(marker);
	}

	free(forward);
	free(dest);
}

static char *
table_based(const char *begin, const char *end, char *dest)
{
	return dnax_replace(dnax_to_dna4_table, begin, end, dest);
}

/*

A: 0100 0001
C: 0100 0011
G: 0100 0111
T: 0101 0100

 */
static char *
despace(const char *begin, const char *end, char *dest)
{
	typedef __m128i vec_type;
	size_t vec_size = sizeof(vec_type);
	size_t capped_length = (end - begin) & ~(vec_size - 1);
	size_t pos = 0;

	vec_type mask_low_nibble =
		_mm_setr_epi8(0, 'A', 0, 'C', 'T', 0, 0, 'G', 0, 0, 0, 0, 0, 0, 0, 0);
	for (size_t i = 0; i < capped_length; i += vec_size) {
		vec_type chunk;
		memcpy(&chunk, begin + i, vec_size);

		// fprintf(stderr, "%16.16s\n", &chunk);

		vec_type v1 = _mm_shuffle_epi8(mask_low_nibble, chunk);
		chunk = _mm_and_si128(chunk, _mm_set1_epi8(0x5f)); // uppercase
		vec_type v4 = _mm_cmpeq_epi8(v1, chunk);

		// fprintf(stderr, "%16.16s\n", &chunk);
		// fprintf(stderr, "%16.16s\n", &v4);

		uint64_t mask16 = (~_mm_movemask_epi8(v4)) & 0xffff;
		vec_type x = _mm_shuffle_epi8(
			chunk, *((__m128i *)despace_mask16 + (mask16 & 0x7fff)));

		// fprintf(stderr, "%16.16s\n", &x);

		memcpy(dest + pos, &x, 16);
		pos += 16 - _mm_popcnt_u64(mask16);

		// fprintf(stderr, "\n");
	}

	// finalize
	return table_based(begin + capped_length, end, dest + pos);
}

static char *
smalltable(const char *begin, const char *end, char *dest)
{
	typedef __m128i vec_type;
	size_t vec_size = sizeof(vec_type);
	size_t capped_length = (end - begin) & ~(vec_size - 1);
	size_t pos = 0;

	static const uint64_t table[128] __attribute__((aligned(64))) = {
		0x0706050403020100, 0x0007060504030201, 0x0107060504030200,
		0x0100070605040302, 0x0207060504030100, 0x0200070605040301,
		0x0201070605040300, 0x0201000706050403, 0x0307060504020100,
		0x0300070605040201, 0x0301070605040200, 0x0301000706050402,
		0x0302070605040100, 0x0302000706050401, 0x0302010706050400,
		0x0302010007060504, 0x0407060503020100, 0x0400070605030201,
		0x0401070605030200, 0x0401000706050302, 0x0402070605030100,
		0x0402000706050301, 0x0402010706050300, 0x0402010007060503,
		0x0403070605020100, 0x0403000706050201, 0x0403010706050200,
		0x0403010007060502, 0x0403020706050100, 0x0403020007060501,
		0x0403020107060500, 0x0403020100070605, 0x0507060403020100,
		0x0500070604030201, 0x0501070604030200, 0x0501000706040302,
		0x0502070604030100, 0x0502000706040301, 0x0502010706040300,
		0x0502010007060403, 0x0503070604020100, 0x0503000706040201,
		0x0503010706040200, 0x0503010007060402, 0x0503020706040100,
		0x0503020007060401, 0x0503020107060400, 0x0503020100070604,
		0x0504070603020100, 0x0504000706030201, 0x0504010706030200,
		0x0504010007060302, 0x0504020706030100, 0x0504020007060301,
		0x0504020107060300, 0x0504020100070603, 0x0504030706020100,
		0x0504030007060201, 0x0504030107060200, 0x0504030100070602,
		0x0504030207060100, 0x0504030200070601, 0x0504030201070600,
		0x0504030201000706, 0x0607050403020100, 0x0600070504030201,
		0x0601070504030200, 0x0601000705040302, 0x0602070504030100,
		0x0602000705040301, 0x0602010705040300, 0x0602010007050403,
		0x0603070504020100, 0x0603000705040201, 0x0603010705040200,
		0x0603010007050402, 0x0603020705040100, 0x0603020007050401,
		0x0603020107050400, 0x0603020100070504, 0x0604070503020100,
		0x0604000705030201, 0x0604010705030200, 0x0604010007050302,
		0x0604020705030100, 0x0604020007050301, 0x0604020107050300,
		0x0604020100070503, 0x0604030705020100, 0x0604030007050201,
		0x0604030107050200, 0x0604030100070502, 0x0604030207050100,
		0x0604030200070501, 0x0604030201070500, 0x0604030201000705,
		0x0605070403020100, 0x0605000704030201, 0x0605010704030200,
		0x0605010007040302, 0x0605020704030100, 0x0605020007040301,
		0x0605020107040300, 0x0605020100070403, 0x0605030704020100,
		0x0605030007040201, 0x0605030107040200, 0x0605030100070402,
		0x0605030207040100, 0x0605030200070401, 0x0605030201070400,
		0x0605030201000704, 0x0605040703020100, 0x0605040007030201,
		0x0605040107030200, 0x0605040100070302, 0x0605040207030100,
		0x0605040200070301, 0x0605040201070300, 0x0605040201000703,
		0x0605040307020100, 0x0605040300070201, 0x0605040301070200,
		0x0605040301000702, 0x0605040302070100, 0x0605040302000701,
		0x0605040302010700, 0x0605040302010007};

	vec_type mask_low_nibble =
		_mm_setr_epi8(0, 'A', 0, 'C', 'T', 0, 0, 'G', 0, 0, 0, 0, 0, 0, 0, 0);
	for (size_t i = 0; i < capped_length; i += vec_size) {
		vec_type chunk;
		memcpy(&chunk, begin + i, vec_size);

		// fprintf(stderr, "%16.16s\n", &chunk);

		vec_type v1 = _mm_shuffle_epi8(mask_low_nibble, chunk);
		chunk = _mm_and_si128(chunk, _mm_set1_epi8(0x5f)); // uppercase
		vec_type chunk_hi = _mm_unpackhi_epi64(chunk, chunk);
		vec_type v4 = _mm_cmpeq_epi8(v1, chunk);

		// fprintf(stderr, "%16.16s\n", &chunk);
		// fprintf(stderr, "%16.16s\n", &v4);

		uint64_t mask16 = (~_mm_movemask_epi8(v4)) & 0xffff;
		uint64_t mask16_hi = mask16 >> 8;

		// fprintf(stderr, "%4.4x\n", mask16);

		vec_type chunk_despaced = _mm_shuffle_epi8(
			chunk, _mm_loadl_epi64((__m128i *)&table[mask16 & 0x7F]));
		vec_type chunk_hi_despaced = _mm_shuffle_epi8(
			chunk_hi, _mm_loadl_epi64((__m128i *)&table[mask16_hi & 0x7F]));

		// fprintf(stderr, "%16.16s\n", &chunk_despaced);

		memcpy(dest + pos, &chunk_despaced, 8); // just copy 8 bytes, whatever
		size_t offset = _mm_popcnt_u64(mask16 & 0xff);
		memcpy(dest + pos + offset, &chunk_hi_despaced, 8);
		pos += 16 - _mm_popcnt_u64(mask16);

		// fprintf(stderr, "\n");
	}

	// finalize
	return table_based(begin + capped_length, end, dest + pos);
}

BENCHMARK_CAPTURE(bench, table_based, table_based);
BENCHMARK_CAPTURE(bench, despace, despace);
BENCHMARK_CAPTURE(bench, smalltable, smalltable);

BENCHMARK_MAIN();
