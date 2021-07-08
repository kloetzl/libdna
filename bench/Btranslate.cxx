#include <algorithm>
#include <array>
#include <benchmark/benchmark.h>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <dna.h>
#include <random>

static const size_t LENGTH = 1000000;

size_t seed = 1729;
size_t invrate;

extern void
gen(char *str, size_t length);

static std::array<std::string, 256> triplets;
std::array<std::string, 256>
computeTriplets();

template <class Pack_fn>
void
bench(benchmark::State &state, Pack_fn fn)
{
	char *dna = (char *)malloc(LENGTH + 1);
	char *aa = (char *)malloc(LENGTH + 1);
	gen(dna, LENGTH);

	triplets = computeTriplets();

	for (auto _ : state) {
		fn(dna, dna + LENGTH, aa);
		benchmark::DoNotOptimize(aa);
	}

	benchmark::DoNotOptimize(aa);

	free(aa);
	free(dna);
}

static void
libdnax_replace(const char *begin, const char *end, char *dest)
{
	dnax_replace(dnax_to_dna4_table, begin, end, dest);
}

/*
function generate_table() {
	var reverse_mapping = {
		A: 'AWMRDHVN',
		C: 'CSYB',
		G: 'GK',
		T: 'TU'
	};

	var a = [];
	a.length = 256;
	a.fill(-1);

	for (var y in reverse_mapping) {
		var y_code = y.charCodeAt(0);
		for (var x of reverse_mapping[y]) {
			a[x.charCodeAt(0)] = y_code;
			a[x.toLowerCase().charCodeAt(0)] = y_code;
		}
	}

	return a.join();
}
 */

char *
mapToA(const char *begin, const char *end, char *dest)
{
	static const signed char mapToA[] = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 65, 67, 67, 65, -1, -1, 71,
		65, -1, -1, 71, -1, 65, 65, -1, -1, -1, 65, 67, 84, 84, 65, 65, -1, 67,
		-1, -1, -1, -1, -1, -1, -1, 65, 67, 67, 65, -1, -1, 71, 65, -1, -1, 71,
		-1, 65, 65, -1, -1, -1, 65, 67, 84, 84, 65, 65, -1, 67, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1};

	static const signed char mapToT[] = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 65, 84, 67, 84, -1, -1, 71,
		84, -1, -1, 84, -1, 67, 84, -1, -1, -1, 71, 71, 84, 84, 71, 84, -1, 84,
		-1, -1, -1, -1, -1, -1, -1, 65, 84, 67, 84, -1, -1, 71, 84, -1, -1, 84,
		-1, 67, 84, -1, -1, -1, 71, 71, 84, 84, 71, 84, -1, 84, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1};

	static const char dna2aa[] =
		"KNKNTTTTRSRSIIMIQHQHPPPPRRRRLLLLEDEDAAAAGGGGVVVV*Y*YSSSS*CWCLFLF";

	const char *ptr = begin;

	while (ptr < end) {
		const char *ccp = dnax_find_first_of((char *)mapToA, ptr, end);
		if (ccp == end) break;

		const char *ddp = dnax_find_first_of((char *)mapToA, ccp + 1, end);
		if (ddp == end) break;

		const char *eep = dnax_find_first_of((char *)mapToA, ddp + 1, end);
		if (eep == end) break;
		ptr = eep + 1;

		char triplet[4] = {*ccp, *ddp, *eep, 0};
		char mappedToA[4], mappedToT[4];

		for (int i = 0; i < 3; i++) {
			mappedToA[i] = mapToA[(unsigned char)triplet[i]];
			mappedToT[i] = mapToT[(unsigned char)triplet[i]];
		}
		mappedToA[3] = mappedToT[3] = 0;

		uint64_t packedA = dna4_pack_2bits(mappedToA, 3);
		char aaA = dna2aa[packedA];
		uint64_t packedT = dna4_pack_2bits(mappedToT, 3);
		char aaT = dna2aa[packedT];

		if (/*LIKELY*/ (aaA == aaT)) {
			*dest++ = aaA;
		} else {
			// rare case
			if (memcmp(triplet, "RAY", 3) == 0) {
				*dest++ = 'B';
			} else if (memcmp(triplet, "SAR", 3) == 0) {
				*dest++ = 'Z';
			} else {
				*dest++ = 'X';
			}
		}
	}

	return dest;
}

/*
"TCAG"

var classes = {
	U: 'T',
Weak           W      A, T         W
Strong         S      C, G         S
Amino          M      A, C         K
Keto           K      G, T         M
Purine         R      A, G         Y
Pyrimidine     Y      C, T         R
not A          B      C, G, T      V
not C          D      A, G, T      H
not G          H      A, C, T      D
not T          V      A, C, G      B
any            N      A, C, G, T   N
}

function mapToMin() {
	var reverse_mapping = {
		'0': 'A',
		'1': 'C',
		'2': 'G',
		'3': 'TUW'
	};

	var a = [];
	a.length = 256;
	a.fill(-1);

	for (var y in reverse_mapping) {
		var y_code = y;
		for (var x of reverse_mapping[y]) {
			a[x.charCodeAt(0)] = y_code;
			a[x.toLowerCase().charCodeAt(0)] = y_code;
		}
	}

	return a.join();
}

function mapToT() {
	var reverse_mapping = {
		'0': 'A',
		'1': 'CM',
		'2': 'GSRV',
		'3': 'TUWKYBDHN'
	};

	var a = [];
	a.length = 256;
	a.fill(-1);

	for (var y in reverse_mapping) {
		var y_code = y;
		for (var x of reverse_mapping[y]) {
			a[x.charCodeAt(0)] = y_code;
			a[x.toLowerCase().charCodeAt(0)] = y_code;
		}
	}

	return a.join();
}
 */

char *
mapToA_fast(const char *begin, const char *end, char *dest)
{
	static const signed char mapToA[] = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0,  2,  1,  2,  -1, -1, 2,
		0,  -1, -1, 2,  -1, 0,  2,  -1, -1, -1, 2,  2,  3,  3,  2,  0,  -1, 1,
		-1, -1, -1, -1, -1, -1, -1, 0,  2,  1,  2,  -1, -1, 2,  0,  -1, -1, 2,
		-1, 0,  2,  -1, -1, -1, 2,  2,  3,  3,  2,  0,  -1, 1,  -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1};

	static const signed char mapToT[] = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0,  3,  1,  3,  -1, -1, 2,
		3,  -1, -1, 3,  -1, 1,  3,  -1, -1, -1, 0,  1,  3,  3,  1,  3,  -1, 3,
		-1, -1, -1, -1, -1, -1, -1, 0,  3,  1,  3,  -1, -1, 2,  3,  -1, -1, 3,
		-1, 1,  3,  -1, -1, -1, 0,  1,  3,  3,  1,  3,  -1, 3,  -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1};

	static const char dna2aa[] =
		"KNKNTTTTRSRSIIMIQHQHPPPPRRRRLLLLEDEDAAAAGGGGVVVV*Y*YSSSS*CWCLFLF";

	const char *ptr = begin;

	while (ptr < end) {
		char triplet[4] = {0};
		memcpy(triplet, ptr, 3);

		uint64_t prePackedA = (mapToA[(unsigned char)triplet[0]] << 4) |
							  (mapToA[(unsigned char)triplet[1]] << 2) |
							  mapToA[(unsigned char)triplet[2]];

		uint64_t packedA;
		if (__builtin_expect((int64_t)prePackedA < 0, 0)) {
			// Rare case, the triplet contained a non-nucleotide char.
			const char *ccp = dnax_find_first_of((char *)mapToA, ptr, end);
			if (ccp == end) break;

			const char *ddp = dnax_find_first_of((char *)mapToA, ccp + 1, end);
			if (ddp == end) break;

			const char *eep = dnax_find_first_of((char *)mapToA, ddp + 1, end);
			if (eep == end) break;
			ptr = eep + 1;

			triplet[0] = *ccp;
			triplet[1] = *ddp;
			triplet[2] = *eep;

			packedA = (mapToA[(unsigned char)triplet[0]] << 4) |
					  (mapToA[(unsigned char)triplet[1]] << 2) |
					  mapToA[(unsigned char)triplet[2]];
		} else {
			ptr += 3;
			packedA = prePackedA;
		}

		uint64_t packedT = (mapToT[(unsigned char)triplet[0]] << 4) |
						   (mapToT[(unsigned char)triplet[1]] << 2) |
						   mapToT[(unsigned char)triplet[2]];

		char aaA = dna2aa[packedA];
		char aaT = dna2aa[packedT];

		char aa;
		if (__builtin_expect(aaA == aaT, 1)) {
			aa = aaA;
		} else {
			// rare case
			if (strncasecmp(triplet, "RAY", 3) == 0) {
				aa = 'B';
			} else if (strncasecmp(triplet, "SAR", 3) == 0) {
				aa = 'Z';
			} else {
				aa = 'X';
			}
		}

		*dest++ = aa;
	}

	return dest;
}

/*

void compute_dna2aa()
{
	for (uint64_t idx = 0; idx < (1 << 6); idx++) {
		char triplet[4] = {0};
		dna4_unpack_2bits(triplet, 3, idx);
		char aa[2] = {0};
		dnax_translate(triplet, triplet + 3, aa);
		printf("%c", aa[0]);
	}
	printf("\n");
}

*/

std::array<std::string, 256>
computeTriplets()
{
	std::vector<std::pair<char, const char *>> foo = {
		{'*', "TAR"}, {'!', "TGA"}, {'A', "GCN"}, {'B', "RAY"}, {'C', "TGY"},
		{'D', "GAY"}, {'E', "GAR"}, {'F', "TTY"}, {'G', "GGN"}, {'H', "CAY"},
		{'I', "ATH"}, {'K', "AAR"}, {'L', "CTN"}, {'l', "TTR"}, {'M', "ATG"},
		{'N', "AAY"}, {'P', "CCN"}, {'Q', "CAR"}, {'R', "CGN"}, {'r', "AGR"},
		{'S', "TCN"}, {'s', "AGY"}, {'T', "ACN"}, {'V', "GTN"}, {'W', "TGG"},
		{'X', "NNN"}, {'Y', "TAY"}, {'Z', "SAR"}};

	std::array<std::string, 256> ret;
	for (auto [aa, triplet] : foo) {
		ret[aa] = triplet;
	}

	return ret;
}

const char dnax_pack_4bits_table[] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, 0,  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, 8,  7,  4,  11, -1, -1, 2,  13, -1, -1, 3,
	-1, 12, 15, -1, -1, -1, 10, 6,  1,  1,  14, 9,  -1, 5,  -1, -1, -1, -1, -1,
	-1, -1, 8,  7,  4,  11, -1, -1, 2,  13, -1, -1, 3,  -1, 12, 15, -1, -1, -1,
	10, 6,  1,  1,  14, 9,  -1, 5,  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1};

int
isSubset(char nuclA, char nuclB)
{
	int codeA = dnax_pack_4bits_table[nuclA];
	int codeB = dnax_pack_4bits_table[nuclB];
	return (codeA | codeB) == codeB;
}

char *
hybrid(const char *begin, const char *end, char *dest)
{
	static const signed char mapToA[] = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0,  2,  1,  2,  -1, -1, 2,
		0,  -1, -1, 2,  -1, 0,  2,  -1, -1, -1, 2,  2,  3,  3,  2,  0,  -1, 1,
		-1, -1, -1, -1, -1, -1, -1, 0,  2,  1,  2,  -1, -1, 2,  0,  -1, -1, 2,
		-1, 0,  2,  -1, -1, -1, 2,  2,  3,  3,  2,  0,  -1, 1,  -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1};

	static const char dna2aa[] =
		"KNKNTTTTRSRSIIMIQHQHPPPPRRRRLLLLEDEDAAAAGGGGVVVV*Y*YSSSS*CWCLFLF";

	const char *ptr = begin;

	while (ptr < end) {
		char triplet[4] = {0};
		memcpy(triplet, ptr, 3);

		uint64_t prePackedA = (mapToA[(unsigned char)triplet[0]] << 4) |
							  (mapToA[(unsigned char)triplet[1]] << 2) |
							  mapToA[(unsigned char)triplet[2]];

		uint64_t packedA;
		if (__builtin_expect((int64_t)prePackedA < 0, 0)) {
			// Rare case, the triplet contained a non-nucleotide char.
			const char *ccp = dnax_find_first_of((char *)mapToA, ptr, end);
			if (ccp == end) break;

			const char *ddp = dnax_find_first_of((char *)mapToA, ccp + 1, end);
			if (ddp == end) break;

			const char *eep = dnax_find_first_of((char *)mapToA, ddp + 1, end);
			if (eep == end) break;
			ptr = eep + 1;

			triplet[0] = *ccp;
			triplet[1] = *ddp;
			triplet[2] = *eep;

			packedA = (mapToA[(unsigned char)triplet[0]] << 4) |
					  (mapToA[(unsigned char)triplet[1]] << 2) |
					  mapToA[(unsigned char)triplet[2]];
		} else {
			ptr += 3;
			packedA = prePackedA;
		}

		char likely_aa = dna2aa[packedA];
		const char *likely_aa_triplet = triplets[likely_aa].c_str();

		// is triplet a subset of likely_aa_triplet?
		int check = isSubset(triplet[0], likely_aa_triplet[0]);
		check &= isSubset(triplet[1], likely_aa_triplet[1]);
		check &= isSubset(triplet[2], likely_aa_triplet[2]);

		if (check) {
			*dest++ = likely_aa; // TODO: fixup
		} else {
			*dest++ = 'X';
		}
	}

	return dest;
}

BENCHMARK_CAPTURE(bench, libdnax_replace, libdnax_replace);
BENCHMARK_CAPTURE(bench, dnax_translate, dnax_translate);
BENCHMARK_CAPTURE(bench, hybrid, hybrid);
BENCHMARK_CAPTURE(bench, mapToA, mapToA);
BENCHMARK_CAPTURE(bench, mapToA_fast, mapToA_fast);

BENCHMARK_MAIN();
