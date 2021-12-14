#include <assert.h>
#include <dna.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	int error_code = 0;
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

double
gc_content(const char *begin, const char *end)
{
	size_t length = end - begin;
	size_t counts[256] = {0};
	dnax_count(counts, begin, end);
	size_t gc = counts['G'] + counts['C'] + counts['S'] + counts['g'] +
				counts['c'] + counts['s'];
	return (double)gc / length;
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

			// Do anythin with the sequence. For instance, calculate GC content.
			printf(">%s\n", seq.name);
			char *end =
				dnax_extract_dna4(seq.data, seq.data + seq.data_len, seq.data);
			seq.data_len = end - seq.data;
			printf("%lf\n", gc_content(seq.data, seq.data + seq.data_len));

			parse_freeseq(&seq);
		}

		parser_close(&parser);
	}
}
