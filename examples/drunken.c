#include <assert.h>
#include <dna.h>
#include <err.h>
#include <errno.h>
#include <murmurhash.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint32_t seed = 1729;
static const size_t K = 13;

#define MAX_HEAP_SIZE 500

size_t
ilog(size_t num)
{
	size_t ret = 0;
	while (num) {
		ret++;
		num >>= 1;
	}
	return ret;
}

struct sequence {
	char *name;
	char *data;
	size_t data_len;
};

struct parser {
	FILE *file;
};

int
parser_init(const char *file_name, struct parser *parser)
{
	FILE *file = fopen(file_name, "r");
	if (!file) err(errno, file_name);

	parser->file = file;

	int c = fgetc(file);
	assert(c == '>');

	return 0;
}

int
parse_seq(struct parser *parser, struct sequence *seq)
{
	seq->name = NULL;
	seq->data = NULL;
	seq->data_len = 0;

	char *buffer = NULL;
	size_t n;
	ssize_t bytes;

	bytes = getdelim(&buffer, &n, '\n', parser->file);
	if (bytes == -1) err(errno, NULL);

	buffer[bytes - 1] = '\0';
	seq->name = buffer; // TODO: strip comment

	buffer = NULL;
	n = 0;
	bytes = getdelim(&buffer, &n, '>', parser->file);
	if (bytes == -1) err(errno, NULL);

	buffer[bytes - 1] = '\0';
	seq->data = buffer;
	seq->data_len = bytes;

	return 0;
}

int
parse_hasmoredata(const struct parser *parser)
{
	return !feof(parser->file);
}

void
parse_freeseq(struct sequence *seq)
{
	free(seq->name);
	free(seq->data);
	seq->name = seq->data = NULL;
	seq->data_len = 0;
}

void
parser_close(struct parser *parser)
{
	fclose(parser->file);
}

typedef void (*process_fn)(void *, const char *, size_t);

void
process_canonical_kmers(
	const char *begin,
	const char *end,
	size_t k,
	process_fn callback,
	void *context)
{
	size_t length = end - begin;
	char *revcomp = malloc(length + 1);
	// the revcomp of non-dna4 bases is arbitrary
	dna4_revcomp(begin, end, revcomp);
	revcomp[length] = '\0'; // for debugging purposes

	// [start, stop) is an interval of only dna4 bases
	const char *start = dnax_find_first_of(dnax_to_dna4_table, begin, end);
	while (start < end - k + 1) {
		const char *stop =
			dnax_find_first_not_of(dnax_to_dna4_table, start + 1, end);

		const char *rc = revcomp + (start - begin);
		for (; start < stop - k + 1; start++, rc++) {
			int cmp = memcmp(start, rc, k);
			callback(context, cmp < 0 ? start : rc, k);
		}

		// if stop is end computing stop + 1 is UB.
		start = dnax_find_first_of(dnax_to_dna4_table, stop, end);
	}

	free(revcomp);
}

struct kmer {
	uint64_t hash[2];
	char seq[16];
};

int
kmer_compare(const void *pa, const void *pb)
{
	const struct kmer *kmer_ptr_a = (const struct kmer *)pa;
	const struct kmer *kmer_ptr_b = (const struct kmer *)pb;
	if (kmer_ptr_a->hash[0] != kmer_ptr_b->hash[0]) {
		return kmer_ptr_a->hash[0] > kmer_ptr_b->hash[0] ? 1 : -1;
	}
	if (kmer_ptr_a->hash[1] != kmer_ptr_b->hash[1]) {
		return kmer_ptr_a->hash[1] > kmer_ptr_b->hash[1] ? 1 : -1;
	}
	return 0;
}

struct heap {
	size_t size;
	struct kmer data[MAX_HEAP_SIZE + 1];
};

void
compute_minizers(void *context, const char *kmer, size_t k)
{
	uint64_t hash[2] = {0};
	lmmh_x64_128(kmer, k, seed, hash);
	struct heap *heap = (struct heap *)context;

	struct kmer mini;
	memcpy(mini.hash, hash, sizeof(hash));
	strncpy(mini.seq, kmer, k);
	if (heap->size < MAX_HEAP_SIZE) {
		heap->data[heap->size] = mini;
		heap->size++;
	} else if (kmer_compare(&mini, &heap->data[MAX_HEAP_SIZE - 1]) < 0) {
		heap->data[MAX_HEAP_SIZE - 1] = mini;
		qsort(heap->data, MAX_HEAP_SIZE, sizeof(struct kmer), kmer_compare);
	}
}

// was: 17
#define XLIM 61
// was: 9
#define YLIM 40
#define ARSZ (XLIM * YLIM)

static uint16_t array[ARSZ] = {0};

void
move(int *x, int *y, int direction)
{
	int pos_x = *x;
	int pos_y = *y;
	// 00, 01, 10, 11
	//  A,  C,  G,  T

	int x_diff[4] = {1, 0, -1, 0}; //{-1,1,-1,1};
	int y_diff[4] = {0, 1, 0, -1}; //{-1,-1,1,1};
	pos_x += x_diff[direction & 3];
	pos_y += y_diff[direction & 3];

	if (pos_x < 0) pos_x = XLIM + pos_x;
	if (pos_x >= XLIM) pos_x = pos_x - XLIM;
	if (pos_y < 0) pos_y = YLIM + pos_y;
	if (pos_y >= YLIM) pos_y = pos_y - YLIM;

	*x = pos_x;
	*y = pos_y;
}

uint16_t *
at(int x, int y)
{
	return array + y * XLIM + x;
}

void
drunken_walk(const char *begin, const char *end, int *x, int *y)
{
	for (const char *ptr = begin; ptr < end; ptr++) {
		int temp = (*ptr) >> 1;
		move(x, y, temp);
		uint16_t *pos = at(*x, *y);
		++*pos;
	}
}

void
print_matrix()
{
	const char *upper_half = "▀";
	char rule[XLIM + 3];
	rule[0] = '+';
	memset(rule + 1, '-', XLIM);
	rule[XLIM + 1] = '+';
	rule[XLIM + 2] = '\0';

	printf("%s\n", rule);
	for (size_t i = 0; i < YLIM; i += 2) {
		printf("|");
		for (size_t j = 0; j < XLIM; j++) {
			printf("%c[38;5;%dm", '\x1b', (*at(j, i)) + 16);
			printf("%c[48;5;%dm", '\x1b', (*at(j, i + 1)) + 16);
			printf("%s", upper_half);
			printf("%c[0m", '\x1b');
		}
		printf("|\n");
	}
	printf("%s\n", rule);
}

int
main(int argc, const char **argv)
{
	struct parser parser;
	struct sequence seq;

	for (int i = 1; i < argc; i++) {
		const char *file_name = argv[i];
		parser_init(file_name, &parser);

		while (parse_hasmoredata(&parser)) {
			parse_seq(&parser, &seq);

			printf(">%s\n", seq.name);

			struct heap heap = {0};
			char *new_end =
				dnax_extract_dna4(seq.data, seq.data + seq.data_len, seq.data);
			process_canonical_kmers(
				seq.data, new_end, K, compute_minizers, &heap);
			parse_freeseq(&seq);

			int x = 0;
			int y = 0;

			fprintf(stderr, "heap size: %zu\n", heap.size);
			for (size_t i = 0; i < heap.size; i++) {
				drunken_walk(heap.data[i].seq, heap.data[i].seq + K, &x, &y);
			}

			print_matrix();
		}

		parser_close(&parser);
	}
}
