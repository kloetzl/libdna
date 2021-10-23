#include <emmintrin.h>
#include <err.h>
#include <errno.h>
#include <getopt.h>
#include <immintrin.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <smmintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <tmmintrin.h>
#include <unistd.h>

void
usage(int exit_code);
void
print_seq(uint32_t seed);

static size_t length = 100000;

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
	n ^= n << 8;
	n *= NOISE3;
	n ^= n >> 8;
	return n;
}

extern __attribute__((weak)) // may be supplied by libc
long long
strtonum(
	const char *numstr,
	long long minval,
	long long maxval,
	const char **errstrp);

long long
my_strtonum(
	const char *numstr,
	long long minval,
	long long maxval,
	const char **errstrp)
{
	if (strtonum != NULL) {
		return strtonum(numstr, minval, maxval, errstrp);
	}

	errno = 0;
	if (errstrp) *errstrp = NULL;

	long long ll = strtoll(numstr, NULL, 10);
	if (errno || ll < minval || ll > maxval) {
		if (errstrp != NULL) {
			*errstrp = "invalid";
		}
	}

	return ll;
}

int
main(int argc, char *argv[])
{
	uint64_t seed = 0;

	int check;
	while ((check = getopt(argc, argv, "hl:s:")) != -1) {
		switch (check) {
			case 'l': {
				const char *errstr;

				length = my_strtonum(optarg, 1, INT_MAX, &errstr);
				if (errstr) errx(1, "length is %s: %s", errstr, optarg);

				break;
			}
			case 's': {
				const char *errstr;

				seed = my_strtonum(optarg, 0, INT_MAX, &errstr);
				if (errstr) errx(1, "seed is %s: %s", errstr, optarg);

				break;
			}
			case 'h': usage(EXIT_SUCCESS);
			case '?': // intentional fallthrough
			default: usage(EXIT_FAILURE);
		}
	}

	if (seed == 0) {
		seed = time(NULL);
	}

	print_seq(seed);

	return 0;
}

static char buffer[1 << 16];

typedef __m256i vec_type;

static inline vec_type
chunk_squirrel3(uint32_t n, uint32_t seed)
{

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
	chunk = _mm256_xor_si256(chunk, _mm256_srli_epi32(chunk, 8));
	chunk = _mm256_mullo_epi32(chunk, chunk_noise3);
	chunk = _mm256_xor_si256(chunk, _mm256_srli_epi32(chunk, 8));

	return chunk;
}

void
print_seq(uint32_t seed)
{
	size_t i = 0;
	vec_type nibblecode = _mm256_setr_epi8(
		'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T', 'A', 'C',
		'G', 'T', 'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T',
		'A', 'C', 'G', 'T');

	for (; i < length; i += sizeof(buffer)) {

		for (size_t j = 0; j < sizeof(buffer); j += sizeof(vec_type)) {
			vec_type chunk = chunk_squirrel3(j, seed);

			vec_type mapped = _mm256_shuffle_epi8(nibblecode, chunk);
			memcpy(buffer + j, &mapped, sizeof(vec_type));
		}

		write(1, buffer, sizeof(buffer));
	}
}

void
usage(int exit_code)
{
	static const char str[] = {
		"sim [-l length] [-s seed]\n"
		"Simulate a set of genomic sequences.\n\n"
		"Options:\n"
		"  -h          Display help and exit\n"
		"  -l length   Sequence length\n"
		"  -s seed     Seed the PRNG\n" //
	};

	fprintf(exit_code == EXIT_SUCCESS ? stdout : stderr, str);
	exit(exit_code);
}
